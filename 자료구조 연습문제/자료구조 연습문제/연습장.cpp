#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

using namespace std;
using namespace chrono;

struct champion {
	string position;
	string name;
	int hp;
	int attack;
	int defense;

};

struct SNode {
	champion data;
	SNode* next;
	SNode(champion c) : data(c), next(nullptr) {}
};

class SinglyLinkedList {
public: 
	SNode* head;
	int size;

	SinglyLinkedList() : head(nullptr), size(0) {}

	void Insert(champion c)
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

	void PrintAll_SL()
	{
		SNode* cur = head;
		while (cur) {
			cout << cur->data.position << " "
				<< cur->data.name << " "
				<< cur->data.hp << " "
				<< cur->data.attack << " "
				<< cur->data.defense << endl;
			cur = cur->next;
		}
	}

	SNode* merge_SL(SNode* a, SNode* b)
	{
		if (!a) return b;
		if (!b) return a;
		if (a->data.hp >= b->data.hp)
		{
			a->next = merge_SL(a->next, b);
			return a;
		}
		else
		{
			b->next = merge_SL(a, b->next);
			return b;
		}
	}

	SNode* mergeSort_SL(SNode* node)
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

		SNode* left = mergeSort_SL(node);
		SNode* right = mergeSort_SL(mid);
		return merge_SL(left, right);
	}

	void SortByHp_SL()
	{
		head = mergeSort_SL(head);
	}


	champion FindMaxHp_SL()
	{
		SNode* cur = head;
		champion maxchamp = cur->data;
		while (cur) {
			if (cur->data.hp > maxchamp.hp)
				maxchamp = cur->data;
			cur = cur->next;
		}
		return maxchamp;
	}


	~SinglyLinkedList() {
		SNode* cur = head;
		while (cur)
		{
			SNode* next = cur->next;
			delete cur;
			cur = next;
		}
	}

};

void Loadfile(const string& filename, SinglyLinkedList& sl)
{
	ifstream fin(filename);
	if (!fin.is_open()) {
		cout << "파일 오류" << endl;
		return;
	}
	champion c;
	while (fin >> c.position >> c.name >> c.hp >> c.attack >> c.defense) {
		sl.Insert(c);
	}
	fin.close();
	cout << "로드 완료" << endl;
}

#define MEASURE(label, func)  {auto start = high_resolution_clock::now(); func; auto end = high_resolution_clock::now(); auto us = duration_cast<microseconds>(end - start).count(); cout << label << "수행시간" << us << "us"<<endl;}




int main()
{
	SinglyLinkedList sl;

	Loadfile("test.txt", sl);
	
	sl.SortByHp_SL();

	MEASURE("PrintAll_SL", sl.PrintAll_SL());

	champion maxsl;
	maxsl = sl.FindMaxHp_SL();
	cout << "최대체력" << endl;
	cout << "이름:" << maxsl.name << "체력:" << maxsl.hp;

}

