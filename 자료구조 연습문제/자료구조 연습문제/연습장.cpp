#include <iostream>
#include <string>
#include <fstream>

using namespace std;

struct Champion {
	string name;
	int hp;
	int mp;
	int speed;
	int range;
	string position;
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
			newNode->next = head;
		}
		else
		{
			SNode* last = head;
			while (last->next != head) last = last->next;
			last->next = newNode;
			newNode->next = head;
		}
		size++;
	}

	void PrintAll_SL() {
		SNode* cur = head;
		do {
			cout << cur->data.name << " "
				<< cur->data.hp << " "
				<< cur->data.mp << " "
				<< cur->data.range << " "
				<< cur->data.speed << " "
				<< cur->data.position << endl;
			cur = cur->next;
		} while (cur != head);
	}

	void Search_SL(string champ) {
		SNode* cur = head;
		do {
			if (champ == cur->data.name)
			{
				cout << cur->data.name << " "
					<< cur->data.hp << " "
					<< cur->data.mp << " "
					<< cur->data.range << " "
					<< cur->data.speed << " "
					<< cur->data.position << endl;
			}
			cur = cur->next;
		} while (cur != head);
	}

	void Delete_SL(string name)
	{
		if (!head) { cout << "없음" << endl; return; }
		bool found = false;

		while (head && head->data.name == name)
		{
			found = true;
			if (head->next == head)
			{
				delete head;
				head = nullptr;
				size--;
				return;
			}
			SNode* last = head;
			while (last->next != head) last = last->next;

			SNode* todelete = head;
			head = head->next;
			last->next = head;
			delete todelete;
			size--;
		}
		if (!head) return;
		SNode* prev = head;
		SNode* cur = head->next;
		while (cur != head)
		{
			if (cur->data.name == name)
			{
				found = true;
			prev->next = cur->next;
			delete cur;
			cur = prev->next;
			size--;
			}
			else
			{
				prev = cur;
				cur = cur->next;
			}

		}
	}

	void FindMaxHp_SL()
	{
		SNode* cur = head;
		Champion maxhp;
		maxhp = cur->data;
		do {
			if (cur->data.hp > maxhp.hp)
				maxhp = cur->data;
			cur = cur->next;
		} while (cur != head);

		do {
			if (maxhp.hp == cur->data.hp)
			{
				cout << cur->data.name << " "
					<< cur->data.hp << " "
					<< cur->data.mp << " "
					<< cur->data.range << " "
					<< cur->data.speed << " "
					<< cur->data.position << endl;
			}
			cur = cur->next;
		} while (cur != head);
	}

	SNode* mergeSort(SNode* a, SNode* b)
	{
		if (!a) return b;
		if (!b) return a;
		if (a->data.hp >= b->data.hp)
		{
			a->next = mergeSort(a->next, b);
			return a;
		}
		else
		{
			b->next = mergeSort(a, b->next);
			return b;
		}

	}
	SNode* mergeSortHp(SNode* node)
	{
		if (!node || !node->next) return node;
		SNode* slow = node;
		SNode* fast = node->next;
		while (fast && fast->next)
		{
			slow = slow->next;
			fast = fast->next->next;
		}
		SNode* mid = slow->next;
		slow->next = nullptr;

		SNode* left = mergeSortHp(node);
		SNode* right = mergeSortHp(mid);
		return mergeSort(left, right);
	}
	void SortByHp()
	{
		SNode* tail = head;
		while (tail->next != head) tail = tail->next;
		tail->next = nullptr;
		head = mergeSortHp(head);
		SNode* newtail = head;
		while (newtail->next) newtail = newtail->next;
		newtail->next = head;
	}
};

void Loadfile(const string& filename, SinglyLinkedList& sl)
{
	ifstream fin(filename);
	if (!fin.is_open())
	{
		cout << "파일오류" << endl;
		return;
	}
	Champion c;
	while (fin >> c.name >> c.hp >> c.mp >> c.speed >> c.range >> c.position)
	{
		sl.Insert_SL(c);
	}
	fin.close();
	cout << "로드완료" << endl;
}

int main()
{
	SinglyLinkedList sl;
	Loadfile("testdata.txt", sl);

	while (1)
	{
		cout << "명령어를 입력" << endl;
		string cmd;
		cin >> cmd;
		if (cmd == "PrintAll")
		{
			sl.PrintAll_SL();
		}
		else if (cmd == "Search")
		{
			string champ;
			cin >> champ;
			sl.Search_SL(champ);
		}
		else if (cmd == "Delete")
		{
			string champ;
			cin >> champ;
			sl.Delete(champ);
		}
		else if (cmd == "Insert")
		{
			Champion c;
			cout << "이름, hp, mp, speed, range, position" << endl;
			cin >> c.name >> c.hp >> c.mp >> c.speed >> c.range >> c.position;
			sl.Insert_SL(c);
		}
		else if (cmd == "SortByHp")
		{
			sl.SortByHp_SL();
			cout << "정렬 완료" << endl;
		}
	}
}
