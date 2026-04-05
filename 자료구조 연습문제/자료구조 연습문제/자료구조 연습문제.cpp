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

};
void PrintAll(legueoflegends LOL[], int i, int count)
{
	if (i == count) return;
	cout << LOL[i].name << " ";
	cout << LOL[i].hp << " ";
	cout << LOL[i].mp << " ";
	cout << LOL[i].speed << " ";
	cout << LOL[i].range << " ";
	cout << LOL[i].position << endl;

	PrintAll(LOL, i + 1, count);
}
int Search(legueoflegends LOL[], int i, int count, const string& champ)
{
	if (i == count) return -1;
	if (LOL[i].name == champ) return i;
	return Search(LOL, i + 1, count, champ);
}
int FindMaxHp(legueoflegends LOL[], int i, int count, int maxhp)
{
	if (i == count) return maxhp;
	if (LOL[i].hp > maxhp) maxhp = LOL[i].hp;
	return FindMaxHp(LOL, i + 1, count, maxhp);
}
void PrintAllWithHp(legueoflegends LOL[], int i, int count, int targetHp)
{
	if (i == count) return;
	if (LOL[i].hp == targetHp)
	{
		cout << LOL[i].name << " "
			<< LOL[i].hp << " "
			<< LOL[i].mp << " "
			<< LOL[i].speed << " "
			<< LOL[i].range << " "
			<< LOL[i].position << '\n';
	}
	PrintAllWithHp(LOL, i + 1, count, targetHp);
}
void ShiftLeft(legueoflegends LOL[], int idx, int count)
{
	if (idx >= count - 1) return;
	LOL[idx] = LOL[idx + 1];
	ShiftLeft(LOL, idx + 1, count);
}

int DeleteOneByName(legueoflegends LOL[], int i, int count, const string& champ)
{
	if (i == count) return count; // 못 찾음

	if (LOL[i].name == champ)
	{
		ShiftLeft(LOL, i, count);
		return count - 1; // 1명 삭제
	}
	return DeleteOneByName(LOL, i + 1, count, champ);
}

int DeleteAllByPosition(legueoflegends LOL[], int i, int count, const string& pos)
{
	if (i == count) return count;

	if (LOL[i].position == pos)
	{
		ShiftLeft(LOL, i, count);
		return DeleteAllByPosition(LOL, i, count - 1, pos); // i 그대로
	}
	return DeleteAllByPosition(LOL, i + 1, count, pos);
}
void ReadChampionRec(legueoflegends& c, int step)
{
	if (step == 0) {
		cout << "새로운 챔피언의 이름을 입력하세요." << endl;
		getline(cin, c.name);
		ReadChampionRec(c, 1);
	}
	else if (step == 1) {
		cout << "hp를 입력하세요." << endl;
		cin >> c.hp;
		ReadChampionRec(c, 2);
	}
	else if (step == 2) {
		cout << "mp를 입력하세요." << endl;
		cin >> c.mp;
		ReadChampionRec(c, 3);
	}
	else if (step == 3) {
		cout << "speed를 입력하세요." << endl;
		cin >> c.speed;
		ReadChampionRec(c, 4);
	}
	else if (step == 4) {
		cout << "range를 입력하세요." << endl;
		cin >> c.range;
		ReadChampionRec(c, 5);
	}
	else if (step == 5) {
		cout << "position를 입력하세요." << endl;
		cin >> c.position;
	}
}
int FindMaxIndexFrom(legueoflegends LOL[], int j, int count, int best)
{
	if (j == count) return best;
	if (LOL[j].hp > LOL[best].hp) best = j;
	return FindMaxIndexFrom(LOL, j + 1, count, best);
}

void SortByHpRec(legueoflegends LOL[], int i, int count)
{
	if (i >= count - 1) return;

	int best = FindMaxIndexFrom(LOL, i + 1, count, i);
	if (best != i) {
		legueoflegends temp = LOL[i];
		LOL[i] = LOL[best];
		LOL[best] = temp;
	}
	SortByHpRec(LOL, i + 1, count);
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
			if (i == -1) cout << "찾지못함" << endl;
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
			if (count >= 172) {
				cout << "배열이 가득 찼습니다." << endl;
				continue;
			}
			ReadChampionRec(LOL[count], 0);
			count++;
			cin.ignore(); // 다음 명령 입력 꼬임 방지
		}
		else if (command == "Delete")
		{
			string champ;
			getline(cin, champ);

			int newCount = DeleteOneByName(LOL, 0, count, champ);
			if (newCount == count) cout << "그런챔피언은 없습니다." << endl;
			else
			{
				count = newCount;
				cout << "삭제되었습니다." << endl;
			}
		}
		else if (command == "DeleteAll")
		{
			string pos;
			cin >> pos;

			int before = count;
			count = DeleteAllByPosition(LOL, 0, count, pos);
			cout << (before - count) << "명 삭제되었습니다." << endl;
		}
		else if (command == "FindMaxHp")
		{
			if (count == 0) { cout << "데이터없음"; continue; }

			int maxhp = FindMaxHp(LOL, 1, count, LOL[0].hp);
			PrintAllWithHp(LOL, 0, count, maxhp);  // 최대 체력인 애들 전부 출력

		}
		else if (command == "SortByHp")
		{
			SortByHpRec(LOL, 0, count);
			cout << "정렬완료" << endl;
		}
	}
}