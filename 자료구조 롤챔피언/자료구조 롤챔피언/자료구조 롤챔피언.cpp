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
			for (int i = 0; i < count; i++)
			{
				cout << LOL[i].name << " ";
				cout << LOL[i].hp << " ";
				cout << LOL[i].mp << " ";
				cout << LOL[i].speed << " ";
				cout << LOL[i].range << " ";
				cout << LOL[i].position << endl;

			}
		}
		else if (command == "Search")
		{
			bool found = false;
			string champ;
			getline(cin, champ);
			for (int i = 0; i < count; i++)
			{
				if (LOL[i].name == champ)
				{
					cout << LOL[i].name << " ";
					cout << LOL[i].hp << " ";
					cout << LOL[i].mp << " ";
					cout << LOL[i].speed << " ";
					cout << LOL[i].range << " ";
					cout << LOL[i].position << endl;
					found = true;
				}

			}
			if (!found)
			{
				cout << "찾지 못했습니다." << endl;
			}
		}
		else if (command == "Insert")
		{
			cout << "새로운 챔피언의 이름을 입력하세요." << endl;
			getline(cin, LOL[count].name);
			cout << "hp를 입력하세요." << endl;
			cin >> LOL[count].hp;
			cout << "mp를 입력하세요." << endl;
			cin >> LOL[count].mp;
			cout << "speed를 입력하세요." << endl;
			cin >> LOL[count].speed;
			cout << "range를 입력하세요." << endl;
			cin >> LOL[count].range;
			cout << "position를 입력하세요." << endl;
			cin >> LOL[count].position;
			count++;
		}
		else if (command == "Delete")
		{
			string champ;
			getline(cin, champ);
			int index = -1;
			for (int i = 0; i < count; ++i)
			{
				if (champ == LOL[i].name)
				{
					index = i;
					cout << "삭제되었습니다." << endl;

					for (int j = index; j < count - 1; ++j)
					{
						LOL[j] = LOL[j + 1];
					}
					count--;
				}

			}
			if (index == -1)
			{
				cout << "그런챔피언은 없습니다." << endl;
			}

		}
		else if (command == "DeleteAll")
		{
			string pos;
			cin >> pos;

			for (int i = 0; i < count; ++i)
			{
				if (LOL[i].position == pos)
				{

					cout << "삭제되었습니다." << endl;

					for (int j = i; j < count - 1; ++j)
					{
						LOL[j] = LOL[j + 1];
					}
					count--;
					i--;
				}
			}
		}
		else if (command == "FindMaxHp")
		{
			int maxhp = LOL[0].hp;
			for (int i = 0; i < count; ++i)
			{
				if (LOL[i].hp > maxhp)
				{
					maxhp = LOL[i].hp;
				}
			}
			for (int i = 0; i < count; ++i)
			{
				if (LOL[i].hp == maxhp)
				{
					cout << LOL[i].name << " ";
					cout << LOL[i].hp << " ";
					cout << LOL[i].mp << " ";
					cout << LOL[i].speed << " ";
					cout << LOL[i].range << " ";
					cout << LOL[i].position << endl;
				}
			}
		}
		else if (command == "SortByHp")
		{
			for (int i = 0; i < count - 1; ++i)
			{
				for (int j = i + 1; j < count; ++j)
				{
					if (LOL[i].hp < LOL[j].hp)
					{
						legueoflegends temp = LOL[i];
						LOL[i] = LOL[j];
						LOL[j] = temp;
					}
				}
			}
		}
	}
}