#include <iostream>
#include <fstream>
#include <string>

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
	Node* cur = head;
	bool found = false;
	do {
		if (cur->data.name == champ)
		{
			found = true;
			printchamp(cur);
		}
		cur = cur->next;
	} while (cur != head);
	if (!found) { cout << "찾지 못함" << endl; return; }
}
void unlinknode(Node* node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;

	if (head == node)
		head = (node->next == node) ? nullptr : node->next;
	node->next = node->prev = nullptr;
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
	newNode->prev = nullptr;
	newNode->next = nullptr;
	if (head == nullptr)
	{
		head = newNode;
		head->next = head;
		head->prev = head;
		listcount++;
		return;
	}
	if (head->data.hp > champ.hp)
	{
		Node* last = head->prev;

		last->next = newNode;
		newNode->prev = last;
		newNode->next = head;
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
	Node* cur = head;
	bool found = false;
	do {
		if (cur->data.name == champ)
		{
			found = true;
			Node* next = cur->next;
			unlinknode(cur);
			delete cur;
			cout << "삭제" << endl;
			cur = next;
		}
		else
		{

			cur = cur->next;
		}
	} while (cur != head);
	if (!found) { cout << "찾지 못함" << endl; return; }
}
void DeleteAll_DL(string pos)
{
	if (head == nullptr) { cout << "찾지 못함" << endl; return; }
	Node* cur = head;
	bool found = false;
	do {
		if (cur->data.position == pos)
		{
			found = true;
			Node* next = cur->next;
			unlinknode(cur);
			delete cur;
			cout << "삭제" << endl;
			cur = next;
		}
		else
		{

			cur = cur->next;
		}
	} while (cur != head);
	if (!found) { cout << "찾지 못함" << endl; return; }
}
void FindMaxHp_DL()
{
	if (head == nullptr)return;
	
	Node* cur = head->next;
	int maxhp = head->data.hp;
	do {
		if (cur->data.hp > maxhp)maxhp = cur->data.hp;
		cur = cur->next;

	} while (cur != head);
	do {
		if (cur->data.hp == maxhp)printchamp(cur);
		cur = cur->next;
	} while (cur != head);
}
int main()
{
	legueoflegends LOL[172];
	int filecount = 0;

	ifstream file("testdata.txt");
	if (!file)
	{
		cout << "파일 오류";
		return 1;
	}
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

	Array2DLinkedList(LOL, filecount);

	while (1)
	{
		cout << "command를 입력: ";
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
			legueoflegends champ;
			cout << "이름을 입력해";
			cin >> champ.name;
			cout << "hp";
			cin >> champ.hp;
			cout << "mp";
			cin >> champ.mp;
			cout << "speed";
			cin >> champ.speed;
			cout << "range";
			cin >> champ.range;
			cout << "position";
			cin >> champ.position;
			Insert_DL(champ);
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
	}
}