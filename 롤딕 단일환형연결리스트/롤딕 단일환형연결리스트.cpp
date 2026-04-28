#include <iostream>
#include <fstream>   // 파일 입출력
#include <string>


using namespace std;

// ─────────────────────────────────────────
// 챔피언 데이터 구조체
// ─────────────────────────────────────────
struct legueoflegends
{
	string name;
	int hp;
	int mp;
	int speed;
	int range;
	string position;
};

// ─────────────────────────────────────────
// 원형 단일 연결 리스트 노드
// ─────────────────────────────────────────
struct Node
{
	legueoflegends data;
	Node* next;       // 다음 노드 포인터 (마지막 노드는 head를 가리킴)
};

Node* head = nullptr; // 리스트의 첫 번째 노드
int listcount = 0;    // 현재 노드 개수

// ─────────────────────────────────────────
// 챔피언 1명 출력
// ─────────────────────────────────────────
void printChamp(Node* node)
{
	cout << node->data.name << " "
		<< node->data.hp << " "
		<< node->data.mp << " "
		<< node->data.speed << " "
		<< node->data.range << " "
		<< node->data.position << endl;
}

// ─────────────────────────────────────────
// 배열 → 원형 단일 연결 리스트 변환
// ─────────────────────────────────────────
void Array2SLinkedList(legueoflegends LOL[], int n)
{
	head = nullptr;
	listcount = 0;

	for (int i = 0; i < n; ++i)
	{
		Node* newNode = new Node();
		newNode->data = LOL[i];
		newNode->next = nullptr;

		if (head == nullptr)
		{
			// 첫 노드: 자기 자신을 가리킴 (원형)
			head = newNode;
			head->next = head;
		}
		else
		{
			// 마지막 노드를 찾아서 새 노드를 뒤에 연결
			Node* last = head;
			while (last->next != head)
				last = last->next;

			last->next = newNode;
			newNode->next = head; // 원형 유지: 마지막 → head
		}
		listcount++;
	}
}

// ─────────────────────────────────────────
// 전체 출력 (원형이라 do-while 사용)
// ─────────────────────────────────────────
void PrintAll_SL()
{
	if (head == nullptr) { cout << "리스트가 비었음" << endl; return; }
	Node* cur = head;
	do
	{
		printChamp(cur);
		cur = cur->next;
	} while (cur != head); // head로 돌아오면 종료
}

// ─────────────────────────────────────────
// 이름으로 검색
// ─────────────────────────────────────────
void Search_SL(string champ)
{
	if (head == nullptr) { cout << "찾지 못함" << endl; return; }

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

	if (!found) cout << "찾지 못함" << endl;
}

// ─────────────────────────────────────────
// HP 내림차순 정렬 상태 유지하며 삽입
// ─────────────────────────────────────────
void Insert_SL(legueoflegends champ)
{
	Node* newNode = new Node();
	newNode->data = champ;
	newNode->next = nullptr;

	// 빈 리스트
	if (head == nullptr)
	{
		head = newNode;
		newNode->next = head;
		listcount++;
		return;
	}

	// 새 노드 HP가 head보다 크거나 같으면 → head 앞에 삽입
	if (champ.hp >= head->data.hp)
	{
		Node* last = head;
		while (last->next != head)
			last = last->next;

		newNode->next = head;
		head = newNode;
		last->next = head; // 원형 유지
		listcount++;
		return;
	}

	// 적절한 위치 탐색 후 삽입
	Node* cur = head;
	while (cur->next != head && cur->next->data.hp > champ.hp)
		cur = cur->next;

	newNode->next = cur->next;
	cur->next = newNode;
	listcount++;
}

// ─────────────────────────────────────────
// 이름으로 삭제 (같은 이름 전부 삭제)
// ─────────────────────────────────────────
void Delete_SL(string champ)
{
	if (head == nullptr) { cout << "그런챔피언은 없음" << endl; return; }

	bool found = false;

	// head가 삭제 대상인 경우 반복 처리
	while (head != nullptr && head->data.name == champ)
	{
		found = true;
		if (head->next == head) // 노드가 1개만 남은 경우
		{
			delete head;
			head = nullptr;
			listcount--;
			cout << "삭제" << endl;
			return;
		}
		// 마지막 노드 찾아서 head 교체
		Node* last = head;
		while (last->next != head) last = last->next;

		Node* toDelete = head;
		head = head->next;
		last->next = head; // 원형 유지
		delete toDelete;
		listcount--;
		cout << "삭제" << endl;
	}

	if (head == nullptr) return;

	// head 이후 노드 탐색하며 삭제
	Node* prev = head;
	Node* cur = head->next;
	while (cur != head)
	{
		if (cur->data.name == champ)
		{
			found = true;
			prev->next = cur->next; // 연결 끊기
			delete cur;
			cur = prev->next;
			listcount--;
			cout << "삭제" << endl;
		}
		else
		{
			prev = cur;
			cur = cur->next;
		}
	}

	if (!found) cout << "그런챔피언은 없스빈다." << endl;
}

// ─────────────────────────────────────────
// 최대 HP 챔피언 출력
// ─────────────────────────────────────────
void FindMaxHP_SL()
{
	if (head == nullptr) return;

	// 1단계: 최대 HP 값 탐색
	int maxhp = head->data.hp;
	Node* cur = head->next;
	while (cur != head)
	{
		if (cur->data.hp > maxhp) maxhp = cur->data.hp;
		cur = cur->next;
	}

	// 2단계: 최대 HP인 노드 출력
	cur = head;
	do
	{
		if (cur->data.hp == maxhp) printChamp(cur);
		cur = cur->next;
	} while (cur != head);
}

// ─────────────────────────────────────────
// 포지션으로 전체 삭제
// ─────────────────────────────────────────
void DeleteAll_SL(string pos)
{
	if (head == nullptr) { cout << "그런챔피언은 없음" << endl; return; }

	// head가 삭제 대상인 경우 반복 처리
	while (head != nullptr && head->data.position == pos)
	{
		if (head->next == head) // 노드 1개
		{
			delete head;
			head = nullptr;
			listcount--;
			cout << "삭제" << endl;
			return;
		}
		Node* last = head;
		while (last->next != head) last = last->next;

		Node* toDelete = head;
		head = head->next;
		last->next = head;
		delete toDelete;
		listcount--;
		cout << "삭제" << endl;
	}

	if (head == nullptr) return;

	// head 이후 노드 탐색하며 삭제
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
			cout << "삭제" << endl;
		}
		else
		{
			prev = cur;
			cur = cur->next;
		}
	}
}

// ─────────────────────────────────────────
// HP 내림차순 정렬 (선택 정렬 방식)
// ─────────────────────────────────────────
void SortByHp_SL()
{
	if (head == nullptr || head->next == head) return;

	// 원형 끊기 (일반 단일 연결 리스트로 변환)
	Node* last = head;
	while (last->next != head) last = last->next;
	last->next = nullptr;

	Node* sortedhead = nullptr;
	Node* sortedtail = nullptr;

	// 매 반복마다 남은 노드 중 최대 HP를 찾아 정렬 리스트에 추가
	while (head != nullptr)
	{
		Node* maxprev = nullptr;
		Node* maxNode = head; // ★ nullptr 아닌 head로 초기화해야 비교 가능
		Node* prev = nullptr;
		Node* cur = head;

		while (cur != nullptr)
		{
			if (cur->data.hp > maxNode->data.hp)
			{
				maxNode = cur;
				maxprev = prev;
			}
			prev = cur;
			cur = cur->next;
		}

		// 최대 노드를 원래 리스트에서 분리
		if (maxprev == nullptr)
			head = maxNode->next;
		else
			maxprev->next = maxNode->next;

		maxNode->next = nullptr;

		// 정렬 리스트 뒤에 붙이기
		if (sortedhead == nullptr)
			sortedhead = sortedtail = maxNode;
		else
		{
			sortedtail->next = maxNode;
			sortedtail = maxNode;
		}
	}

	// 원형 복원
	head = sortedhead;
	sortedtail->next = head;
}

// ─────────────────────────────────────────
// main
// ─────────────────────────────────────────
int main()
{
	legueoflegends LOL[172];
	int filecount = 0;

	// 파일 열기
	ifstream file("testdata.txt");
	if (!file)
	{
		cout << "파일 오류" << endl;
		return 1;
	}

	// 파일에서 직접 읽기 (공백/줄바꿈 기준으로 자동 파싱)
	while (file >> LOL[filecount].name
		>> LOL[filecount].hp
		>> LOL[filecount].mp
		>> LOL[filecount].speed
		>> LOL[filecount].range
		>> LOL[filecount].position)
	{
		filecount++;
	}

	file.close();

	// 배열 → 원형 연결 리스트 변환
	Array2SLinkedList(LOL, filecount);

	// 명령어 처리 루프
	while (1)
	{
		cout << "명령어를 입력하세요.";
		string command;
		cin >> command;
		cin.ignore(); // 입력 버퍼 개행 제거

		if (command == "PrintAll")
			PrintAll_SL();
		else if (command == "Search")
		{
			string champ; cin >> champ;
			Search_SL(champ);
		}
		else if (command == "Insert")
		{
			legueoflegends newChamp;
			cout << "이름"; cin >> newChamp.name;
			cout << "hp"; cin >> newChamp.hp;
			cout << "mp"; cin >> newChamp.mp;
			cout << "speed"; cin >> newChamp.speed;
			cout << "range"; cin >> newChamp.range;
			cout << "position"; cin >> newChamp.position;
			cin.ignore();
			Insert_SL(newChamp);
		}
		else if (command == "Delete")
		{
			string champ; cin >> champ; cin.ignore();
			Delete_SL(champ);
		}
		else if (command == "DeleteAll")
		{
			string pos; cin >> pos; cin.ignore();
			DeleteAll_SL(pos);
		}
		else if (command == "FindMaxHp")
			FindMaxHP_SL();
		else if (command == "SortByHp")
		{
			SortByHp_SL();
			cout << "정렬완료" << endl;
		}
	}
}