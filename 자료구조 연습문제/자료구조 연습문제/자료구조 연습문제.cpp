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
	bool alive = true;
};

void PrintAll(legueoflegends LOL[], int i, int count)
{
	if (count == i) return;
	if (LOL[i].alive == true)
	{
		cout << LOL[i].name << " ";
		cout << LOL[i].hp << " ";
		cout << LOL[i].mp << " ";
		cout << LOL[i].speed << " ";
		cout << LOL[i].range << " ";
		cout << LOL[i].position << endl;
	}
	return PrintAll(LOL, i + 1, count);
}
int Search(legueoflegends LOL[], int i, int count, const string& champ)
{
	if (count == i) return-1;
	if (LOL[i].alive == true && LOL[i].name == champ)return 0;
	return Search(LOL, i + 1, count, champ);
}
void Insert(legueoflegends LOL[], int i, int &count)
{
	if (count == i)
	{
		cout << "새로운 챔피언을 입력하세요." << endl;
		getline(cin, LOL[i].name);
		cout << "hp를 입력하세요." << endl;
		cin >> LOL[i].hp;
		cout << "mp를 입력하세요." << endl;
		cin >> LOL[i].mp;
		cout << "speed를 입력하세요." << endl;
		cin >> LOL[i].speed;
		cout << "range를 입력하세요." << endl;
		cin >> LOL[i].range;
		cout << "position를 입력하세요." << endl;
		cin >> LOL[i].position;
		cin.ignore();
		count++;
		LOL[i].alive = true;
		return;
	}
	else if (LOL[i].alive == false)
	{
		cout << "새로운 챔피언을 입력하세요." << endl;
		getline(cin, LOL[i].name);
		cout << "hp를 입력하세요." << endl;
		cin >> LOL[i].hp;
		cout << "mp를 입력하세요." << endl;
		cin >> LOL[i].mp;
		cout << "speed를 입력하세요." << endl;
		cin >> LOL[i].speed;
		cout << "range를 입력하세요." << endl;
		cin >> LOL[i].range;
		cout << "position를 입력하세요." << endl;
		cin >> LOL[i].position;
		cin.ignore();
		LOL[i].alive = true;
		return;
	}
	return Insert(LOL, i + 1, count);
}
int Delete(legueoflegends LOL[], int i, int count, const string& champ)
{
	if (count == i) return-1;
	if (LOL[i].alive == true && LOL[i].name == champ)
	{
		LOL[i].alive = false;
		return 0;
	}
	return Delete(LOL, i + 1, count, champ);
}
int DeleteAll(legueoflegends LOL[], int i, int count, const string& pos)
{
	if (count == i) return-1;
	int erased = 0;
	if (LOL[i].alive == true && LOL[i].position == pos)
	{
		LOL[i].alive = false;
		erased = 1;
		
	}
	return erased + DeleteAll(LOL, i + 1, count, pos);
}
int MaxHpValue(legueoflegends LOL[], int i, int count)
{
	if (count == i) return -1;
	int rest = MaxHpValue(LOL, i + 1, count);

	if (!LOL[i].alive) return rest;
	if (rest == -1) return LOL[i].hp;
	return (LOL[i].hp > rest) ? LOL[i].hp : rest;
}
void PrintMaxHp(legueoflegends LOL[], int i, int count, int hp)
{
	if (count == i) return;
	if (LOL[i].alive && LOL[i].hp == hp)
	{
		cout << LOL[i].name << " ";
		cout << LOL[i].hp << " ";
		cout << LOL[i].mp << " ";
		cout << LOL[i].speed << " ";
		cout << LOL[i].range << " ";
		cout << LOL[i].position << endl;
	}
	PrintMaxHp(LOL, i + 1, count, hp);
}

int main()
{
	legueoflegends LOL[172];
	int count = 0;

	ifstream file("testdata.txt");
	if (!file)
	{
		cout << "파일오류";
	}
	string line;
	while (getline(file, line))
	{
		if (line.empty()) continue;
		string word[100];
		int wordcount = 0;
		string cur = "";
		for (int i = 0; i <= (int)line.size(); ++i)
		{
			if (i == (int)line.size() || line[i] == ' ' || line[i] == '\t')
			{
				if (!cur.empty())
				{
					word[wordcount++] = cur;
					cur = "";

				}

			}
			else
			{
				cur += line[i];
			}
		}
		LOL[count].name = "";
		int i = 0;
		while (i < wordcount && !isdigit(word[i][0]))
		{
			if (i > 0)LOL[count].name += " ";
			LOL[count].name += word[i];
			++i;
		}
		LOL[count].hp = stoi(word[i++]);
		LOL[count].mp = stoi(word[i++]);
		LOL[count].speed = stoi(word[i++]);
		LOL[count].range = stoi(word[i++]);
		LOL[count].position = (word[i]);
		count++;
	}
	file.close();
	while (1)
	{
		string command;
		cout << "명령어를 입력하세요." << endl;
		cin >> command;
		cin.ignore();
		if (command == "PrintAll")
		{
			PrintAll(LOL, 0, count);
		}
		else if (command == "Search")
		{
			string champ;
			getline(cin, champ);
			int i = Search(LOL, 0, count, champ);
			if (i == -1)
			{
				cout << "그런 챔피언없음";
			}
			else
			{
				cout << LOL[i].name << " ";
				cout << LOL[i].hp << " ";
				cout << LOL[i].mp << " ";
				cout << LOL[i].speed << " ";
				cout << LOL[i].range << " ";
				cout << LOL[i].position << endl;
			}


		}
		else if (command == "Insert")
		{
			Insert(LOL, 0, count);
		}
		else if (command == "Delete")
		{
			string champ;
			getline(cin, champ);
			int i = Delete(LOL, 0, count, champ);
			if (i == -1)
			{
				cout << "그런 챔피언없음";
			}
			else
			{
				cout << "삭제완료" << endl;
			}
		}
		else if (command == "DeleteAll")
		{
			string pos;
			cin >> pos;
			int erased = DeleteAll(LOL, 0, count, pos);
			if (erased == -1)
			{
				cout << "그런 챔피언없음";
			}
			else
			{
				cout << "삭제완료" << endl;
			}
		}
		else if (command == "FindMaxHp")
		{
			int maxhp = MaxHpValue(LOL, 0, count);
			if (maxhp == -1)cout << "노데이터";
			else PrintMaxHp(LOL, 0, count, maxhp);
		}
		else if (command == "SortByHp")
		{
			
		}
		else
		{
			cout << "정상적인 명령어가 아님" << endl;
		}
	}
}