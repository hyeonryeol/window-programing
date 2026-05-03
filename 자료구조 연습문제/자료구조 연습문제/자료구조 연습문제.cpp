#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct legueoflegends {
	string name;
	int hp, mp, speed, range;
	string position;
};

struct Node {
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
		newNode->next = nullptr;
		newNode->prev = nullptr;

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
	if (head == nullptr) { cout << " 리스트가 비었음" << endl; return; }
	Node* cur = head;
	do
	{
		printchamp(cur);
		cur = cur->next;
	} while (cur != head);
}
void Search_DL(string champ)
{
	if (head == nullptr) { cout << "찾지못함" << endl; return; }
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
}
void unlinkednode(Node* node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;

	if (head == nullptr)
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
int main()
{
	legueoflegends LOL[172];
	int filecount = 0;

	ifstream file("testdata.txt");
	if (!file)
	{
		cout << "파일오류";
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

		}
	}
}