#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

using namespace std;
using namespace chrono;

struct Champion {
	string positoin;
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

struct BTNode {
	Champion data;
	BTNode* left;
	BTNode* right;
	BTNode(Champion c) : data(c), left(nullptr), right(nullptr){}
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
		else
		{
			SNode* cur = head;
			while (cur->next) cur = cur->next;
			cur->next = newNode;
		}
		size++;
	}

	SNode* merge_SL(SNode* a, SNode* b)
	{
		if (!a) return b;
		if (!b) return a;
		if (a->data.hp <= b->data.hp)
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
		SNode* fast = fast->next;
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

	void SortByHp()
	{
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

class BinaryTree {
public:
	BTNode* root;

	BinaryTree() : root(nullptr) {}

	BTNode* Insert(BTNode* node, Champion c) {
		if (!node) return new BTNode(c);
		if (c.name < node->data.name)
			node->left = Insert(node->left, c);
		else if (c.name > node->data.name)
			node->right = Insert(node->right , c);
		return node;
		
	}

	void Insert_BT(Champion c)
	{
		root = Insert(root, c);
	}
};