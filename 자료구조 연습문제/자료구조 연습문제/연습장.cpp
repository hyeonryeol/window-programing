#include <iostream>
#include <string>
#include <fstream>

using namespace std;

struct legueoflegends
{
	string name;
	int hp, mp, speed, range;
	string position;
};

struct Node
{
	legueoflegends data;
	Node* prev;
	Node* next;

};

Node* head = nullptr;
int listcount = 0;

void printchamp(Node* node)
{
	cout << node->data.name << " "
		<< node->data.hp << " "
		<< node->data.mp << " "
		<< node->data.speed << " "
		<< node->data.range << " "
		<< node->data.position << endl;
}

void Array2DLinkedList(legueoflegends LOL[], int n)
{
	head = nullptr;
	listcount = 0;

	for (int i = 0; i < n; ++i)
	{
		Node* newNode = new Node();
		newNode->data = LOL[i];
		newNode->prev = nullptr;
		newNode->next = nullptr;

		if (head == nullptr)
		{
			head = newNode;
			head->next = head;
			head->prev = head;
		}
		else
		{
			Node* last = head->prev;

			last->next = newNode;
			newNode->prev = last;
			newNode->next = head;
			head->prev = newNode;
		}
		listcount++;
	}
}


void PrintAll_DL()
{
	if (head == nullptr) { cout << "리스트가 비었음" << endl; return; }

	Node* cur = head;
	do {
		printchamp(cur);
		cur = cur->next;

	} while (cur != head);
}


void Search_DL(string champ)
{
	if (head == nullptr) { cout << "찾지 못함" << endl; return; }
	bool found = false;
	Node* cur = head;
	do {
		if (cur->data.name == champ)
		{
			printchamp(cur);
			found = true;
		}
		cur = cur->next;
	} while (cur != head);
	if (!found) cout << "찾지 못함" << endl;
}

void unlinkedNode(Node* node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;

	if (head == node)
		head = (node->next == node) ? nullptr : node->next;

	node->next = node->prev = nullptr;
	listcount--;
}
void insertafter(Node* target, Node* newNode)
{
	newNode->next = target->next;
	newNode->prev = target;
	target->next->prev = newNode;
	target->next = newNode;
}
void Insert_DL(legueoflegends champ)
{
	Node* newNode = new Node();
	newNode->data = champ;
	newNode->prev = newNode->next = nullptr;

	if (head == nullptr)
	{
		head = newNode;
		head->next = head->prev = head;
		listcount++;
		return;
	}
	if (champ.hp > head->data.hp)
	{
		Node* last = head->prev;

		newNode->next = head;
		newNode->prev = last;
		last->next = newNode;
		head->prev = newNode;
		head = newNode;
		listcount++;
		return;
	}

	Node* cur = head;
	while (cur->next != head && cur->next->data.hp > champ.hp)
		cur = cur->next;

	insertafter(cur, newNode);
	listcount++;
}
void Delete_DL(string champ)
{
	if (head == nullptr) { cout << "찾지 못함" << endl; return; }
	bool found = false;
	Node* cur = head;
	int count = listcount;
	for (int i = 0; i < count; i++)
	{
		Node* next = cur->next;

		if (cur->data.name == champ)
		{
			found = true;
			unlinkedNode(cur);
			delete cur;
			cout << "삭제" << endl;
		}
		if (head == nullptr) break;
		cur = next;
	}
	if (!found) cout << "찾지 못함" << endl;
}
void DeleteAll_DL(string pos)
{
	if (head == nullptr) { cout << "찾지 못함" << endl; return; }
	bool found = false;
	Node* cur = head;
	int count = listcount;
	for (int i = 0; i < count; i++)
	{
		Node* next = cur->next;

		if (cur->data.position == pos)
		{
			found = true;
			unlinkedNode(cur);
			delete cur;
			cout << "삭제" << endl;
		}
		if (head == nullptr) break;
		cur = next;
	}
	if (!found) cout << "찾지 못함" << endl;
}
void FindMaxHp_DL()
{
	if (head == nullptr) return;
	int maxhp = head->data.hp;
	Node* cur = head->next;
	while (cur != head)
	{
		if (cur->data.hp > maxhp) maxhp = cur->data.hp;
		cur = cur->next;
	}

	cur = head;
	do {
		if (cur->data.hp == maxhp) printchamp(cur);
		cur = cur->next;
	} while (cur != head);
}
void SortByHp_DL()
{
	if (head == nullptr || head->next == head) return;

	int n = listcount;
	for (int i = 0; i < n; ++i)
	{
		Node* unsortedstart = head;
		for (int j = 0; j < i; j++)
			unsortedstart = unsortedstart->next;

		Node* maxNode = unsortedstart;
		Node* cur = unsortedstart->next;

		while (cur != head)
		{
			if (cur->data.hp > maxNode->data.hp)
				maxNode = cur;
			cur = cur->next;
		}

		if (maxNode == unsortedstart) continue;
		maxNode->prev->next = maxNode->next;
		maxNode->next->prev = maxNode->prev;

		Node* before = unsortedstart->prev;
		before->next = maxNode;
		maxNode->prev = before;
		maxNode->next = unsortedstart;
		unsortedstart->prev = maxNode;
		
		if (unsortedstart == head)
			head = maxNode;
	}
}
int main()
{
	legueoflegends LOL[172];
	int filecount = 0;

	ifstream file("testdata.txt");
	if (!file)
	{
		cout << "파일 오류" << endl;
		return 1;
	}
	while (file >> LOL[filecount].name >> LOL[filecount].hp
		>> LOL[filecount].mp
		>> LOL[filecount].speed
		>> LOL[filecount].range
		>> LOL[filecount].position)
	{
		filecount++;
	}
	file.close();

	Array2DLinkedList(LOL, filecount);

	while (1)
	{
		cout << "명령어를 입력하세요.";
		string command;
		cin >> command;
		cin.ignore();

		if (command == "PrintAll")
		{
			PrintAll_DL();
		}
		else if (command == "Search")
		{
			string champ;
			cin >> champ;
			cin.ignore();
			Search_DL(champ);
		}
		else if (command == "Insert")
		{
			legueoflegends c;
			cout << "이름을 입력하세요.";
			cin >> c.name;
			cout << "hp 를 입력하세요.";
			cin >> c.hp;
			cout << "mp를 입력하세요.";
			cin >> c.mp;
			cout << "speed를 입력하세요.";
			cin >> c.speed;
			cout << "range를 입력하세요.";
			cin >> c.range;
			cout << "positon를 입력하세요.";
			cin >> c.position;
			cin.ignore();
			Insert_DL(c);
		}
		else if (command == "Delete")
		{
			string champ;
			cin >> champ;
			cin.ignore();
			Delete_DL(champ);
		}
		else if (command == "DeleteAll")
		{
			string pos;
			cin >> pos;
			cin.ignore();
			DeleteAll_DL(pos);
		}
		else if (command == "FindMaxHp")
		{
			FindMaxHp_DL();
		}
		else if(command == "SortByHp")
		{
			SortByHp_DL();
			cout << "정렬되었습니다." << endl;
		}
		else
		{
			cout << "명령어를 다시 입력하세요." << endl;
		}
	}
}
