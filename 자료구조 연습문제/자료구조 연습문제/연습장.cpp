#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct legueoflegends
{
	string name;
	int hp;
	int mp;
	int speed;
	int range;
	string position;
};
struct Node
{
	legueoflegends data;
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
			head = newNode;
			head->next = newNode;
		}
		else
		{
			Node* last = head;
			while (last->next != head)
				last = last->next;
			last->next = newNode;
			newNode->next = head;
		}
		listcount++;
	}
}
void PrintAll_SL()
{
	if (head == nullptr) { cout << "리스트가 비었음" << endl; return; }
	Node* cur = head;
	do
	{
		printchamp(cur);
		cur = cur->next;
	} while (cur != head);
}
void Search_SL(string champ)
{
	if (head == nullptr) { cout << "찾지못함" << endl; return; }
	Node* cur = head;
	bool found = false;
	do
	{
		if (cur->data.name == champ)
		{
			printchamp(cur);
			found = true;
		}
			cur = cur->next;
	} while (cur != head);
	if (!found) { cout << "찾지 못함"; }
}
void FindMaxHp_SL()
{
	if (head == nullptr) return;
	int maxhp = head->data.hp;
	Node* cur = head -> next;
	while(cur != head)
	{
		if (cur->data.hp > maxhp) maxhp = cur->data.hp;
		cur = cur->next;
	}
	cur = head;
	do
	{
		if (cur->data.hp == maxhp) printchamp(cur);
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

	Array2SLinkedList(LOL, filecount);

	while (1)
	{
		cout << "명령어를 입력하세요.";
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
			cin >> champ;
			cin.ignore();
			Search_SL(champ);
		}
		else if (command == "FindMaxHp")
		{
			FindMaxHp_SL();
		}

	}
}