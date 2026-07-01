[README.md](https://github.com/user-attachments/files/29536870/README.md)[U# Mini Google Maps — Tìm đường CLI

Dự án C++ mô phỏng một "Google Maps mini" chạy trên dòng lệnh, dùng để quản lý mạng
lưới địa điểm/đường đi và tìm đường bằng các cấu trúc dữ liệu tự cài đặt.

## Cấu trúc dự án

| File            | Nội dung |
|-----------------|----------|
| `Graph.h`       | Khai báo lớp `Graph` (Adjacency List) và 3 cấu trúc dữ liệu tự cài đặt: `MyStack<T>`, `MyQueue<T>`, `MinHeap<T>` |
| `Graph.cpp`     | Cài đặt toàn bộ hàm của `Graph`: load bản đồ, hiển thị, Dijkstra, BFS, DFS, kiểm tra liên thông, thêm/xóa |
| `main.cpp`      | Chương trình CLI dạng menu, gọi các chức năng của `Graph` |
| `test_graph.cpp`| 5 test case: `test_them_du_lieu`, `test_tim_kiem`, `test_xoa_du_lieu`, `test_edge_cases`, `test_hieu_nang` (n=1000) |
| `banddo.txt`    | File bản đồ mẫu (danh sách cạnh + trọng số) để thử chức năng 1 |

## Ánh xạ cấu trúc dữ liệu ⇄ chức năng

| Cấu trúc dữ liệu             | Vai trò |
|-------------------------------|---------|
| `Graph` (Adjacency List)      | Lưu mạng lưới đường đi (đỉnh = địa điểm, cạnh = tuyến đường có trọng số) |
| `MinHeap<T>` (Min-Heap tự cài)| Priority Queue cho thuật toán **Dijkstra** (tìm đường ngắn nhất theo trọng số) |
| `MyStack<T>`                  | Truy vết và in lại đường đi (dùng trong cả Dijkstra và BFS); DFS duyệt bằng stack |
| `MyQueue<T>`                  | **BFS** tìm đường ít điểm dừng nhất; kiểm tra liên thông |

Lưu ý: Min-Heap, Stack, Queue đều được **tự cài đặt thủ công** (không dùng
`std::priority_queue`, `std::stack`, `std::queue` có sẵn của STL) để thể hiện đúng
tinh thần "cấu trúc dữ liệu" của đề tài.

## Biên dịch

```bash
# Chương trình chính (menu CLI)
g++ -std=c++17 -O2 Graph.cpp main.cpp -o mini_maps
./mini_maps

# Bộ test (5 test case)
g++ -std=c++17 -O2 Graph.cpp test_graph.cpp -o test_graph
./test_graph
```

> Lưu ý: `main.cpp` và `test_graph.cpp` **đều có hàm `main()` riêng** nên phải biên
> dịch thành 2 chương trình khác nhau (không được compile chung trong 1 lệnh `g++`).

## Định dạng file bản đồ (.txt)

Mỗi dòng: `TenDiemA TenDiemB TrongSo`

```
HaNoi HaiPhong 120
HaNoi NamDinh 90
NamDinh NinhBinh 40
```

- Dòng bắt đầu bằng `#` hoặc dòng trống sẽ bị bỏ qua (comment).
- Đồ thị mặc định là **vô hướng** (đường đi 2 chiều) — cạnh A-B tự động tạo cả B-A.
- Tên địa điểm không được chứa khoảng trắng (dùng `HaNoi`, `NewYork`, ... thay vì có dấu cách).

## 7 chức năng của chương trình

1. **Tải bản đồ từ file .txt** — đọc danh sách cạnh + trọng số, dựng đồ thị.
2. **Hiển thị bản đồ** — dạng danh sách kề, hoặc dạng liệt kê tuyến đường kiểu "ASCII".
3. **Tìm đường ngắn nhất (Dijkstra)** — theo tổng trọng số nhỏ nhất, dùng Min-Heap.
4. **Tìm đường ít điểm dừng nhất (BFS)** — theo số cạnh (chặng) ít nhất, bỏ qua trọng số.
5. **Thêm/Xóa** — thêm địa điểm, thêm tuyến đường, xóa tuyến đường, xóa địa điểm (kèm xóa các
   tuyến đường liên quan).
6. **DFS** — liệt kê tất cả địa điểm có thể đến được từ 1 điểm xuất phát.
7. **Kiểm tra liên thông** — báo bản đồ có bị chia cắt (có địa điểm không thể đến được) hay không.

## Về 5 test case

- `test_them_du_lieu()` — thêm địa điểm/tuyến đường, kiểm tra số lượng đỉnh/cạnh.
- `test_tim_kiem()` — so sánh kết quả Dijkstra (theo trọng số) và BFS (theo số chặng) trên
  cùng một đồ thị có đường đi "ngắn nhưng nặng" và "dài nhưng nhẹ".
- `test_xoa_du_lieu()` — xóa cạnh, xóa đỉnh, kiểm tra đồ thị cập nhật đúng và đường đi bị mất
  khi cắt kết nối.
- `test_edge_cases()` — đồ thị rỗng, tìm đường giữa 2 đỉnh không tồn tại, tải file không tồn
  tại, cạnh trùng lặp (Dijkstra phải chọn cạnh nhẹ nhất), đỉnh trùng tên, điểm đầu = điểm cuối.
- `test_hieu_nang()` — dựng đồ thị 1000 đỉnh (dạng chuỗi + một số cạnh tắt ngẫu nhiên), đo thời
  gian chạy Dijkstra/BFS/DFS/kiểm tra liên thông bằng `<chrono>`.

Toàn bộ 5 test case đã được biên dịch và chạy thử — **PASS 100%**.
ploading README.md…]()
