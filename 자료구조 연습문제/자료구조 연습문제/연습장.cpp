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
	int defense;
};

struct SNode {
	Champion data;
	SNode* next;
	SNode(Champion c) : data(c), next(nullptr) {}
};

struct BTNode
{
	Champion data;
	BTNode* left;
	BTNode* right;
	BTNode(Champion c) : data(c), left(nullptr), right(nullptr) {}
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
		if (a->data.name <= b->data.name)
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

	void SortByName_SL()
	{
		head = mergeSort_SL(head);
	}

	~SinglyLinkedList()
	{
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

	BTNode* insertNode(BTNode* node, Champion c)
	{
		if (!node) return new BTNode(c);
		if (c.name < node->data.name)
		{
			node->left = insertNode(node->left, c);
		}
		else if (c.name > node->data.name)
		{
			node->right = insertNode(node->right, c);
		}
		return node;
	}

	void Insert_BT(Champion c)
	{
		root = insertNode(root, c);
	}

	BTNode* searchNode(BTNode* node, string name)
	{
		if (!node) return nullptr;
		if (name == node->data.name) return node;
		else if (name < node->data.name) return searchNode(node->left, name);
		return searchNode(node->right, name);
	}

	Champion* SearchByName_BT(string name)
	{
		BTNode* result = searchNode(root, name);
		if (result) return &result->data;
		return nullptr;

	}
	BTNode* findMin(BTNode* node)
	{
		while (node->left) node = node->left;
		return node;
	}
	BTNode* deleteNode(BTNode* node, string name)
	{
		if (!node) return nullptr;
		if (name < node->data.name)
		{
			node->left = deleteNode(node->left, name);
		}
		else if (name > node->data.name)
		{
			node->right = deleteNode(node->right, name);
		}
		else
		{
			if (!node->left)
			{
				BTNode* temp = node->right;
				delete node;
				return temp;
			}
			else if (!node->right)
			{
				BTNode* temp = node->left;
				delete node;
				return temp;
			}
			else
			{
				BTNode* minNode = findMin(node->right);
				node->data = minNode->data;
				node->right = deleteNode(node->right, minNode->data.name);
			}
		}
		return node;
	}

	void Delete_BT(string name)
	{
		root = deleteNode(root, name);
	}

	void inorder(BTNode* node)
	{
		if (!node) return;
		inorder(node->left);
		cout << node->data.position << " "
			<< node->data.name << " "
			<< node->data.hp << " "
			<< node->data.attack << " "
			<< node->data.defense << endl;
		inorder(node->right);
	}

	void PrintAll_BT()
	{
		inorder(root);
	}
	void SortByName_BT()
	{
		inorder(root);
	}

	void destoryTree(BTNode* node)
	{
		if (!node) return;
		destoryTree(node->left);
		destoryTree(node->right);
		delete node;
	}

	~BinaryTree() {
		destoryTree(root);
	}
};

void Loadfile(const string& filename, SinglyLinkedList& sl, BinaryTree& bt)
{
	ifstream fin(filename);
	if (!fin.is_open()) {
		cout << "파일오류" << endl;
		return;
	}
	Champion c;
	while (fin >> c.position >> c.name >> c.hp >> c.attack >> c.defense)
	{
		sl.Insert_SL(c);
		bt.Insert_BT(c);
	}
	fin.close();
	cout << "로드완료.총" << sl.size << "개 데이터" << endl;
}

#define MEASURE(label, func) {auto start = high_resolution_clock::now(); func; auto end = high_resolution_clock::now(); auto us = duration_cast<microseconds>(end - start).count(); cout << label<<"수행시간" <<us <<"us"<<endl;}

int main()
{
	SinglyLinkedList sl;
	BinaryTree bt;

	Loadfile("test.txt", sl, bt);

	cout << "1" << endl;
	MEASURE("SortByName_SL", sl.SortByName_SL());

	cout << "SearchByName_BT" << endl;
	
	MEASURE("SearchByName_BT", bt.SearchByName_BT("P54151216"));

	cout << "Insert_BT" << endl;
	Champion newchamp = { "MID", "TESCHAMP", 5000, 2000, 3000 };
	MEASURE("Insert_BT", bt.Insert_BT(newchamp));

	cout << "Delete_BT" << endl;
	MEASURE("Delete_BT", bt.Delete_BT("TESCHAMP"));

	cout << "PritnAll_BT" << endl;
	MEASURE("PrintAll_BT", bt.PrintAll_BT());

	cout << "SortByName_BT" << endl;
	MEASURE("SortByName_BT", bt.SortByName_BT());
	return 0;
}