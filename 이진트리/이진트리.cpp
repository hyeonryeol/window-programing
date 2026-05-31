#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

using namespace std;
using namespace chrono;

// 챔피언 한 명의 정보를 담는 구조체
struct Champion {
    string position; // 포지션 (TOP, MID, BOT 등)
    string name;     // 챔피언 이름
    int hp;
    int attack;
    int defense;
};

// 단순 연결 리스트의 노드
// data: 챔피언 정보, next: 다음 노드를 가리키는 포인터
struct SNode {
    Champion data;
    SNode* next;
    SNode(Champion c) : data(c), next(nullptr) {}
};

// 이진 탐색 트리의 노드
// data: 챔피언 정보, left: 이름이 더 작은 노드, right: 이름이 더 큰 노드
struct BTNode {
    Champion data;
    BTNode* left;
    BTNode* right;
    BTNode(Champion c) : data(c), left(nullptr), right(nullptr) {}
};

// ─────────────────────────────────────────
// 단순 연결 리스트
// ─────────────────────────────────────────
class SinglyLinkedList {
public:
    SNode* head;
    int size;

    SinglyLinkedList() : head(nullptr), size(0) {}

    // 리스트 맨 뒤에 챔피언 추가
    // head가 없으면 첫 노드로, 있으면 끝까지 이동 후 연결
    void Insert(Champion c) {
        SNode* newNode = new SNode(c);
        if (!head) {
            head = newNode;
        }
        else {
            SNode* cur = head;
            while (cur->next) cur = cur->next; // 마지막 노드까지 이동
            cur->next = newNode;
        }
        size++;
    }

    // 병합 정렬의 합치기 단계
    // 두 정렬된 리스트 a, b를 이름 오름차순으로 하나로 합침 (재귀)
    SNode* merge_SL(SNode* a, SNode* b) {
        if (!a) return b;
        if (!b) return a;
        if (a->data.name <= b->data.name) {
            a->next = merge_SL(a->next, b); // a가 더 작으면 a를 앞에 두고 나머지를 재귀 합병
            return a;
        }
        else {
            b->next = merge_SL(a, b->next); // b가 더 작으면 b를 앞에 두고 나머지를 재귀 합병
            return b;
        }
    }

    // 병합 정렬의 분할 단계
    // slow/fast 포인터로 리스트 중간을 찾아 절반씩 나눈 뒤 각각 정렬하고 합침
    // slow는 한 칸씩, fast는 두 칸씩 이동 → fast가 끝에 닿으면 slow가 중간
    SNode* mergeSort_SL(SNode* node) {
        if (!node || !node->next) return node; // 노드가 0~1개면 이미 정렬됨
        SNode* slow = node;
        SNode* fast = node->next;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        SNode* mid = slow->next; // 중간 지점
        slow->next = nullptr;    // 리스트를 앞/뒤 두 개로 자름
        SNode* left = mergeSort_SL(node); // 앞쪽 절반 정렬
        SNode* right = mergeSort_SL(mid); // 뒤쪽 절반 정렬
        return merge_SL(left, right);     // 정렬된 두 리스트 합치기
    }

    // 병합 정렬로 이름 기준 오름차순 정렬 (O(n log n))
    void SortByName_SL() {
        head = mergeSort_SL(head);
    }

    // 소멸자: 모든 노드 메모리 해제
    ~SinglyLinkedList() {
        SNode* cur = head;
        while (cur) {
            SNode* next = cur->next;
            delete cur;
            cur = next;
        }
    }
};

// ─────────────────────────────────────────
// 이진 탐색 트리 (BST)
// 이름 기준으로 왼쪽 < 현재 < 오른쪽 순서 유지
// ─────────────────────────────────────────
class BinaryTree {
public:
    BTNode* root;

    BinaryTree() : root(nullptr) {}

    // 재귀적으로 올바른 위치를 찾아 노드 삽입
    // 이름이 현재 노드보다 작으면 왼쪽, 크면 오른쪽으로 내려감
    BTNode* insertNode(BTNode* node, Champion c) {
        if (!node) return new BTNode(c); // 빈 자리에 삽입
        if (c.name < node->data.name)
            node->left = insertNode(node->left, c);
        else if (c.name > node->data.name)
            node->right = insertNode(node->right, c);
        return node; // 이름이 같으면 중복이므로 삽입하지 않음
    }

    void Insert_BT(Champion c) {
        root = insertNode(root, c);
    }

    // 이름으로 노드 탐색 (재귀)
    // 이름이 같으면 반환, 작으면 왼쪽, 크면 오른쪽으로 내려감 (O(log n))
    BTNode* searchNode(BTNode* node, string name) {
        if (!node) return nullptr;           // 찾지 못한 경우
        if (name == node->data.name) return node;
        if (name < node->data.name) return searchNode(node->left, name);
        return searchNode(node->right, name);
    }

    // 이름으로 챔피언 탐색, 없으면 nullptr 반환
    Champion* SearchByName_BT(string name) {
        BTNode* result = searchNode(root, name);
        if (result) return &result->data;
        return nullptr;
    }

    // 서브트리에서 가장 작은 노드(가장 왼쪽) 반환
    // 삭제 시 오른쪽 서브트리의 최솟값을 찾는 데 사용
    BTNode* findMin(BTNode* node) {
        while (node->left) node = node->left;
        return node;
    }

    // 이름으로 노드 삭제 (재귀), 세 가지 경우 처리:
    //  1) 자식 없음: 그냥 삭제
    //  2) 자식 1개: 자식으로 대체
    //  3) 자식 2개: 오른쪽 서브트리의 최솟값으로 대체 후 그 노드 삭제
    BTNode* deleteNode(BTNode* node, string name) {
        if (!node) return nullptr;
        if (name < node->data.name) {
            node->left = deleteNode(node->left, name);
        }
        else if (name > node->data.name) {
            node->right = deleteNode(node->right, name);
        }
        else {
            // 삭제 대상 노드를 찾은 경우
            if (!node->left) {
                // 왼쪽 자식 없음: 오른쪽 자식으로 대체
                BTNode* temp = node->right;
                delete node;
                return temp;
            }
            else if (!node->right) {
                // 오른쪽 자식 없음: 왼쪽 자식으로 대체
                BTNode* temp = node->left;
                delete node;
                return temp;
            }
            else {
                // 자식이 둘 다 있음: 오른쪽 서브트리 최솟값으로 현재 노드 데이터 교체
                BTNode* minNode = findMin(node->right);
                node->data = minNode->data;
                // 교체에 사용한 최솟값 노드를 오른쪽 서브트리에서 삭제
                node->right = deleteNode(node->right, minNode->data.name);
            }
        }
        return node;
    }

    void Delete_BT(string name) {
        root = deleteNode(root, name);
    }

    // 중위 순회(Inorder): 왼쪽 → 현재 → 오른쪽 순으로 방문
    // BST에서 중위 순회하면 이름 오름차순으로 출력됨
    void inorder(BTNode* node) {
        if (!node) return;
        inorder(node->left);
        cout << node->data.position << " "
            << node->data.name << " "
            << node->data.hp << " "
            << node->data.attack << " "
            << node->data.defense << endl;
        inorder(node->right);
    }

    // 중위 순회로 전체 챔피언 출력 (이름 오름차순)
    void PrintAll_BT() {
        inorder(root);
    }

    // PrintAll_BT와 동일: BST 특성상 중위 순회 = 정렬된 순서 출력
    void SortByName_BT() {
        inorder(root);
    }

    // 후위 순회(Postorder)로 모든 노드 메모리 해제
    // 자식을 먼저 지우고 부모를 지워야 안전함
    void destroyTree(BTNode* node) {
        if (!node) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

    ~BinaryTree() {
        destroyTree(root);
    }
};

// 파일에서 챔피언 데이터를 읽어 연결 리스트와 BST 양쪽에 동시에 저장
void Loadfile(const string& filename, SinglyLinkedList& sl, BinaryTree& bt) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cout << "파일을 열 수 없음" << endl;
        return;
    }
    Champion c;
    while (fin >> c.position >> c.name >> c.hp >> c.attack >> c.defense) {
        sl.Insert(c);    // 연결 리스트 맨 뒤에 추가
        bt.Insert_BT(c); // BST에 이름 기준으로 삽입
    }
    fin.close();
    cout << "로드완료. 총 " << sl.size << "개 데이터" << endl;
}

// 수행시간 측정 매크로: 함수 실행 전후로 시간을 재서 microseconds 단위로 출력
#define MEASURE(label, func) {auto start = high_resolution_clock::now(); func; auto end = high_resolution_clock::now(); auto us = duration_cast<microseconds>(end-start).count(); cerr << label << " 수행시간: " << us << " us" << endl;}

int main() {
    SinglyLinkedList sl;
    BinaryTree bt;

    // test.txt에서 데이터 로드 → 연결 리스트 & BST에 저장
    Loadfile("test.txt", sl, bt);

    // 1. 연결 리스트 병합 정렬 수행시간 측정
    cout << "1. SortByName_SL 수행시간" << endl;
    MEASURE("SortByName_SL", sl.SortByName_SL());

    // 2. BST 이름 탐색 수행시간 측정
    cout << "2. SearchByName_BT 수행시간" << endl;
    MEASURE("SearchByName_BT", bt.SearchByName_BT("K4535675"));

    // 3. BST 삽입 수행시간 측정
    cout << "3. Insert_BT 수행시간" << endl;
    Champion newChamp = { "MID", "TestChamp", 5000, 3000, 2000 };
    MEASURE("Insert_BT", bt.Insert_BT(newChamp));

    // 4. BST 삭제 수행시간 측정
    cout << "4. Delete_BT 수행시간" << endl;
    MEASURE("Delete_BT", bt.Delete_BT("TestChamp"));

    // 5. BST 전체 출력 수행시간 측정
    // cout을 nul로 리다이렉트해서 실제 터미널 출력 시간을 제외하고 순수 순회 시간만 측정
    cout << "5. PrintAll_BT 수행시간" << endl;
    ofstream nullStream("nul");
    streambuf* originalBuf = cout.rdbuf(nullStream.rdbuf());
    MEASURE("PrintAll_BT", bt.PrintAll_BT());
    cout.rdbuf(originalBuf); // cout 복원

    // 6. BST 정렬 출력 수행시간 측정 (중위 순회 = 자동 정렬)
    cout << "6. SortByName_BT 수행시간" << endl;
    cout.rdbuf(nullStream.rdbuf());
    MEASURE("SortByName_BT", bt.SortByName_BT());
    cout.rdbuf(originalBuf); // cout 복원

    return 0;
}