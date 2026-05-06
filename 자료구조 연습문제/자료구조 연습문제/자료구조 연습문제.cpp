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
			head->prev = head;
			head->next = head;
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
	if (head == nullptr) { cout << "리스트가 비었음 " << endl; return; }
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
		if (champ == cur->data.name)
		{
			found = true;
			printchamp(cur);

		}
		cur = cur->next;
	} while (cur != head);
}
void unlinknode(Node* node)
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
	target->prev = newNode;
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
		head->prev = head;
		head->next = head;
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
	Node* cur = head;
	bool found = false;
	do {
		if (champ == cur->data.name)
		{
			found = true;
			Node* next = cur->next;
			unlinknode(cur);
			delete cur;
			next = cur;

		}
		else
		{
		cur = cur->next;

		}
	} while (cur != head);
}
void FindMaxHp_DL()
{
	if (head == nullptr) return;
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
void SortByHp_DL()
{
	if (head == nullptr)return;
	Node* unsort = head;
	while (unsort->next != head)
	{
		Node* maxnode = unsort;
		Node* cur = unsort->next;
		while (cur != head)
		{
			if (cur->data.hp > maxnode->data.hp)
				maxnode = cur;
			cur = cur->next;
			
		}
		if (maxnode == unsort)
		{
			unsort = unsort->next;
		}
		else
		{
			maxnode->prev->next = maxnode->next;
			maxnode->next->prev = maxnode->prev;

			Node* before = unsort->prev;
			before->next = maxnode;
			maxnode->prev = before;
			maxnode->next = unsort;
			unsort->prev = maxnode;
			if (unsort == head) head = maxnode;
		}
	}
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
		cout << "명령어를 입력하세요.:";
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
		else if (command == "FindMaxHp")
		{
			FindMaxHp_DL();
		}
		else if (command == "SortByHp")
		{
			SortByHp_DL();
		}
	}
}