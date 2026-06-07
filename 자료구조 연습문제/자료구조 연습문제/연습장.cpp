#include <iostream>
#include <fstream>
#include <string>

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

	void Insert(Champion c)
	{
		SNode* newNode = new SNode(c);
		if (!head)
		{
			head = newNode;
		}
		else {
			SNode* cur = head;
			while (cur->next) cur = cur->next;
			cur->next = newNode;
		}
		size++;
	}

	SNode* merge_SL(SNode* a, SNode* b)
	{
		if (!a)return b;
		if (!b) return a;
		if (a->data.hp >= b->data.hp)
		{
			a->next = merge_SL(a->next, b);
			return a;
		}
		else {
			b->next = merge_SL(a, b->next);
			return b;
		}
	}

	SNode* mergeSort_SL(SNode* node) {
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

	void SortByHp() {
		head = mergeSort_SL(head);
	}


	~SinglyLinkedList() {
		SNode* cur = head;
		while (cur) {
			SNode* next = cur->next;
			delete cur;
			cur = next;
		}
	}
};

Loadfile
