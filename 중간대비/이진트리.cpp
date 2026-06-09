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

struct BTNode {
	Champion data;
	BTNode* left;
	BTNode* right;
	BTNode(Champion c) : data(c), left(nullptr), right(nullptr) {}
};

class BinaryTree {
public:
	BTNode* root;
	int size;

	BinaryTree() : root(nullptr), size(0) {}

	BTNode* insertNode(BTNode* node, Champion c)
	{
		if (!node) return new BTNode(c);
		if (c.name < node->data.name)
			node->left = insertNode(node->left, c);
		else if (c.name > node->data.name)
			node->right = insertNode(node->right, c);
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
		if (name < node->data.name) return searchNode(node->left, name);
		return searchNode(node->right, name);
	}

	Champion* SearchByName(string name)
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
			node->left = deleteNode(node->left, name);
		else if (name > node->data.name)
			node->right = deleteNode(node->right, name);
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

	void findMaxHp(BTNode* node, int& maxHp)
	{
		if (!node) return;
		if (node->data.hp > maxHp)
			maxHp = node->data.hp;
		findMaxHp(node->left, maxHp);
		findMaxHp(node->right, maxHp);
	}

	void printMaxHp(BTNode* node, int maxHp)
	{
		if (!node) return;
		if (node->data.hp == maxHp)
			cout << node->data.position << " "
			<< node->data.name << " "
			<< node->data.hp << " "
			<< node->data.attack << " "
			<< node->data.defense << endl;
		printMaxHp(node->left, maxHp);
		printMaxHp(node->right, maxHp);
	}

	void FindMaxHp_BT()
	{
		if (!root) return;
		int maxHp = root->data.hp;
		findMaxHp(root, maxHp);
		printMaxHp(root, maxHp);
	}
};

void Loadfile(const string& filename, BinaryTree& bt)
{
	ifstream fin(filename);
	if (!fin.is_open()) { cout << "파일 오류" << endl; return; }
	Champion c;
	while (fin >> c.position >> c.name >> c.hp >> c.attack >> c.defense)
	{
		bt.Insert_BT(c);
	}
	fin.close();
}

int main()
{
	BinaryTree bt;
	Loadfile("test.txt", bt);

	while (1)
	{
		string cmd;
		cout << "명령어" << endl;
		cin >> cmd;
		if (cmd == "PrintAll")
		{
			bt.PrintAll_BT();
		}
		else if (cmd == "Search")
		{
			string name;
			cin >> name;
			Champion* result = bt.SearchByName(name);
			if (result)
				cout << result->position << " "
				<< result->name << " "
				<< result->hp << " "
				<< result->attack << " "
				<< result->defense << endl;
			else
				cout << "찾지 못함" << endl;
		}
		else if (cmd == "Delete")
		{
			string name;
			cin >> name;
			bt.Delete_BT(name);
		}
		else if (cmd == "Insert")
		{
			cout << "포지션, 이름, hp, attack, defense" << endl;
			Champion c;
			cin >> c.position >> c.name >> c.hp >> c.attack >> c.defense;
			bt.Insert_BT(c);
		}
		else if (cmd == "FindMaxHp")
		{
			bt.FindMaxHp_BT();
		}
		else if (cmd == "SortByName")
		{
			bt.SortByName_BT();
		}
	}
}
