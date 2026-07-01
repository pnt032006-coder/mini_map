/*
 * main.cpp
 * -----------------------------------------------------
 * Đề tài 2 - Mini Google Maps (Tìm đường CLI)
 * Chương trình menu tương tác với người dùng
 * -----------------------------------------------------
 */
#include "Graph.h"
#include <iostream>
#include <string>
#include <limits>

using namespace std;

void inLuaChon() {
    cout << "\n===================================================\n";
    cout << "          MINI GOOGLE MAPS - TIM DUONG CLI\n";
    cout << "===================================================\n";
    cout << " 1. Tai ban do tu file .txt\n";
    cout << " 2. Hien thi ban do (danh sach ke / dang tuyen duong)\n";
    cout << " 3. Tim duong ngan nhat giua 2 diem (Dijkstra)\n";
    cout << " 4. Tim duong it diem dung nhat (BFS)\n";
    cout << " 5. Them / Xoa dia diem hoac tuyen duong\n";
    cout << " 6. Tim tat ca dia diem co the den tu 1 diem (DFS)\n";
    cout << " 7. Kiem tra ban do co bi ngat ket noi khong\n";
    cout << " 0. Thoat chuong trinh\n";
    cout << "===================================================\n";
    cout << "Lua chon cua ban: ";
}

void inDuongDi(const vector<string>& path) {
    if (path.empty()) {
        cout << ">> Khong tim thay duong di phu hop.\n";
        return;
    }
    for (size_t i = 0; i < path.size(); i++) {
        cout << path[i];
        if (i + 1 < path.size()) cout << " -> ";
    }
    cout << "\n";
}

void ngatDong() {
    // don dep bo dem nhap sau khi doc so bang cin >>
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ---------- Chuc nang 1: Tai ban do ----------
void chucNangTaiBanDo(Graph& g) {
    cout << "Nhap ten file ban do (vd: banddo.txt): ";
    string filename;
    getline(cin, filename);
    if (g.loadFromFile(filename)) {
        cout << ">> Da tai ban do thanh cong tu \"" << filename << "\".\n";
        cout << ">> Tong so dia diem: " << g.vertexCount()
             << " | Tong so tuyen duong: " << g.edgeCount() << "\n";
    } else {
        cout << ">> Khong the mo file \"" << filename << "\". Vui long kiem tra lai duong dan.\n";
    }
}

// ---------- Chuc nang 2: Hien thi ban do ----------
void chucNangHienThi(const Graph& g) {
    if (g.vertexCount() == 0) {
        cout << ">> Ban do dang trong. Vui long tai ban do hoac them dia diem truoc.\n";
        return;
    }
    cout << "Chon kieu hien thi: 1) Danh sach ke   2) Dang tuyen duong (ASCII)\n> ";
    int c;
    if (!(cin >> c)) { ngatDong(); return; }
    ngatDong();
    if (c == 2) g.displayAscii();
    else g.display();
}

// ---------- Chuc nang 3: Dijkstra ----------
void chucNangDijkstra(const Graph& g) {
    cout << "Nhap diem bat dau: ";
    string a; getline(cin, a);
    cout << "Nhap diem ket thuc: ";
    string b; getline(cin, b);

    int totalWeight;
    vector<string> path = g.dijkstra(a, b, totalWeight);
    if (path.empty()) {
        cout << ">> Khong tim thay duong di tu \"" << a << "\" den \"" << b << "\".\n";
    } else {
        cout << ">> Duong di ngan nhat (theo trong so): ";
        inDuongDi(path);
        cout << ">> Tong khoang cach / chi phi: " << totalWeight << "\n";
    }
}

// ---------- Chuc nang 4: BFS ----------
void chucNangBFS(const Graph& g) {
    cout << "Nhap diem bat dau: ";
    string a; getline(cin, a);
    cout << "Nhap diem ket thuc: ";
    string b; getline(cin, b);

    vector<string> path = g.bfsPath(a, b);
    if (path.empty()) {
        cout << ">> Khong tim thay duong di tu \"" << a << "\" den \"" << b << "\".\n";
    } else {
        cout << ">> Duong di it diem dung nhat (" << (path.size() - 1) << " chang): ";
        inDuongDi(path);
    }
}

// ---------- Chuc nang 5: Them / Xoa ----------
void chucNangThemXoa(Graph& g) {
    cout << "\n--- THEM / XOA ---\n";
    cout << " a. Them dia diem moi\n";
    cout << " b. Them tuyen duong (canh)\n";
    cout << " c. Xoa tuyen duong (canh)\n";
    cout << " d. Xoa dia diem\n";
    cout << "Lua chon: ";
    string opt;
    getline(cin, opt);
    if (opt.empty()) { cout << ">> Lua chon khong hop le.\n"; return; }

    char c = opt[0];
    if (c == 'a' || c == 'A') {
        cout << "Ten dia diem moi: ";
        string name; getline(cin, name);
        g.addVertex(name);
        cout << ">> Da them dia diem \"" << name << "\".\n";
    } else if (c == 'b' || c == 'B') {
        cout << "Diem xuat phat: "; string from; getline(cin, from);
        cout << "Diem den: ";       string to;   getline(cin, to);
        cout << "Trong so (khoang cach/thoi gian): ";
        int w;
        cin >> w; ngatDong();
        g.addEdge(from, to, w);
        cout << ">> Da them tuyen duong " << from << " <-> " << to << " (" << w << ").\n";
    } else if (c == 'c' || c == 'C') {
        cout << "Diem xuat phat: "; string from; getline(cin, from);
        cout << "Diem den: ";       string to;   getline(cin, to);
        if (g.removeEdge(from, to))
            cout << ">> Da xoa tuyen duong " << from << " <-> " << to << ".\n";
        else
            cout << ">> Khong tim thay tuyen duong nay.\n";
    } else if (c == 'd' || c == 'D') {
        cout << "Ten dia diem can xoa: ";
        string name; getline(cin, name);
        if (g.removeVertex(name))
            cout << ">> Da xoa dia diem \"" << name << "\" va cac tuyen duong lien quan.\n";
        else
            cout << ">> Khong tim thay dia diem nay.\n";
    } else {
        cout << ">> Lua chon khong hop le.\n";
    }
}

// ---------- Chuc nang 6: DFS ----------
void chucNangDFS(const Graph& g) {
    cout << "Nhap diem xuat phat: ";
    string a; getline(cin, a);
    vector<string> result = g.dfsReachable(a);
    if (result.empty()) {
        cout << ">> Dia diem \"" << a << "\" khong ton tai tren ban do.\n";
        return;
    }
    cout << ">> Co the den duoc " << result.size() << " dia diem tu \"" << a << "\":\n   ";
    for (size_t i = 0; i < result.size(); i++) {
        cout << result[i];
        if (i + 1 < result.size()) cout << ", ";
    }
    cout << "\n";
}

// ---------- Chuc nang 7: Kiem tra lien thong ----------
void chucNangKiemTraLienThong(const Graph& g) {
    if (g.vertexCount() == 0) {
        cout << ">> Ban do dang trong.\n";
        return;
    }
    if (g.isConnected()) {
        cout << ">> Ban do LIEN THONG - co the di chuyen giua moi cap dia diem.\n";
    } else {
        cout << ">> Ban do BI NGAT KET NOI - co it nhat 1 dia diem khong the toi duoc tu cac dia diem khac.\n";
    }
}

// ---------------------------------------------------------------
int main() {
    Graph g; // do thi vo huong (duong 2 chieu) mac dinh

    cout << "Chao mung den voi MINI GOOGLE MAPS!\n";
    cout << "Ban co the tai ban do tu file .txt (chuc nang 1) hoac tu them dia diem/tuyen duong (chuc nang 5).\n";

    while (true) {
        inLuaChon();
        int choice;
        if (!(cin >> choice)) {
            ngatDong();
            cout << ">> Vui long nhap so hop le.\n";
            continue;
        }
        ngatDong();

        switch (choice) {
            case 1: chucNangTaiBanDo(g); break;
            case 2: chucNangHienThi(g); break;
            case 3: chucNangDijkstra(g); break;
            case 4: chucNangBFS(g); break;
            case 5: chucNangThemXoa(g); break;
            case 6: chucNangDFS(g); break;
            case 7: chucNangKiemTraLienThong(g); break;
            case 0:
                cout << "Cam on ban da su dung Mini Google Maps. Tam biet!\n";
                return 0;
            default:
                cout << ">> Lua chon khong hop le, vui long thu lai.\n";
        }
    }
    return 0;
}
