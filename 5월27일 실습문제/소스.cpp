#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
using namespace std;
using namespace chrono;

struct Champion {
    string position;
    string name;
    int hp;
    int attack;
    int defense;
};

struct SNode {
    Champion data;
    SNode* next;
    SNode(Champion c) : data(c), next(nullptr) {}
};

struct DNode {
    Champion data;
    DNode* prev;
    DNode* next;
    DNode(Champion c) : data(c), prev(nullptr), next(nullptr) {}
};

class SinglyLinkedList {
public:
    SNode* head;
    int size;

    SinglyLinkedList() : head(nullptr), size(0) {}

    void Insert(Champion c) {
        SNode* newNode = new SNode(c);
        if (!head) {
            head = newNode;
        }
        else {
            SNode* cur = head;
            while (cur->next) cur = cur->next;
            cur->next = newNode;
        }
        size++;
    }

    void PrintAll_SL() {
        SNode* cur = head;
        while (cur) {
            cout << cur->data.position << " "
                << cur->data.name << " "
                << cur->data.hp << " "
                << cur->data.attack << " "
                << cur->data.defense << "\n";
            cur = cur->next;
        }
    }

    void SortByHP_SL() {
        if (!head) return;
        bool swapped;
        do {
            swapped = false;
            SNode* cur = head;
            while (cur->next) {
                if (cur->data.hp > cur->next->data.hp) {
                    Champion temp = cur->data;
                    cur->data = cur->next->data;
                    cur->next->data = temp;
                    swapped = true;
                }
                cur = cur->next;
            }
        } while (swapped);
    }

    Champion FindMaxHP_SL() {
        SNode* cur = head;
        Champion maxChamp = cur->data;
        while (cur) {
            if (cur->data.hp > maxChamp.hp)
                maxChamp = cur->data;
            cur = cur->next;
        }
        return maxChamp;
    }

    ~SinglyLinkedList() {
        SNode* cur = head;
        while (cur) {
            SNode* next = cur->next;
            delete cur;
            cur = next;
        }
    }
};

class DoublyLinkedList {
public:
    DNode* head;
    DNode* tail;
    int size;

    DoublyLinkedList() : head(nullptr), tail(nullptr), size(0) {}

    void Insert(Champion c) {
        DNode* newNode = new DNode(c);
        if (!tail) {
            head = tail = newNode;
        }
        else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        size++;
    }

    void PrintAll_DL() {
        DNode* cur = head;
        while (cur) {
            cout << cur->data.position << " "
                << cur->data.name << " "
                << cur->data.hp << " "
                << cur->data.attack << " "
                << cur->data.defense << "\n";
            cur = cur->next;
        }
    }

    void SortByHP_DL() {
        if (!head) return;
        bool swapped;
        do {
            swapped = false;
            DNode* cur = head;
            while (cur->next) {
                if (cur->data.hp > cur->next->data.hp) {
                    Champion temp = cur->data;
                    cur->data = cur->next->data;
                    cur->next->data = temp;
                    swapped = true;
                }
                cur = cur->next;
            }
        } while (swapped);
    }

    Champion FindMaxHP_DL() {
        DNode* cur = head;
        Champion maxChamp = cur->data;
        while (cur) {
            if (cur->data.hp > maxChamp.hp)
                maxChamp = cur->data;
            cur = cur->next;
        }
        return maxChamp;
    }

    ~DoublyLinkedList() {
        DNode* cur = head;
        while (cur) {
            DNode* next = cur->next;
            delete cur;
            cur = next;
        }
    }
};

void LoadFile(const string& filename, SinglyLinkedList& sl, DoublyLinkedList& dl) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "파일을 열 수 없습니다: " << filename << "\n";
        return;
    }

    Champion c;
    while (fin >> c.position >> c.name >> c.hp >> c.attack >> c.defense) {
        sl.Insert(c);
        dl.Insert(c);
    }
    fin.close();
    cout << "로드 완료. 총 " << sl.size << "개 데이터\n\n";
}

#define MEASURE(label, func) \
{ \
    auto start = high_resolution_clock::now(); \
    func; \
    auto end = high_resolution_clock::now(); \
    auto ms = duration_cast<milliseconds>(end - start).count(); \
    cout << label << " 수행시간: " << ms << " ms\n"; \
}

int main() {
    SinglyLinkedList sl;
    DoublyLinkedList dl;

    LoadFile("test.txt", sl, dl);

    cout << "1. PrintAll 수행시간 비교" << endl;
    MEASURE("PrintAll_SL", sl.PrintAll_SL());
    MEASURE("PrintAll_DL", dl.PrintAll_DL());

    cout << "2. SortByHP 수행시간 비교" << endl;
    MEASURE("SortByHP_SL", sl.SortByHP_SL());
    MEASURE("SortByHP_DL", dl.SortByHP_DL());

    cout << "3. FindMaxHP 수행시간 비교" << endl;
    Champion maxSL, maxDL;
    MEASURE("FindMaxHP_SL", maxSL = sl.FindMaxHP_SL());
    MEASURE("FindMaxHP_DL", maxDL = dl.FindMaxHP_DL());

    cout << "\nSL 최대 체력: " << maxSL.name << " (" << maxSL.hp << ")\n";
    cout << "DL 최대 체력: " << maxDL.name << " (" << maxDL.hp << ")\n";

    return 0;
}