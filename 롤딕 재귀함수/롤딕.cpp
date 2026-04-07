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
	if (i == count)return;
	if (LOL[i].alive == true)
	{
		cout << LOL[i].name << " ";
		cout << LOL[i].hp << " ";
		cout << LOL[i].mp << " ";
		cout << LOL[i].speed << " ";
		cout << LOL[i].range << " ";
		cout << LOL[i].position << endl;
	}
	PrintAll(LOL, i + 1, count);
}

int Search(legueoflegends LOL[], int i, int count, const string& champ)
{
	if (i == count)return -1;
	if (LOL[i].alive == true && champ == LOL[i].name)return i;

	return Search(LOL, i + 1, count, champ);
}

void Insert(legueoflegends LOL[], int i, int& count)
{
	if (i == count)
	{
		cout << "새로운 챔피언의 이름을 입력하세요." << endl;
		getline(cin, LOL[count].name);
		cout << "hp를 입력하세요." << endl;
		cin >> LOL[count].hp;
		if (cin.fail())
		{
			cout << "잘못된 입력입니다.";
			exit(0);
		}
		cout << "mp를 입력하세요." << endl;
		cin >> LOL[count].mp;
		if (cin.fail())
		{
			cout << "잘못된 입력입니다.";
			exit(0);
		}
		cout << "speed를 입력하세요." << endl;
		cin >> LOL[count].speed;
		if (cin.fail())
		{
			cout << "잘못된 입력입니다.";
			exit(0);
		}
		cout << "range를 입력하세요." << endl;
		cin >> LOL[count].range;
		if (cin.fail())
		{
			cout << "잘못된 입력입니다.";
			exit(0);
		}
		cout << "position를 입력하세요." << endl;
		cin >> LOL[count].position;
		if (cin.fail())
		{
			cout << "잘못된 입력입니다.";
			exit(0);
		}
		LOL[count].alive = true;
		count++;
		cin.ignore();
		return;
	}

	if (LOL[i].alive == false)
	{
		cout << "새로운 챔피언의 이름을 입력하세요." << endl;
		getline(cin, LOL[i].name);
		cout << "hp를 입력하세요." << endl;
		cin >> LOL[i].hp;
		if (cin.fail())
		{
			cout << "잘못된 입력입니다.";
			exit(0);
		}
		cout << "mp를 입력하세요." << endl;
		cin >> LOL[i].mp;
		if (cin.fail())
		{
			cout << "잘못된 입력입니다.";
			exit(0);
		}
		cout << "speed를 입력하세요." << endl;
		cin >> LOL[i].speed;
		if (cin.fail())
		{
			cout << "잘못된 입력입니다.";
			exit(0);
		}
		cout << "range를 입력하세요." << endl;
		cin >> LOL[i].range;
		if (cin.fail())
		{
			cout << "잘못된 입력입니다.";
			exit(0);
		}
		cout << "position를 입력하세요." << endl;
		cin >> LOL[i].position;
		if (cin.fail())
		{
			cout << "잘못된 입력입니다.";
			exit(0);
		}
		LOL[i].alive = true;
		cin.ignore();
		return;
	}
	Insert(LOL, i + 1, count);
}

int Delete(legueoflegends LOL[], int i, int count, const string& champ)
{
	if (i == count)return -1;
	if (LOL[i].alive == true && champ == LOL[i].name)
	{
		LOL[i].alive = false;
		return i;
	}

	return Delete(LOL, i + 1, count, champ);
}

int DeleteAll(legueoflegends LOL[], int i, int count, const string& pos)
{
	if (i == count)return 0;
	int erased = 0;
	if (LOL[i].alive == true && pos == LOL[i].position)
	{
		LOL[i].alive = false;
		erased = 1;
	}

	return erased + DeleteAll(LOL, i + 1, count, pos);
}

int FirstAlive(legueoflegends LOL[], int i, int count) //초기값잡기
{
	if (i == count) return -1;
	if (LOL[i].alive) return i;
	return FirstAlive(LOL, i + 1, count);
}
int FindMaxHP(legueoflegends LOL[], int i, int count, int maxhp) //비교
{
	if (i == count) return maxhp;
	if (LOL[i].alive && LOL[i].hp > maxhp) maxhp = LOL[i].hp;
	return FindMaxHP(LOL, i + 1, count, maxhp);
}
void PrintMaxHp(legueoflegends LOL[], int i, int count, int maxhp) //비교 프린트
{
	if (i == count)return;
	if (LOL[i].alive && LOL[i].hp == maxhp)
	{
		cout << LOL[i].name << " "
			<< LOL[i].hp << " "
			<< LOL[i].mp << " "
			<< LOL[i].speed << " "
			<< LOL[i].range << " "
			<< LOL[i].position << endl;
	}
	PrintMaxHp(LOL, i + 1, count, maxhp);

}
int FindSortHP(legueoflegends LOL[], int j, int count, int best)
{
	if (j == count) return best;

	// 둘 다 살아있을 때만 hp 비교
	if (LOL[best].alive && LOL[j].alive && LOL[j].hp > LOL[best].hp)
	{
		best = j;
	}

	return FindSortHP(LOL, j + 1, count, best);
}
void SortByHp(legueoflegends LOL[], int i, int count)
{
	if (i >= count - 1) return;

	// 시작점이 죽어있으면 이 자리는 건너뜀
	if (!LOL[i].alive) {
		SortByHp(LOL, i + 1, count);
		return;
	}

	int best = FindSortHP(LOL, i + 1, count, i);
	if (best != i)
	{
		legueoflegends temp = LOL[i];
		LOL[i] = LOL[best];
		LOL[best] = temp;
	}

	SortByHp(LOL, i + 1, count);
}
int main()
{
	legueoflegends LOL[172];
	int count = 0;

	ifstream file("testdata.txt");

	if (!file)
	{
		cout << "파일오류" << endl;
	}


	string line;
	while (getline(file, line))
	{
		if (line.empty()) continue;
		string words[100];
		int wordCount = 0;
		string cur = "";
		for (int i = 0; i <= (int)line.size(); ++i)
		{
			if (i == (int)line.size() || line[i] == ' ' || line[i] == '\t')
			{
				if (!cur.empty())
				{
					words[wordCount++] = cur;
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
		while (i < wordCount && !isdigit(words[i][0]))
		{
			if (i > 0)LOL[count].name += " ";
			LOL[count].name += words[i];
			i++;


		}
		LOL[count].hp = stoi(words[i++]);
		LOL[count].mp = stoi(words[i++]);
		LOL[count].speed = stoi(words[i++]);
		LOL[count].range = stoi(words[i++]);
		LOL[count].position = words[i];
		count++;

	}
	file.close();

	while (1)
	{
		cout << "Search [챔피언이름] : 주어진 챔피언의 정보를 출력한다." << endl;
		cout << "Insert : 새로운 챔피언의 정보를 입력받아 삽입한다." << endl;
		cout << "Delete[챔피언이름] : 주어진 챔피언에 대한 정보를 삭제한다." << endl;
		cout << "DeleteAll[position] : 주어진 위치의 모든 챔피언 정보를 삭제한다." << endl;
		cout << "PrintAll : 모든 챔피언의 정보를 배열에 저장된 순서대로 출력한다." << endl;
		cout << "FindMaxHp : 가장 체력이 큰 챔피언의 정보를 출력한다." << endl;
		cout << "SortByHp : 체력이 큰 챔피언부터 순서대로 저장한다." << endl;

		cout << "명령어를 입력하세요.";
		string command;
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
				cout << "찾지못함" << endl;
			}
			else
			{
				cout << LOL[i].name << " "
					<< LOL[i].hp << " "
					<< LOL[i].mp << " "
					<< LOL[i].speed << " "
					<< LOL[i].range << " "
					<< LOL[i].position << endl;
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
				cout << "찾지못함" << endl;
			}
			else
			{
				cout << "삭제함" << endl;
			}

		}
		else if (command == "DeleteAll")
		{
			string pos;
			cin >> pos;

			int erased = DeleteAll(LOL, 0, count, pos);
			if (erased == 0)
			{
				cout << "찾지못함" << endl;
			}
			else
			{
				cout << "삭제함" << endl;
			}
		}
		else if (command == "FindMaxHp")
		{
			int first = FirstAlive(LOL, 0, count);
			if (first == -1) {
				cout << "데이터없음" << endl;
			}
			else
			{
				int maxhp = FindMaxHP(LOL, first + 1, count, LOL[first].hp);
				PrintMaxHp(LOL, 0, count, maxhp);

			}
		}
		else if (command == "SortByHp")
		{
			SortByHp(LOL, 0, count);
			cout << "정렬완료" << endl;
		}
		else
		{
			cout << "정상적인 명령어가 아니빈다." << endl;
		}
	}
}