## Tên ứng dụng
**Mini Google Maps — Tìm đường CLI**
Ứng dụng dòng lệnh mô phỏng bản đồ giao thông thu nhỏ: quản lý mạng lưới địa điểm/tuyến
đường, tìm đường ngắn nhất theo trọng số, tìm đường ít điểm dừng nhất, kiểm tra khả năng
kết nối và duyệt các địa điểm có thể tới được — tất cả dựa trên các cấu trúc dữ liệu tự
cài đặt (không dùng `std::priority_queue`, `std::stack`, `std::queue` có sẵn).

## Cấu trúc dữ liệu sử dụng
- **Graph (Adjacency List)**: dùng để lưu mạng lưới đường đi (đỉnh = địa điểm, cạnh =
  tuyến đường có trọng số) vì danh sách kề tiết kiệm bộ nhớ với đồ thị thưa và cho phép
  duyệt các cạnh kề của một đỉnh với chi phí O(bậc của đỉnh), phù hợp cho Dijkstra/BFS/DFS.
- **Min-Heap (Priority Queue tự cài)**: dùng cho thuật toán **Dijkstra** vì tại mỗi bước
  cần lấy ra đỉnh có khoảng cách tạm thời nhỏ nhất trong O(log n), nhanh hơn nhiều so với
  duyệt tuyến tính tìm min trên toàn bộ tập đỉnh chưa thăm.
- **Stack**: dùng để **truy vết và in lại đường đi** vì thuật toán Dijkstra/BFS lưu `parent`
  của từng đỉnh theo chiều từ đích về nguồn, nên cần đảo ngược thứ tự — đặc tính LIFO của
  stack giải quyết việc đảo chiều này một cách tự nhiên. Stack cũng được dùng để duyệt
  **DFS** (tìm tất cả địa điểm có thể đến được từ 1 điểm).
- **Queue**: dùng cho **BFS** (tìm đường ít điểm dừng nhất) vì đặc tính FIFO của queue đảm
  bảo các đỉnh được duyệt theo đúng thứ tự lớp (level-order), nhờ đó đường đi tìm được luôn
  là đường có ít cạnh nhất. Queue cũng được dùng để **kiểm tra tính liên thông** của bản đồ.

## Compile và chạy
```bash
g++ -std=c++17 src/main.cpp src/Graph.cpp -o app && ./app
```

Biên dịch bộ test (file test hoàn toàn độc lập, không cần các file trong `src/`):
```bash
g++ -std=c++17 test_graph_standalone.cpp -o test_app && ./test_app
```

## Chức năng
1. Tải bản đồ từ file `.txt` (danh sách cạnh + trọng số)
2. Hiển thị bản đồ dạng danh sách kề hoặc dạng liệt kê tuyến đường (ASCII)
3. Tìm đường ngắn nhất giữa 2 điểm theo tổng trọng số (Dijkstra)
4. Tìm đường qua ít điểm dừng nhất, không tính trọng số (BFS)
5. Thêm / xóa địa điểm hoặc tuyến đường
6. Tìm tất cả địa điểm có thể đến được từ 1 điểm xuất phát (DFS)
7. Kiểm tra bản đồ có bị ngắt kết nối hay không

## Test cases
1. **test_them_du_lieu** — thêm địa điểm và tuyến đường mới, kiểm tra số lượng đỉnh/cạnh
   trong đồ thị được cập nhật đúng.
2. **test_tim_kiem** — dựng đồ thị có một đường "ngắn nhưng nặng trọng số" và một đường
   "dài hơn nhưng nhẹ trọng số", kiểm tra Dijkstra chọn đúng đường tổng trọng số nhỏ nhất
   còn BFS chọn đúng đường có ít cạnh nhất — chứng minh 2 thuật toán cho kết quả khác nhau.
3. **test_xoa_du_lieu** — xóa một tuyến đường và một địa điểm, kiểm tra đồ thị cập nhật
   đúng (đường đi bị mất khi cắt kết nối, đỉnh không còn tồn tại sau khi xóa).
4. **test_edge_cases** — các trường hợp biên: đồ thị rỗng, tìm đường giữa 2 điểm không tồn
   tại, tải file bản đồ không tồn tại, thêm cạnh trùng lặp (Dijkstra phải chọn cạnh nhẹ
   nhất trong các cạnh trùng), thêm đỉnh trùng tên nhiều lần, điểm đầu trùng điểm cuối.
5. **test_hieu_nang** — dựng đồ thị với **n = 1000** đỉnh (dạng chuỗi kết hợp một số cạnh
   tắt ngẫu nhiên), đo thời gian chạy Dijkstra, BFS, DFS và kiểm tra liên thông bằng
   `<chrono>` để đánh giá hiệu năng với dữ liệu lớn.

## Cấu trúc file
```
src/
  main.cpp       — Menu và điều khiển (giao diện CLI, gọi các hàm của Graph)
  Graph.h        — Khai báo struct/class + nguyên mẫu hàm (Graph, MinHeap, MyStack, MyQueue)
  Graph.cpp      — Cài đặt chi tiết (Dijkstra, BFS, DFS, load file, thêm/xóa, kiểm tra liên thông)
test_graph_standalone.cpp  — File test độc lập, tự chứa toàn bộ cấu trúc dữ liệu bên trong
banddo.txt                 — File bản đồ mẫu để thử chức năng tải bản đồ
```
