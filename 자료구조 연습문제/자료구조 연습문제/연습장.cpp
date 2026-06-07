#include <iostream>
#include <string>
#include <fstream>

using namespace std;

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

class SinglyLinkedList {
public:
	SNode* head;
	int size;

	SinglyLinkedList() : head(nullptr), size(0) {}

	void Insert_SL(Champion c)
	{
		SNode* newNode = new SNode(c);
		if (!head)
		{
			head = newNode;
		}
		else
		{
			SNode* cur = head;
			while (cur->next) cur = cur->next;
			cur->next = newNode;
		}
		size++;
	}
	void Insertsorted(Champion c)
	{
		SNode* newNode = new SNode(c);
		if (!head || head->data.hp >= c.hp)
		{
			newNode->next = head;
			head = newNode;
		}
		else {
			SNode* cur = head;
			while (cur->next && cur->next->data.hp < c.hp)
				cur = cur->next;
			newNode->next = cur->next;
			cur->next = newNode;
		}
		size++;
	}
	void PrintAll()
	{
		SNode* cur = head;
		while (cur)
		{
			cout << cur->data.position << " "
				<< cur->data.name << " "
				<< cur->data.hp << " "
				<< cur->data.attack << " "
				<< cur->data.defense << endl;
			cur = cur->next;
		}
	}
	void Search_SL(string champ)
	{
		SNode* cur = head;

		while (cur)
		{
			if (champ == cur->data.name)
			{
				cout << cur->data.position << " "
					<< cur->data.name << " "
					<< cur->data.hp << " "
					<< cur->data.attack << " "
					<< cur->data.defense << endl;

			}
			cur = cur->next;
		}
	}
	void Delete(string champ)
	{

		if (!head)
		{
			cout << "리스트가 비었음" << endl;
			return;
		}
		else if (champ == head->data.name)
		{
			SNode* del = head;
			head = head->next;
			delete del;
			size--;
			return;
		}
		SNode* cur = head;
		while (cur->next && cur->next->data.name != champ)
			cur = cur->next;
		if (!cur->next)
		{
			cout << "없음" << endl;
			return;
		}
		SNode* del = cur->next;
		cur->next = del->next;
		delete del;
		size--;

	}
};

void Loadfile(const string& filename, SinglyLinkedList& sl)
{
	ifstream fin(filename);
	if (!fin.is_open()) {
		cout << "파일 오류" << endl;
		return;
	}
	Champion c;
	while (fin >> c.position >> c.name >> c.hp >> c.attack >> c.defense)
	{
		sl.Insert_SL(c);
	}
	fin.close();

}

int main()
{
	SinglyLinkedList sl;
	Loadfile("test.txt", sl);
	while (1)
	{
		string cmd;
		cin >> cmd;
		if (cmd == "printall")
		{
			sl.PrintAll();
		}
		else if (cmd == "insert")
		{
			Champion c;
			cout << "포지션을 입력하세요." << endl;
			cin >> c.position;
			cout << "이름을 입력" << endl;
			cin >> c.name;
			cout << "hp" << endl;
			cin >> c.hp;
			cout << "attack" << endl;
			cin >> c.attack;
			cout << "defense" << endl;
			cin >> c.defense;
			sl.Insertsorted(c);
			cout << "입력완료" << endl;
		}
		else if (cmd == "Search")
		{
			cout << "찾을 이름" << endl;
			string champ;
			cin >> champ;
			sl.Search_SL(champ);
		}
		else if (cmd == "delete")
		{
			string name;
			cin >> name;
			sl.Delete(name);
		}
	}
}
