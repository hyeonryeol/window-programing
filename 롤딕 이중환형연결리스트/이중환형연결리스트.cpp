#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// ═══════════════════════════════════════════════════════════════
// 📌 이중 환형(원형) 연결리스트란?
//
//  ┌────────────────────────────────────────────────┐
//  │  [head] ↔ [A] ↔ [B] ↔ [C] ↔ (다시 head로)   │
//  └────────────────────────────────────────────────┘
//
//  - 이중(Doubly)   : 각 노드가 앞(prev)·뒤(next) 양방향 포인터를 가짐
//  - 환형(Circular) : 마지막 노드의 next → head,
//                     head의 prev → 마지막 노드  (끊어지지 않고 원형)
//
//  덕분에 head->prev 만으로 마지막 노드를 O(1)에 찾을 수 있음
// ═══════════════════════════════════════════════════════════════

// ───────────────────────────────────────
// 챔피언 데이터 구조체
// ───────────────────────────────────────
struct legueoflegends
{
    string name;
    int hp, mp, speed, range;
    string position;
};

// ───────────────────────────────────────
// 노드 구조체
//
//  prev ←─[data]─→ next
//  └── 이전 노드        └── 다음 노드
//
// ───────────────────────────────────────
struct Node
{
    legueoflegends data;
    Node* prev;   // 이전 노드를 가리킴 (단일 연결리스트에는 없던 포인터)
    Node* next;   // 다음 노드를 가리킴
};

Node* head = nullptr; // 리스트의 시작점. 비어있으면 nullptr
int listcount = 0;    // 현재 노드 개수 (삭제/삽입 시 갱신)

// ───────────────────────────────────────
// 챔피언 1명 출력
// ───────────────────────────────────────
void printChamp(Node* node)
{
    cout << node->data.name << " "
        << node->data.hp << " "
        << node->data.mp << " "
        << node->data.speed << " "
        << node->data.range << " "
        << node->data.position << endl;
}

// ═══════════════════════════════════════════════════════════════
// Array2DLinkedList
//  배열 → 이중 환형 연결리스트 변환
//
//  흐름 예시 (A, B, C 순서로 삽입할 때):
//
//  [1] A 삽입 → head = A,  A.next = A,  A.prev = A
//       (자기 자신이 원형)
//
//  [2] B 삽입
//      last = head->prev = A
//      A.next = B,  B.prev = A
//      B.next = head(A),  A.prev = B
//      → A ↔ B ↔ (back to A)
//
//  [3] C 삽입
//      last = head->prev = B
//      B.next = C,  C.prev = B
//      C.next = head(A),  A.prev = C
//      → A ↔ B ↔ C ↔ (back to A)
// ═══════════════════════════════════════════════════════════════
void Array2DLinkedList(legueoflegends LOL[], int n)
{
    head = nullptr;
    listcount = 0;

    for (int i = 0; i < n; ++i)
    {
        // 새 노드 생성 및 데이터 설정
        Node* newNode = new Node();
        newNode->data = LOL[i];
        newNode->prev = nullptr;
        newNode->next = nullptr;

        if (head == nullptr)
        {
            // ── 첫 번째 노드: 자기 자신을 향하도록 설정 (원형의 시작) ──
            head = newNode;
            head->next = head;  // next → 자기 자신
            head->prev = head;  // prev → 자기 자신
        }
        else
        {
            // ── 두 번째 이후 노드: 리스트 맨 뒤에 추가 ──
            //
            //  이중 연결리스트의 장점: head->prev = 마지막 노드
            //  (단일이었으면 while 루프로 끝까지 순회해야 함)
            //
            Node* last = head->prev;  // 현재 마지막 노드

            last->next = newNode;  // (기존 마지막).next → 새 노드
            newNode->prev = last;     // 새 노드.prev       → 기존 마지막
            newNode->next = head;     // 새 노드.next       → head (원형 유지)
            head->prev = newNode;  // head.prev          → 새 노드 (원형 유지)
        }
        listcount++;
    }
}

// ═══════════════════════════════════════════════════════════════
// PrintAll_DL : 전체 출력
//
//  do-while을 쓰는 이유:
//    - 원형 리스트는 시작(head)과 끝 조건이 같아서
//      일반 while(cur != head)로 시작하면 첫 노드부터 출력 못 함
//    - do { 출력 } while(cur != head) 로 최소 1번 실행 후 비교
// ═══════════════════════════════════════════════════════════════
void PrintAll_DL()
{
    if (head == nullptr) { cout << "리스트가 비었음" << endl; return; }

    Node* cur = head;
    do {
        printChamp(cur);
        cur = cur->next;
    } while (cur != head); // head로 돌아오면 한 바퀴 완료
}

// ═══════════════════════════════════════════════════════════════
// Search_DL : 이름으로 검색
// ═══════════════════════════════════════════════════════════════
void Search_DL(string champ)
{
    if (head == nullptr) { cout << "찾지 못함" << endl; return; }

    bool found = false;
    Node* cur = head;
    do {
        if (cur->data.name == champ)
        {
            printChamp(cur);
            found = true;   // 찾아도 계속 순회 (동명이인 가능성)
        }
        cur = cur->next;
    } while (cur != head);

    if (!found) cout << "찾지 못함" << endl;
}

// ═══════════════════════════════════════════════════════════════
// unlinkNode : 노드를 리스트에서 분리 (삭제용 헬퍼)
//
//  분리 전:  prev ↔ [node] ↔ next
//  분리 후:  prev ↔ next       (node는 리스트에서 제거됨)
//
//  이중 연결리스트라서 prev 포인터로 앞 노드를 바로 알 수 있음
//  (단일이었으면 앞 노드를 따로 추적해야 함)
// ═══════════════════════════════════════════════════════════════
void unlinkNode(Node* node)
{
    node->prev->next = node->next;  // 앞 노드의 next → node 다음 노드
    node->next->prev = node->prev;  // 뒤 노드의 prev → node 이전 노드

    // 삭제 노드가 head였다면 head를 다음 노드로 갱신
    if (head == node)
        head = (node->next == node) ? nullptr : node->next;
    //       ↑ 노드가 1개(자기 자신만 가리킴)이면 리스트가 빔

    node->next = node->prev = nullptr; // 분리된 노드 포인터 정리
    listcount--;
}

// ═══════════════════════════════════════════════════════════════
// insertAfter : target 노드 바로 뒤에 newNode 삽입 (삽입용 헬퍼)
//
//  삽입 전:  target ↔ target.next
//  삽입 후:  target ↔ newNode ↔ (기존 target.next)
// ═══════════════════════════════════════════════════════════════
void insertAfter(Node* target, Node* newNode)
{
    newNode->next = target->next;  // 새 노드 next → 기존 다음 노드
    newNode->prev = target;        // 새 노드 prev → target
    target->next->prev = newNode;       // 기존 다음 노드 prev → 새 노드
    target->next = newNode;       // target next → 새 노드
}

// ═══════════════════════════════════════════════════════════════
// Insert_DL : HP 내림차순 순서를 유지하며 삽입
//
//  예: [700 → 500 → 300] 에 HP=600 삽입
//      → [700 → 600 → 500 → 300]
//
//  경우 1: 리스트가 비어있음 → 단독 노드 생성
//  경우 2: 새 HP >= head HP → head 앞(맨 앞)에 삽입, head 갱신
//  경우 3: 중간 또는 끝    → 적절한 위치 찾아서 삽입
// ═══════════════════════════════════════════════════════════════
void Insert_DL(legueoflegends champ)
{
    Node* newNode = new Node();
    newNode->data = champ;
    newNode->prev = newNode->next = nullptr;

    // ── 경우 1: 빈 리스트 ──
    if (head == nullptr)
    {
        head = newNode;
        head->next = head->prev = head;
        listcount++;
        return;
    }

    // ── 경우 2: 새 노드가 head보다 HP가 크거나 같음 → 맨 앞 삽입 ──
    if (champ.hp >= head->data.hp)
    {
        Node* last = head->prev;  // 마지막 노드 (원형 유지를 위해 필요)

        newNode->next = head;      // 새 노드 next → 기존 head
        newNode->prev = last;      // 새 노드 prev → 기존 마지막
        last->next = newNode;   // 마지막 노드 next → 새 노드
        head->prev = newNode;   // 기존 head prev → 새 노드
        head = newNode;   // head를 새 노드로 갱신
        listcount++;
        return;
    }

    // ── 경우 3: 중간/끝 위치 탐색 ──
    // cur->next의 HP가 새 노드 HP보다 크면 계속 이동
    // 멈춘 위치의 바로 뒤에 삽입
    Node* cur = head;
    while (cur->next != head && cur->next->data.hp > champ.hp)
        cur = cur->next;

    insertAfter(cur, newNode); // cur 뒤에 삽입
    listcount++;
}

// ═══════════════════════════════════════════════════════════════
// Delete_DL : 이름이 같은 노드 전부 삭제
//
//  포인트: 삭제 전에 next를 미리 저장해두어야
//          cur을 delete한 뒤에도 다음 노드로 이동 가능
// ═══════════════════════════════════════════════════════════════
void Delete_DL(string champ)
{
    if (head == nullptr) { cout << "그런챔피언은 없음" << endl; return; }

    bool found = false;
    Node* cur = head;

    // listcount만큼만 순회 (무한루프 방지)
    int count = listcount;
    for (int i = 0; i < count; i++)
    {
        Node* next = cur->next;  // ★ 삭제 전에 미리 다음 노드 저장

        if (cur->data.name == champ)
        {
            found = true;
            unlinkNode(cur); // 리스트에서 분리
            delete cur;      // 메모리 해제
            cout << "삭제" << endl;
        }

        if (head == nullptr) break; // 리스트가 완전히 비면 종료
        cur = next;                 // 미리 저장해 둔 next로 이동
    }

    if (!found) cout << "그런챔피언은 없음" << endl;
}

// ═══════════════════════════════════════════════════════════════
// DeleteAll_DL : 포지션이 같은 노드 전부 삭제
//  (Delete_DL과 동일한 구조, 비교 기준만 position으로 변경)
// ═══════════════════════════════════════════════════════════════
void DeleteAll_DL(string pos)
{
    if (head == nullptr) { cout << "그런챔피언은 없음" << endl; return; }

    bool found = false;
    Node* cur = head;
    int count = listcount;

    for (int i = 0; i < count; i++)
    {
        Node* next = cur->next; // 삭제 전에 미리 다음 노드 저장

        if (cur->data.position == pos)
        {
            found = true;
            unlinkNode(cur);
            delete cur;
            cout << "삭제" << endl;
        }

        if (head == nullptr) break;
        cur = next;
    }

    if (!found) cout << "그런챔피언은 없음" << endl;
}

// ═══════════════════════════════════════════════════════════════
// FindMaxHp_DL : 최대 HP 챔피언 출력
//
//  2단계:
//   1단계) 전체 순회해서 최대 HP 값을 구함
//   2단계) 다시 순회해서 최대 HP인 노드를 전부 출력
// ═══════════════════════════════════════════════════════════════
void FindMaxHp_DL()
{
    if (head == nullptr) return;

    // 1단계: 최대 HP 탐색
    int maxhp = head->data.hp;
    Node* cur = head->next;
    while (cur != head)
    {
        if (cur->data.hp > maxhp) maxhp = cur->data.hp;
        cur = cur->next;
    }

    // 2단계: 최대 HP 노드 출력
    cur = head;
    do {
        if (cur->data.hp == maxhp) printChamp(cur);
        cur = cur->next;
    } while (cur != head);
}

// ═══════════════════════════════════════════════════════════════
// SortByHp_DL : HP 내림차순 정렬 (선택 정렬, 노드 자체 이동)
//
//  ★ 핵심 조건: 값(data)을 교환하면 안 됨 → 노드를 직접 이동
//
//  선택 정렬 흐름:
//   라운드 0: 전체에서 최대 HP 노드를 찾아 맨 앞(head)으로 이동
//   라운드 1: 나머지에서 최대 HP 노드를 찾아 두 번째 자리로 이동
//   라운드 2: ...반복...
//
//  노드 이동 방법 (maxNode를 unsortedStart 앞으로):
//
//   이동 전:  ... ↔ [maxNode] ↔ ...  ←← 원래 위치에서 분리
//   이동 후:  ... ↔ [maxNode] ↔ [unsortedStart] ↔ ...
//
// ═══════════════════════════════════════════════════════════════
void SortByHp_DL()
{
    if (head == nullptr || head->next == head) return; // 노드 0개 또는 1개면 정렬 불필요

    int n = listcount;

    for (int i = 0; i < n - 1; i++)
    {
        // unsortedStart: i번째 자리 (이 라운드에서 채워야 할 위치)
        Node* unsortedStart = head;
        for (int j = 0; j < i; j++)
            unsortedStart = unsortedStart->next; // i번 이동하면 미정렬 구간 시작

        // ── 미정렬 구간(unsortedStart ~ 끝)에서 최대 HP 노드 탐색 ──
        Node* maxNode = unsortedStart;
        Node* cur = unsortedStart->next;

        // 원형이므로 head까지 오면 한 바퀴 완료
        while (cur != head)
        {
            if (cur->data.hp > maxNode->data.hp)
                maxNode = cur;
            cur = cur->next;
        }

        // 이미 제 위치면 이동 불필요
        if (maxNode == unsortedStart) continue;

        // ── 1단계: maxNode를 현재 위치에서 분리 ──
        //
        //   분리 전:  [maxNode.prev] ↔ [maxNode] ↔ [maxNode.next]
        //   분리 후:  [maxNode.prev] ↔ [maxNode.next]
        //
        maxNode->prev->next = maxNode->next;
        maxNode->next->prev = maxNode->prev;

        // ── 2단계: maxNode를 unsortedStart 바로 앞에 삽입 ──
        //
        //   삽입 전:  [before] ↔ [unsortedStart]
        //   삽입 후:  [before] ↔ [maxNode] ↔ [unsortedStart]
        //
        Node* before = unsortedStart->prev; // unsortedStart 앞 노드
        before->next = maxNode;
        maxNode->prev = before;
        maxNode->next = unsortedStart;
        unsortedStart->prev = maxNode;

        // ── 3단계: unsortedStart가 head였다면 head를 maxNode로 갱신 ──
        if (unsortedStart == head)
            head = maxNode;
    }
}

// ═══════════════════════════════════════════════════════════════
// main
// ═══════════════════════════════════════════════════════════════
int main()
{
    legueoflegends LOL[172];
    int filecount = 0;

    // 파일 읽기
    ifstream file("testdata.txt");
    if (!file) { cout << "파일 오류" << endl; return 1; }

    while (file >> LOL[filecount].name >> LOL[filecount].hp
        >> LOL[filecount].mp >> LOL[filecount].speed
        >> LOL[filecount].range >> LOL[filecount].position)
        filecount++;

    file.close();

    // 배열 → 이중 환형 연결리스트 변환
    Array2DLinkedList(LOL, filecount);

    // 명령어 처리 루프
    while (1)
    {
        cout << "명령어를 입력하세요: ";
        string command;
        cin >> command;
        cin.ignore();

        if (command == "PrintAll")  PrintAll_DL();
        else if (command == "Search") { string c; cin >> c; Search_DL(c); }
        else if (command == "Insert")
        {
            legueoflegends champ;
            cout << "이름 hp mp speed range position: ";
            cin >> champ.name >> champ.hp >> champ.mp >> champ.speed >> champ.range >> champ.position;
            cin.ignore();
            Insert_DL(champ);
        }
        else if (command == "Delete") { string c; cin >> c; cin.ignore(); Delete_DL(c); }
        else if (command == "DeleteAll") { string p; cin >> p; cin.ignore(); DeleteAll_DL(p); }
        else if (command == "FindMaxHp")   FindMaxHp_DL();
        else if (command == "SortByHp") { SortByHp_DL(); cout << "정렬완료" << endl; }
    }
}