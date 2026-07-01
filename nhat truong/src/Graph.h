/*
 * Graph.h
 * -----------------------------------------------------
 * Đề tài 2 - Mini Google Maps (Tìm đường CLI)
 *
 * Khai báo:
 *   - MyStack<T>   : Stack tự cài đặt (dùng để truy vết đường đi)
 *   - MyQueue<T>   : Queue tự cài đặt (dùng cho BFS)
 *   - MinHeap<T>   : Priority Queue dạng Min-Heap tự cài đặt (dùng cho Dijkstra)
 *   - Graph        : Đồ thị lưu bằng Adjacency List (danh sách kề)
 * -----------------------------------------------------
 */
#ifndef GRAPH_H
#define GRAPH_H

#include <string>
#include <vector>
#include <list>
#include <unordered_map>

// =====================================================
//                     MY STACK
// =====================================================
// Stack tự cài đặt bằng mảng động (vector) - dùng để truy vết đường đi
template <typename T>
class MyStack {
private:
    std::vector<T> data;
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
// Queue tự cài đặt bằng danh sách liên kết đơn - dùng cho BFS
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
// Priority Queue dạng Min-Heap tự cài đặt (không dùng std::priority_queue)
// Dùng cho thuật toán Dijkstra. T cần hỗ trợ operator< và operator>
template <typename T>
class MinHeap {
private:
    std::vector<T> heap;

    void heapifyUp(int idx) {
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (heap[idx] < heap[parent]) {
                std::swap(heap[idx], heap[parent]);
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
            std::swap(heap[idx], heap[smallest]);
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
    int vertexCountVal;                              // tổng số đỉnh đã từng tạo (kể cả đã xoá - tombstone)
    bool directedGraph;                               // true = đồ thị có hướng
    std::unordered_map<std::string, int> nameToId;    // tên -> id (chỉ chứa đỉnh đang active)
    std::vector<std::string> idToName;                // id -> tên
    std::vector<std::list<Edge>> adj;                 // danh sách kề
    std::vector<bool> activeFlag;                     // đỉnh còn tồn tại hay đã bị xoá

    int findOrCreateVertex(const std::string& name);

public:
    explicit Graph(bool directed = false);

    // ----- Quản lý đỉnh / cạnh -----
    bool hasVertex(const std::string& name) const;
    int  getId(const std::string& name) const;        // -1 nếu không tồn tại
    std::string getName(int id) const;
    int  vertexCount() const;                          // số đỉnh đang active
    int  edgeCount() const;                             // số cạnh đang active

    void addVertex(const std::string& name);
    void addEdge(const std::string& from, const std::string& to, int weight);
    bool removeEdge(const std::string& from, const std::string& to);
    bool removeVertex(const std::string& name);

    // ----- Tải bản đồ -----
    bool loadFromFile(const std::string& filename);

    // ----- Hiển thị -----
    void display() const;          // danh sách kề dạng bảng
    void displayAscii() const;     // dạng liệt kê tuyến đường "ASCII"

    // ----- Thuật toán -----
    // Dijkstra: đường đi ngắn nhất theo tổng trọng số. totalWeight = -1 nếu không có đường.
    std::vector<std::string> dijkstra(const std::string& start, const std::string& end, int& totalWeight) const;

    // BFS: đường đi ít điểm dừng (ít cạnh) nhất, không quan tâm trọng số.
    std::vector<std::string> bfsPath(const std::string& start, const std::string& end) const;

    // DFS: tất cả địa điểm có thể đến được từ 1 điểm xuất phát
    std::vector<std::string> dfsReachable(const std::string& start) const;

    // Kiểm tra đồ thị có bị ngắt kết nối không (xét trên các đỉnh còn active)
    bool isConnected() const;

    std::vector<std::string> allVertexNames() const;   // liệt kê tên các đỉnh đang active
};

#endif // GRAPH_H
