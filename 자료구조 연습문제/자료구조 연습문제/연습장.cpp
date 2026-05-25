#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

using namespace std;
using namespace chrono;

struct Champion {
	string position;
	string name;
	int hp;
	int attack;
	int armor;

};

struct SNode {
	Champion data;
	SNode* next;
	SNode(Champion c) : data(c), next(nullptr) {}
};

struct DNode {
	Champion data;
	DNode* prev;
	DNode* next;
	DNode(Champion c) : data(c), prev(nullptr), next(nullptr) {}
};

class SinglyLinkedList {
public:
	SNode* head;
	int size;
	SinglyLinkedList() : head(nullptr), size(0) {}

	void Insert(Champion c) {
		SNode* newNode = new SNode(c);
		if (!head) {
			head = newNode;

		}
		else {
			SNode* cur = head;
			while (cur->next) cur = cur->next;
			cur->next = newNode;
		}
		size++;
	}

	void PrintAll_SL() {
		SNode* cur = head;
		while (cur) {
			cout << cur->data.position << " "
				<< cur->data.name << " "
				<< cur->data.hp << " "
				<< cur->data.attack << " "
				<< cur->data.armor << endl;
			cur = cur->next;
		}
	}

	SNode* merge_SL(SNode* a, SNode* b) {
		if (!a) return b;
		if (!b) return a;
		if (a->data.hp <= b->data.hp) {
			a->next = merge_SL(a->next, b);
			return a;
		}
		else
		{
			b->next = merge_SL(a, b->next);
			return b;
		}
	}

	SNode* mergeSort_SL(SNode* node) {
		if (!node || !node->next) return node;

		SNode* slow = node;
		SNode* fast = node->next;
		while (fast && fast->next) {
			slow = slow->next;
			fast = fast->next->next;
		}

		SNode* mid = slow->next;
		slow->next = nullptr;

		SNode* left = mergeSort_SL(node);
		SNode* right = mergeSort_SL(mid);
		return merge_SL(left, right);

	}

	void SortByHp_SL() {
		head = mergeSort_SL(head);
	}

	Champion FindMaxHp_SL() {
		SNode* cur = head;
		Champion maxChamp = cur->data;
		while (cur)
		{
			if (cur->data.hp > maxChamp.hp)
				maxChamp = cur->data;
			cur = cur->next;
		}
		return maxChamp;
	}

	~SinglyLinkedList() {
		SNode* cur = head;
		while(cur) {
			SNode* next = cur->next;
			delete cur;
			cur = next;

		}
	}
};

class DoublyLinkedList {
public:
	DNode* head;
	DNode* tail;
	int size;

	DoublyLinkedList() : head(nullptr), tail(nullptr), size(0) {}

	void Insert(Champion c) {
		DNode* newNode = new DNode(c);
		if (!tail) {
			head = tail = newNode;
		}
		else
		{ 
			tail->next = newNode;
			newNode->prev = tail;
			tail = newNode;
		}
		size++;
	}

	void PrintAll_DL() {
		DNode* cur = head;
		while (cur) {
			cout << cur->data.position << " "
				<< cur->data.name << " "
				<< cur->data.hp << " "
				<< cur->data.attack << " "
				<< cur->data.armor << endl;
			cur = cur->next;
		}
	}

	DNode* merge_DL(DNode* a, DNode* b) {
		if (!a) return b;
		if (!b) return a;
		if (a->data.hp <= b->data.hp) {
			a->next = merge_DL(a->next, b);
			if (a->next) a->next->prev = a;
			a->prev = nullptr;
			return a;
		}
		else {
			b->next = merge_DL(a, b->next);
			if (b->next) b->next->prev = b;
			b->prev = nullptr;
			return b;
		}
	}

	DNode* mergeSort_DL(DNode* node) {
		if (!node || !node->next) return node;

		DNode* slow = node;
		DNode* fast = node->next;
		while (fast && fast->next) {
			slow = slow->next;
			fast = fast->next->next;
		}

		DNode* mid = slow->next;
		slow->next = nullptr;

		DNode* left = mergeSort_DL(node);
		DNode* right = mergeSort_DL(mid);
		return merge_DL(left, right);
	}

	void SortByHp_DL()
	{
		head = mergeSort_DL(head);
		tail = head;
		while (tail && tail->next) tail = tail->next;
	}

	Champion FindMaxHp_DL() {
		DNode* cur = head;
		Champion maxChamp = cur->data;
		while (cur) {
			if (cur->data.hp > maxChamp.hp)
				maxChamp = cur->data;
			cur = cur->next;
		}
		return maxChamp;
	}

	~DoublyLinkedList() {
		DNode* cur = head;
		while (cur) {
			DNode* next = cur->next;
			delete cur;
			cur = next;
		}
	}
};

void LoadFile(const string& filename, SinglyLinkedList& sl, DoublyLinkedList& dl) {
	ifstream fin(filename);
	if (!fin.is_open()) {
		cerr << "파일 오류" << filename << endl;
		return;
	}

	Champion c;
	while (fin >> c.position >> c.name >> c.hp >> c.attack >> c.armor) {
		sl.Insert(c);
		dl.Insert(c);

	}
	fin.close();
	cout << "로드 완료. 총" << sl.size << "개 데이터" << endl;
}

#define MEASURE(label, func) \
{ \
	auto start = high_resolution_clock::now(); \
	func; \
	auto end = high_resolution_clock::now(); \
	auto us = duration_cast<microseconds>(end - start).count(); \
	cout <<label <<"수행시간:" <<us << "us" <<endl; \
}

int main()
{
	SinglyLinkedList sl;
	DoublyLinkedList dl;

	LoadFile("test.txt", sl, dl);

	cout << "1.PrintAll 수행시간 비교:" << endl;
	MEASURE("PrintAll_SL", sl.PrintAll_SL());
	cout << endl;
	MEASURE("printAll_DL", dl.PrintAll_DL());

	cout << "2.SortByHp 수행시간 비교";
	MEASURE("SortByHp_SL", sl.SortByHp_SL());
	cout << endl;
	MEASURE("SortByHp_DL", dl.SortByHp_DL());

	cout << "3.FindMaxHp 수행시간비교";
	MEASURE("FindMaxHp_SL", sl.FindMaxHp_SL());
	cout << endl;
	MEASURE("FindMaxHp_DL", dl.FindMaxHp_DL());

	return 0;
}