/*
 * Graph.cpp
 * -----------------------------------------------------
 * Cài đặt lớp Graph (Adjacency List) và các thuật toán:
 *   - Dijkstra (dùng MinHeap tự cài đặt)
 *   - BFS (dùng MyQueue tự cài đặt)
 *   - DFS (dùng MyStack tự cài đặt)
 * -----------------------------------------------------
 */
#include "Graph.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include <algorithm>

using namespace std;

Graph::Graph(bool directed) : vertexCountVal(0), directedGraph(directed) {}

// ---------------------------------------------------------------
int Graph::findOrCreateVertex(const string& name) {
    auto it = nameToId.find(name);
    if (it != nameToId.end()) return it->second;

    int id = vertexCountVal++;
    nameToId[name] = id;
    idToName.push_back(name);
    adj.push_back(list<Edge>());
    activeFlag.push_back(true);
    return id;
}

bool Graph::hasVertex(const string& name) const {
    return nameToId.find(name) != nameToId.end();
}

int Graph::getId(const string& name) const {
    auto it = nameToId.find(name);
    if (it == nameToId.end()) return -1;
    return it->second;
}

string Graph::getName(int id) const {
    if (id < 0 || id >= (int)idToName.size()) return "";
    return idToName[id];
}

int Graph::vertexCount() const {
    int cnt = 0;
    for (bool a : activeFlag) if (a) cnt++;
    return cnt;
}

int Graph::edgeCount() const {
    int cnt = 0;
    for (int i = 0; i < (int)adj.size(); i++) {
        if (!activeFlag[i]) continue;
        cnt += (int)adj[i].size();
    }
    return directedGraph ? cnt : cnt / 2;
}

void Graph::addVertex(const string& name) {
    findOrCreateVertex(name);
}

void Graph::addEdge(const string& from, const string& to, int weight) {
    int u = findOrCreateVertex(from);
    int v = findOrCreateVertex(to);
    adj[u].push_back({ v, weight });
    if (!directedGraph && u != v) {
        adj[v].push_back({ u, weight });
    }
}

bool Graph::removeEdge(const string& from, const string& to) {
    if (!hasVertex(from) || !hasVertex(to)) return false;
    int u = getId(from), v = getId(to);
    bool removed = false;

    auto it = adj[u].begin();
    while (it != adj[u].end()) {
        if (it->to == v) { it = adj[u].erase(it); removed = true; }
        else ++it;
    }
    if (!directedGraph) {
        auto it2 = adj[v].begin();
        while (it2 != adj[v].end()) {
            if (it2->to == u) it2 = adj[v].erase(it2);
            else ++it2;
        }
    }
    return removed;
}

bool Graph::removeVertex(const string& name) {
    auto it = nameToId.find(name);
    if (it == nameToId.end()) return false;
    int id = it->second;

    // Xoá mọi cạnh trỏ đến id từ các đỉnh khác
    for (int i = 0; i < (int)adj.size(); i++) {
        if (i == id) continue;
        auto e = adj[i].begin();
        while (e != adj[i].end()) {
            if (e->to == id) e = adj[i].erase(e);
            else ++e;
        }
    }
    adj[id].clear();
    activeFlag[id] = false;
    nameToId.erase(it);
    return true;
}

// ---------------------------------------------------------------
bool Graph::loadFromFile(const string& filename) {
    ifstream fin(filename);
    if (!fin.is_open()) return false;

    string line;
    while (getline(fin, line)) {
        // bỏ dòng trống hoặc dòng comment bắt đầu bằng '#'
        size_t firstNonSpace = line.find_first_not_of(" \t\r\n");
        if (firstNonSpace == string::npos) continue;
        if (line[firstNonSpace] == '#') continue;

        istringstream iss(line);
        string a, b;
        int w;
        if (iss >> a >> b >> w) {
            addEdge(a, b, w);
        }
    }
    fin.close();
    return true;
}

// ---------------------------------------------------------------
void Graph::display() const {
    cout << "\n===== DANH SACH KE (ADJACENCY LIST) =====\n";
    cout << "Loai do thi: " << (directedGraph ? "Co huong" : "Vo huong") << "\n";
    cout << "So dia diem: " << vertexCount() << " | So tuyen duong: " << edgeCount() << "\n";
    cout << "-------------------------------------------\n";
    for (int i = 0; i < (int)adj.size(); i++) {
        if (!activeFlag[i]) continue;
        cout << "[" << idToName[i] << "] -> ";
        if (adj[i].empty()) {
            cout << "(khong co duong noi)";
        } else {
            bool first = true;
            for (const Edge& e : adj[i]) {
                if (!activeFlag[e.to]) continue;
                if (!first) cout << ", ";
                cout << idToName[e.to] << "(" << e.weight << ")";
                first = false;
            }
        }
        cout << "\n";
    }
    cout << "-------------------------------------------\n";
}

void Graph::displayAscii() const {
    cout << "\n===== BAN DO DANG SO DO TUYEN DUONG =====\n";
    for (int i = 0; i < (int)adj.size(); i++) {
        if (!activeFlag[i]) continue;
        for (const Edge& e : adj[i]) {
            if (!activeFlag[e.to]) continue;
            if (directedGraph) {
                cout << "   " << idToName[i] << " ---(" << e.weight << ")---> " << idToName[e.to] << "\n";
            } else {
                // đồ thị vô hướng: chỉ in mỗi cạnh 1 lần (i < e.to)
                if (i < e.to) {
                    cout << "   " << idToName[i] << " <---(" << e.weight << ")---> " << idToName[e.to] << "\n";
                }
            }
        }
    }
    cout << "===========================================\n";
}

// ---------------------------------------------------------------
// DIJKSTRA - dùng MinHeap tự cài đặt, truy vết bằng MyStack
vector<string> Graph::dijkstra(const string& start, const string& end, int& totalWeight) const {
    totalWeight = -1;
    if (!hasVertex(start) || !hasVertex(end)) return {};

    int s = getId(start), e = getId(end);
    int n = (int)adj.size();
    vector<long long> dist(n, LLONG_MAX);
    vector<int> parent(n, -1);
    vector<bool> visited(n, false);

    dist[s] = 0;
    MinHeap<pair<long long, int>> pq; // (khoang cach, dinh)
    pq.push({ 0, s });

    while (!pq.empty()) {
        pair<long long, int> cur = pq.top();
        pq.pop();
        long long d = cur.first;
        int u = cur.second;
        if (visited[u]) continue;
        visited[u] = true;
        if (u == e) break;

        for (const Edge& edge : adj[u]) {
            int v = edge.to;
            if (!activeFlag[v] || visited[v]) continue;
            long long nd = d + edge.weight;
            if (nd < dist[v]) {
                dist[v] = nd;
                parent[v] = u;
                pq.push({ nd, v });
            }
        }
    }

    if (dist[e] == LLONG_MAX) return {}; // khong co duong di

    totalWeight = (int)dist[e];

    // Truy vet duong di bang Stack
    MyStack<int> st;
    int cur = e;
    while (cur != -1) {
        st.push(cur);
        cur = parent[cur];
    }
    vector<string> path;
    while (!st.empty()) {
        path.push_back(idToName[st.top()]);
        st.pop();
    }
    return path;
}

// ---------------------------------------------------------------
// BFS - duong di it dinh trung gian nhat (it canh nhat), dung MyQueue
vector<string> Graph::bfsPath(const string& start, const string& end) const {
    if (!hasVertex(start) || !hasVertex(end)) return {};

    int s = getId(start), e = getId(end);
    int n = (int)adj.size();
    vector<bool> visited(n, false);
    vector<int> parent(n, -1);

    MyQueue<int> q;
    visited[s] = true;
    q.enqueue(s);

    while (!q.empty()) {
        int u = q.front();
        q.dequeue();
        if (u == e) break;
        for (const Edge& edge : adj[u]) {
            int v = edge.to;
            if (!activeFlag[v] || visited[v]) continue;
            visited[v] = true;
            parent[v] = u;
            q.enqueue(v);
        }
    }

    if (!visited[e]) return {};

    MyStack<int> st;
    int cur = e;
    while (cur != -1) {
        st.push(cur);
        cur = parent[cur];
    }
    vector<string> path;
    while (!st.empty()) {
        path.push_back(idToName[st.top()]);
        st.pop();
    }
    return path;
}

// ---------------------------------------------------------------
// DFS - tim tat ca dia diem co the den duoc tu 1 diem, dung MyStack
vector<string> Graph::dfsReachable(const string& start) const {
    vector<string> result;
    if (!hasVertex(start)) return result;

    int s = getId(start);
    int n = (int)adj.size();
    vector<bool> visited(n, false);

    MyStack<int> st;
    st.push(s);

    while (!st.empty()) {
        int u = st.top();
        st.pop();
        if (visited[u] || !activeFlag[u]) continue;
        visited[u] = true;
        result.push_back(idToName[u]);
        for (const Edge& edge : adj[u]) {
            if (activeFlag[edge.to] && !visited[edge.to]) {
                st.push(edge.to);
            }
        }
    }
    return result;
}

// ---------------------------------------------------------------
bool Graph::isConnected() const {
    int n = (int)adj.size();
    int startId = -1;
    int activeTotal = 0;
    for (int i = 0; i < n; i++) {
        if (activeFlag[i]) {
            activeTotal++;
            if (startId == -1) startId = i;
        }
    }
    if (activeTotal <= 1) return true; // 0 hoac 1 dinh -> coi la lien thong

    vector<bool> visited(n, false);
    MyQueue<int> q;
    visited[startId] = true;
    q.enqueue(startId);
    int visitedCount = 1;

    while (!q.empty()) {
        int u = q.front();
        q.dequeue();
        for (const Edge& edge : adj[u]) {
            int v = edge.to;
            if (activeFlag[v] && !visited[v]) {
                visited[v] = true;
                visitedCount++;
                q.enqueue(v);
            }
        }
    }
    return visitedCount == activeTotal;
}

vector<string> Graph::allVertexNames() const {
    vector<string> names;
    for (int i = 0; i < (int)idToName.size(); i++) {
        if (activeFlag[i]) names.push_back(idToName[i]);
    }
    return names;
}
