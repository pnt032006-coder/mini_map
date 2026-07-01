/*
 * test_graph_standalone.cpp
 * -----------------------------------------------------
 * FILE TEST DOC LAP - khong include Graph.h / Graph.cpp
 * Toan bo cau truc du lieu (MyStack, MyQueue, MinHeap, Graph)
 * duoc dinh nghia lai NGAY TRONG FILE NAY, tu chua hoan toan.
 *
 * Bien dich (CHI CAN 1 FILE DUY NHAT, khong can Graph.cpp):
 *   g++ -std=c++17 -O2 test_graph_standalone.cpp -o test_graph
 *   ./test_graph
 * -----------------------------------------------------
 */
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <cassert>
#include <chrono>
#include <climits>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace std::chrono;

// =====================================================
//                     MY STACK
// =====================================================
template <typename T>
class MyStack {
private:
    vector<T> data;
public:
    void push(const T& value) { data.push_back(value); }
    void pop() { if (!data.empty()) data.pop_back(); }
    T top() const { return data.back(); }
    bool empty() const { return data.empty(); }
    int size() const { return (int)data.size(); }
};

// =====================================================
//                     MY QUEUE
// =====================================================
template <typename T>
class MyQueue {
private:
    struct Node {
        T value;
        Node* next;
    };
    Node* head;
    Node* tail;
    int count;
public:
    MyQueue() : head(nullptr), tail(nullptr), count(0) {}
    ~MyQueue() { while (!empty()) dequeue(); }

    void enqueue(const T& value) {
        Node* node = new Node{ value, nullptr };
        if (tail) tail->next = node;
        else head = node;
        tail = node;
        count++;
    }
    void dequeue() {
        if (!head) return;
        Node* tmp = head;
        head = head->next;
        if (!head) tail = nullptr;
        delete tmp;
        count--;
    }
    T front() const { return head->value; }
    bool empty() const { return head == nullptr; }
    int size() const { return count; }
};

// =====================================================
//                     MIN HEAP
// =====================================================
template <typename T>
class MinHeap {
private:
    vector<T> heap;

    void heapifyUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (heap[idx] < heap[parent]) {
                swap(heap[idx], heap[parent]);
                idx = parent;
            } else break;
        }
    }
    void heapifyDown(int idx) {
        int n = (int)heap.size();
        while (true) {
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;
            int smallest = idx;
            if (left < n && heap[left] < heap[smallest]) smallest = left;
            if (right < n && heap[right] < heap[smallest]) smallest = right;
            if (smallest == idx) break;
            swap(heap[idx], heap[smallest]);
            idx = smallest;
        }
    }
public:
    bool empty() const { return heap.empty(); }
    int size() const { return (int)heap.size(); }

    void push(const T& value) {
        heap.push_back(value);
        heapifyUp((int)heap.size() - 1);
    }
    T top() const { return heap.front(); }
    void pop() {
        if (heap.empty()) return;
        heap[0] = heap.back();
        heap.pop_back();
        if (!heap.empty()) heapifyDown(0);
    }
};

// =====================================================
//                        GRAPH
// =====================================================
struct Edge {
    int to;
    int weight;
};

class Graph {
private:
    int vertexCountVal;
    bool directedGraph;
    unordered_map<string, int> nameToId;
    vector<string> idToName;
    vector<list<Edge>> adj;
    vector<bool> activeFlag;

    int findOrCreateVertex(const string& name) {
        auto it = nameToId.find(name);
        if (it != nameToId.end()) return it->second;
        int id = vertexCountVal++;
        nameToId[name] = id;
        idToName.push_back(name);
        adj.push_back(list<Edge>());
        activeFlag.push_back(true);
        return id;
    }

public:
    explicit Graph(bool directed = false) : vertexCountVal(0), directedGraph(directed) {}

    bool hasVertex(const string& name) const {
        return nameToId.find(name) != nameToId.end();
    }
    int getId(const string& name) const {
        auto it = nameToId.find(name);
        if (it == nameToId.end()) return -1;
        return it->second;
    }
    string getName(int id) const {
        if (id < 0 || id >= (int)idToName.size()) return "";
        return idToName[id];
    }
    int vertexCount() const {
        int cnt = 0;
        for (bool a : activeFlag) if (a) cnt++;
        return cnt;
    }
    int edgeCount() const {
        int cnt = 0;
        for (int i = 0; i < (int)adj.size(); i++) {
            if (!activeFlag[i]) continue;
            cnt += (int)adj[i].size();
        }
        return directedGraph ? cnt : cnt / 2;
    }

    void addVertex(const string& name) { findOrCreateVertex(name); }

    void addEdge(const string& from, const string& to, int weight) {
        int u = findOrCreateVertex(from);
        int v = findOrCreateVertex(to);
        adj[u].push_back({ v, weight });
        if (!directedGraph && u != v) adj[v].push_back({ u, weight });
    }

    bool removeEdge(const string& from, const string& to) {
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

    bool removeVertex(const string& name) {
        auto it = nameToId.find(name);
        if (it == nameToId.end()) return false;
        int id = it->second;
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

    bool loadFromFile(const string& filename) {
        ifstream fin(filename);
        if (!fin.is_open()) return false;
        string line;
        while (getline(fin, line)) {
            size_t firstNonSpace = line.find_first_not_of(" \t\r\n");
            if (firstNonSpace == string::npos) continue;
            if (line[firstNonSpace] == '#') continue;
            istringstream iss(line);
            string a, b; int w;
            if (iss >> a >> b >> w) addEdge(a, b, w);
        }
        fin.close();
        return true;
    }

    void display() const {
        cout << "\n===== DANH SACH KE (ADJACENCY LIST) =====\n";
        cout << "Loai do thi: " << (directedGraph ? "Co huong" : "Vo huong") << "\n";
        cout << "So dia diem: " << vertexCount() << " | So tuyen duong: " << edgeCount() << "\n";
        cout << "-------------------------------------------\n";
        for (int i = 0; i < (int)adj.size(); i++) {
            if (!activeFlag[i]) continue;
            cout << "[" << idToName[i] << "] -> ";
            if (adj[i].empty()) cout << "(khong co duong noi)";
            else {
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

    vector<string> dijkstra(const string& start, const string& end, int& totalWeight) const {
        totalWeight = -1;
        if (!hasVertex(start) || !hasVertex(end)) return {};
        int s = getId(start), e = getId(end);
        int n = (int)adj.size();
        vector<long long> dist(n, LLONG_MAX);
        vector<int> parent(n, -1);
        vector<bool> visited(n, false);
        dist[s] = 0;
        MinHeap<pair<long long, int>> pq;
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
        if (dist[e] == LLONG_MAX) return {};
        totalWeight = (int)dist[e];
        MyStack<int> st;
        int cur = e;
        while (cur != -1) { st.push(cur); cur = parent[cur]; }
        vector<string> path;
        while (!st.empty()) { path.push_back(idToName[st.top()]); st.pop(); }
        return path;
    }

    vector<string> bfsPath(const string& start, const string& end) const {
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
        while (cur != -1) { st.push(cur); cur = parent[cur]; }
        vector<string> path;
        while (!st.empty()) { path.push_back(idToName[st.top()]); st.pop(); }
        return path;
    }

    vector<string> dfsReachable(const string& start) const {
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
                if (activeFlag[edge.to] && !visited[edge.to]) st.push(edge.to);
            }
        }
        return result;
    }

    bool isConnected() const {
        int n = (int)adj.size();
        int startId = -1;
        int activeTotal = 0;
        for (int i = 0; i < n; i++) {
            if (activeFlag[i]) {
                activeTotal++;
                if (startId == -1) startId = i;
            }
        }
        if (activeTotal <= 1) return true;
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
};

// =====================================================
//              5 TEST CASE (theo yeu cau)
// =====================================================

// TEST 1: Them du lieu (dia diem, tuyen duong)
void test_them_du_lieu() {
    cout << "\n===== TEST: test_them_du_lieu() =====\n";
    Graph g;
    g.addEdge("HaNoi", "HaiPhong", 120);
    g.addEdge("HaNoi", "NamDinh", 90);
    g.addVertex("DaNang");

    assert(g.hasVertex("HaNoi"));
    assert(g.hasVertex("HaiPhong"));
    assert(g.hasVertex("NamDinh"));
    assert(g.hasVertex("DaNang"));
    assert(g.vertexCount() == 4);
    assert(g.edgeCount() == 2);

    cout << "Tong dia diem: " << g.vertexCount() << " (mong doi 4)\n";
    cout << "Tong tuyen duong: " << g.edgeCount() << " (mong doi 2)\n";
    cout << ">> PASS\n";
}

// TEST 2: Tim kiem duong di (Dijkstra + BFS)
void test_tim_kiem() {
    cout << "\n===== TEST: test_tim_kiem() =====\n";
    Graph g;
    g.addEdge("A", "B", 1);
    g.addEdge("B", "C", 2);
    g.addEdge("A", "C", 10);

    int totalWeight;
    vector<string> shortest = g.dijkstra("A", "C", totalWeight);
    cout << "Dijkstra A->C, trong so: " << totalWeight << " (mong doi 3)\n";
    assert(!shortest.empty());
    assert(totalWeight == 3);
    assert(shortest.size() == 3);
    assert(shortest[0] == "A" && shortest[2] == "C");

    vector<string> bfs = g.bfsPath("A", "C");
    cout << "BFS A->C, so dinh tren duong di: " << bfs.size() << " (mong doi 2)\n";
    assert(bfs.size() == 2);
    assert(bfs[0] == "A" && bfs[1] == "C");

    cout << ">> PASS\n";
}

// TEST 3: Xoa du lieu (dia diem, tuyen duong)
void test_xoa_du_lieu() {
    cout << "\n===== TEST: test_xoa_du_lieu() =====\n";
    Graph g;
    g.addEdge("A", "B", 5);
    g.addEdge("B", "C", 3);

    bool removedEdge = g.removeEdge("A", "B");
    assert(removedEdge);

    int w;
    vector<string> path = g.dijkstra("A", "C", w);
    cout << "Sau khi xoa canh A-B, duong A->C: " << (path.empty() ? "KHONG TON TAI" : "TON TAI") << " (mong doi KHONG TON TAI)\n";
    assert(path.empty());

    bool removedVertex = g.removeVertex("B");
    assert(removedVertex);
    assert(!g.hasVertex("B"));
    cout << "Sau khi xoa dinh B, con lai: " << g.vertexCount() << " dinh (mong doi 2)\n";
    assert(g.vertexCount() == 2);

    bool removedAgain = g.removeVertex("KhongTonTai");
    assert(!removedAgain);

    cout << ">> PASS\n";
}

// TEST 4: Edge cases - input rong, trung lap
void test_edge_cases() {
    cout << "\n===== TEST: test_edge_cases() =====\n";

    Graph g1;
    int w;
    vector<string> path1 = g1.dijkstra("X", "Y", w);
    assert(path1.empty());
    vector<string> path2 = g1.bfsPath("X", "Y");
    assert(path2.empty());
    assert(g1.isConnected() == true);
    cout << "4.1 Do thi rong: OK\n";

    Graph g2;
    bool loaded = g2.loadFromFile("file_khong_ton_tai_12345.txt");
    assert(!loaded);
    cout << "4.2 Tai file khong ton tai: OK\n";

    Graph g3;
    g3.addEdge("A", "B", 5);
    g3.addEdge("A", "B", 5);
    g3.addEdge("A", "B", 2);
    int w3;
    vector<string> path3 = g3.dijkstra("A", "B", w3);
    cout << "4.3 Canh trung lap A-B, Dijkstra chon trong so nho nhat: " << w3 << " (mong doi 2)\n";
    assert(w3 == 2);

    Graph g4;
    g4.addVertex("A");
    g4.addVertex("A");
    g4.addVertex("A");
    cout << "4.4 Them dinh 'A' 3 lan, so dinh: " << g4.vertexCount() << " (mong doi 1)\n";
    assert(g4.vertexCount() == 1);

    Graph g5;
    g5.addEdge("A", "B", 1);
    int w5;
    vector<string> path5 = g5.dijkstra("A", "A", w5);
    cout << "4.5 Dijkstra A->A, trong so: " << w5 << " (mong doi 0)\n";
    assert(w5 == 0);
    assert(path5.size() == 1);

    cout << ">> PASS\n";
}

// TEST 5: Hieu nang voi n = 1000 dinh
void test_hieu_nang() {
    cout << "\n===== TEST: test_hieu_nang() (n = 1000) =====\n";
    Graph g;
    const int N = 1000;

    auto tStart = high_resolution_clock::now();

    for (int i = 0; i < N - 1; i++) {
        g.addEdge("N" + to_string(i), "N" + to_string(i + 1), (i % 10) + 1);
    }
    for (int i = 0; i < N; i += 7) {
        int j = (i + 123) % N;
        g.addEdge("N" + to_string(i), "N" + to_string(j), (i % 5) + 1);
    }

    auto tBuildDone = high_resolution_clock::now();

    int totalWeight;
    vector<string> path = g.dijkstra("N0", "N999", totalWeight);
    auto tDijkstraDone = high_resolution_clock::now();

    vector<string> bfs = g.bfsPath("N0", "N999");
    auto tBfsDone = high_resolution_clock::now();

    vector<string> reachable = g.dfsReachable("N0");
    auto tDfsDone = high_resolution_clock::now();

    bool connected = g.isConnected();
    auto tConnectedDone = high_resolution_clock::now();

    double buildMs    = duration<double, milli>(tBuildDone - tStart).count();
    double dijkstraMs = duration<double, milli>(tDijkstraDone - tBuildDone).count();
    double bfsMs      = duration<double, milli>(tBfsDone - tDijkstraDone).count();
    double dfsMs      = duration<double, milli>(tDfsDone - tBfsDone).count();
    double connectMs  = duration<double, milli>(tConnectedDone - tDfsDone).count();

    cout << "So dinh: " << g.vertexCount() << " | So canh: " << g.edgeCount() << "\n";
    cout << "Thoi gian xay dung do thi : " << buildMs    << " ms\n";
    cout << "Thoi gian Dijkstra (N0->N999): " << dijkstraMs << " ms, tong trong so = " << totalWeight << "\n";
    cout << "Thoi gian BFS (N0->N999)      : " << bfsMs      << " ms, so chang = " << (bfs.empty() ? 0 : (int)bfs.size() - 1) << "\n";
    cout << "Thoi gian DFS tu N0           : " << dfsMs      << " ms, so dinh toi duoc = " << reachable.size() << "\n";
    cout << "Thoi gian kiem tra lien thong : " << connectMs  << " ms, ket qua = " << (connected ? "LIEN THONG" : "NGAT KET NOI") << "\n";

    assert(!path.empty());
    assert(!bfs.empty());
    assert((int)reachable.size() == N);
    assert(connected == true);

    cout << ">> PASS\n";
}

// =====================================================
int main() {
    cout << "#####################################################\n";
    cout << "#   BO TEST DOC LAP - MINI GOOGLE MAPS              #\n";
    cout << "#   (khong phu thuoc Graph.h / Graph.cpp)           #\n";
    cout << "#####################################################\n";

    test_them_du_lieu();
    test_tim_kiem();
    test_xoa_du_lieu();
    test_edge_cases();
    test_hieu_nang();

    cout << "\n=====================================================\n";
    cout << " TAT CA 5 TEST CASE DEU PASS THANH CONG!\n";
    cout << "=====================================================\n";
    return 0;
}
