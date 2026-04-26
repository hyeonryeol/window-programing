#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// ===================== 구조체 정의 =====================
struct legueoflegends
{
    string name;
    int hp;
    int mp;
    int speed;
    int range;
    string position;
};

// 단일 환형 연결리스트 노드
struct Node
{
    legueoflegends data;
    Node* next;
};

// ===================== 전역변수 =====================
Node* head = nullptr;  // 리스트의 시작 노드
int listcount = 0;

// ===================== 출력 헬퍼 =====================
void printChamp(Node* node)
{
    cout << node->data.name << " "
        << node->data.hp << " "
        << node->data.mp << " "
        << node->data.speed << " "
        << node->data.range << " "
        << node->data.position << endl;
}


// 배열에 저장된 데이터를 단일 환형 연결리스트로 변환
void Array2SLinkedList(legueoflegends LOL[], int n)
{
    head = nullptr;
    listcount = 0;

    for (int i = 0; i < n; i++)
    {
        Node* newNode = new Node();
        newNode->data = LOL[i];
        newNode->next = nullptr;

        if (head == nullptr)
        {
            head = newNode;
            head->next = head;  // 자기 자신을 가리킴 (환형)
        }
        else
        {
            // 마지막 노드 찾기
            Node* last = head;
            while (last->next != head)
                last = last->next;

            last->next = newNode;
            newNode->next = head;  // 마지막 노드 -> head (환형 유지)
        }
        listcount++;
    }
}

// ===================== PrintAll_SL =====================
void PrintAll_SL()
{
    if (head == nullptr) { cout << "리스트가 비어있습니다." << endl; return; }

    Node* cur = head;
    do
    {
        printChamp(cur);
        cur = cur->next;
    } while (cur != head);  // head로 돌아오면 종료
}

// ===================== Search_SL =====================
void Search_SL(string champ)
{
    if (head == nullptr) { cout << "찾지 못했습니다." << endl; return; }

    bool found = false;
    Node* cur = head;
    do
    {
        if (cur->data.name == champ)
        {
            printChamp(cur);
            found = true;
        }
        cur = cur->next;
    } while (cur != head);

    if (!found) cout << "찾지 못했습니다." << endl;
}

// ===================== Insert_SL =====================
// HP 내림차순 정렬 순서를 유지하면서 삽입
void Insert_SL(legueoflegends champ)
{
    Node* newNode = new Node();
    newNode->data = champ;
    newNode->next = nullptr;

    // 리스트가 비어있는 경우
    if (head == nullptr)
    {
        head = newNode;
        newNode->next = head;
        listcount++;
        return;
    }

    // 새 노드가 head보다 hp가 크거나 같으면 → 맨 앞(새 head)에 삽입
    if (champ.hp >= head->data.hp)
    {
        Node* last = head;
        while (last->next != head)
            last = last->next;

        newNode->next = head;
        head = newNode;
        last->next = head;  // 마지막 노드 → 새 head
        listcount++;
        return;
    }

    // 삽입 위치 탐색 (HP 내림차순 기준)
    Node* cur = head;
    while (cur->next != head && cur->next->data.hp > champ.hp)
        cur = cur->next;

    newNode->next = cur->next;
    cur->next = newNode;
    listcount++;
}

// ===================== Delete_SL =====================
void Delete_SL(string champ)
{
    if (head == nullptr) { cout << "그런 챔피언은 없습니다." << endl; return; }

    bool found = false;

    // head 노드가 삭제 대상인 경우 반복 처리
    while (head != nullptr && head->data.name == champ)
    {
        found = true;
        if (head->next == head)  // 노드가 1개인 경우
        {
            delete head;
            head = nullptr;
            listcount--;
            cout << "삭제되었습니다." << endl;
            return;
        }
        Node* last = head;
        while (last->next != head) last = last->next;

        Node* toDelete = head;
        head = head->next;
        last->next = head;
        delete toDelete;
        listcount--;
        cout << "삭제되었습니다." << endl;
    }

    if (head == nullptr) return;

    // 나머지 노드 탐색
    Node* prev = head;
    Node* cur = head->next;
    while (cur != head)
    {
        if (cur->data.name == champ)
        {
            found = true;
            prev->next = cur->next;
            delete cur;
            cur = prev->next;
            listcount--;
            cout << "삭제되었습니다." << endl;
        }
        else
        {
            prev = cur;
            cur = cur->next;
        }
    }

    if (!found) cout << "그런 챔피언은 없습니다." << endl;
}

// ===================== DeleteAll_SL =====================
void DeleteAll_SL(string pos)
{
    if (head == nullptr) return;

    // head 노드가 삭제 대상인 경우 반복 처리
    while (head != nullptr && head->data.position == pos)
    {
        if (head->next == head)
        {
            delete head;
            head = nullptr;
            listcount--;
            cout << "삭제되었습니다." << endl;
            return;
        }
        Node* last = head;
        while (last->next != head) last = last->next;

        Node* toDelete = head;
        head = head->next;
        last->next = head;
        delete toDelete;
        listcount--;
        cout << "삭제되었습니다." << endl;
    }

    if (head == nullptr) return;

    Node* prev = head;
    Node* cur = head->next;
    while (cur != head)
    {
        if (cur->data.position == pos)
        {
            prev->next = cur->next;
            delete cur;
            cur = prev->next;
            listcount--;
            cout << "삭제되었습니다." << endl;
        }
        else
        {
            prev = cur;
            cur = cur->next;
        }
    }
}

// ===================== FindMaxHp_SL =====================
void FindMaxHp_SL()
{
    if (head == nullptr) return;

    // 최대 hp 탐색
    int maxhp = head->data.hp;
    Node* cur = head->next;
    while (cur != head)
    {
        if (cur->data.hp > maxhp) maxhp = cur->data.hp;
        cur = cur->next;
    }

    // 최대 hp 챔피언 출력
    cur = head;
    do
    {
        if (cur->data.hp == maxhp) printChamp(cur);
        cur = cur->next;
    } while (cur != head);
}

// ===================== SortByHp_SL =====================
// 노드 자체를 이동시켜 HP 내림차순 정렬 (선택 정렬)
void SortByHp_SL()
{
    if (head == nullptr || head->next == head) return;

    // 환형 → 일반 연결리스트로 임시 변환 (마지막 next를 nullptr)
    Node* last = head;
    while (last->next != head) last = last->next;
    last->next = nullptr;

    Node* sortedHead = nullptr;
    Node* sortedTail = nullptr;

    // 선택 정렬: 매번 남은 노드 중 최대 hp 노드를 찾아 정렬된 리스트에 추가
    while (head != nullptr)
    {
        Node* maxPrev = nullptr;
        Node* maxNode = head;
        Node* prev = nullptr;
        Node* cur = head;

        while (cur != nullptr)
        {
            if (cur->data.hp > maxNode->data.hp)
            {
                maxNode = cur;
                maxPrev = prev;
            }
            prev = cur;
            cur = cur->next;
        }

        // 최대 노드 분리
        if (maxPrev == nullptr)
            head = maxNode->next;
        else
            maxPrev->next = maxNode->next;

        maxNode->next = nullptr;

        // 정렬된 리스트에 추가
        if (sortedHead == nullptr)
        {
            sortedHead = sortedTail = maxNode;
        }
        else
        {
            sortedTail->next = maxNode;
            sortedTail = maxNode;
        }
    }

    // 다시 환형으로 복원
    head = sortedHead;
    sortedTail->next = head;
}

// ===================== main =====================
int main()
{
    // --- 파일 읽기 (기존 배열 방식) ---
    legueoflegends LOL[172];
    int fileCount = 0;

    ifstream file("testdata.txt");
    if (!file)
    {
        cout << "파일오류" << endl;
        return 1;
    }

    string line;
    while (getline(file, line))
    {
        if (line.empty()) continue;
        string words[100];
        int wordCount = 0;
        string cur = "";
        for (int i = 0; i <= (int)line.size(); ++i)
        {
            if (i == (int)line.size() || line[i] == ' ' || line[i] == '\t')
            {
                if (!cur.empty()) { words[wordCount++] = cur; cur = ""; }
            }
            else cur += line[i];
        }

        LOL[fileCount].name = "";
        int i = 0;
        while (i < wordCount && !isdigit(words[i][0]))
        {
            if (i > 0) LOL[fileCount].name += " ";
            LOL[fileCount].name += words[i];
            i++;
        }
        LOL[fileCount].hp = stoi(words[i++]);
        LOL[fileCount].mp = stoi(words[i++]);
        LOL[fileCount].speed = stoi(words[i++]);
        LOL[fileCount].range = stoi(words[i++]);
        LOL[fileCount].position = words[i];
        fileCount++;
    }
    file.close();

    // --- 배열 → 단일 환형 연결리스트 변환 ---
    Array2SLinkedList(LOL, fileCount);

    // --- 메인 루프 ---
    while (1)
    {
        cout << "\n=== LOL Dictionary (Circular Linked List) ===" << endl;
        cout << "Search       : 챔피언 이름으로 검색" << endl;
        cout << "Insert       : 새 챔피언 삽입 (HP 정렬 유지)" << endl;
        cout << "Delete       : 챔피언 삭제" << endl;
        cout << "DeleteAll    : 포지션의 모든 챔피언 삭제" << endl;
        cout << "PrintAll     : 전체 출력" << endl;
        cout << "FindMaxHp    : 최대 HP 챔피언 출력" << endl;
        cout << "SortByHp     : HP 내림차순 정렬 (노드 이동)" << endl;
        cout << "명령어를 입력하세요: ";

        string command;
        cin >> command;
        cin.ignore();

        if (command == "PrintAll")
        {
            PrintAll_SL();
        }
        else if (command == "Search")
        {
            string champ;
            getline(cin, champ);
            Search_SL(champ);
        }
        else if (command == "Insert")
        {
            legueoflegends newChamp;
            cout << "이름: ";    getline(cin, newChamp.name);
            cout << "hp: ";     cin >> newChamp.hp;
            cout << "mp: ";     cin >> newChamp.mp;
            cout << "speed: ";  cin >> newChamp.speed;
            cout << "range: ";  cin >> newChamp.range;
            cout << "position: "; cin >> newChamp.position;
            cin.ignore();
            Insert_SL(newChamp);
        }
        else if (command == "Delete")
        {
            string champ;
            getline(cin, champ);
            Delete_SL(champ);
        }
        else if (command == "DeleteAll")
        {
            string pos;
            cin >> pos;
            cin.ignore();
            DeleteAll_SL(pos);
        }
        else if (command == "FindMaxHp")
        {
            FindMaxHp_SL();
        }
        else if (command == "SortByHp")
        {
            SortByHp_SL();
            cout << "정렬 완료." << endl;
        }
    }
}