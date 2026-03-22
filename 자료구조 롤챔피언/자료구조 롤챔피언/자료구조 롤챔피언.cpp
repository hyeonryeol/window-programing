#include <iostream>
#include <string>
#include <fstream>

using namespace std;

struct legueoflegendes //구조체
{
	string name; //이름
	int hp; //체력
	int mp; //마나
	int speed; //이동속도
	int range; //사거리
	string positon;
};

int main()
{
	legueoflegendes LOL[172]; //롤 챔피언수만큼 배열 선언함
	int count = 0; //파일의 챔피언 수

	ifstream file("testdata.txt");  // 파일 열기

	if (!file.is_open())        // 파일 열기 실패 확인
	{
		cout << "파일 열기 실패" << endl;

	}


	string line;
	while (getline(file, line))
	{
		if (line.empty()) continue;

		// 단어로 쪼개기
		string words[100];
		int wordCount = 0;
		string cur = "";
		for (int i = 0; i <= (int)line.size(); i++)
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

		// 숫자 나오기 전까지 이름
		LOL[count].name = "";
		int i = 0;
		while (i < wordCount && !isdigit(words[i][0]))
		{
			if (i > 0) LOL[count].name += " ";
			LOL[count].name += words[i];
			i++;
		}

		// 나머지 순서대로
		LOL[count].hp = stoi(words[i++]);
		LOL[count].mp = stoi(words[i++]);
		LOL[count].speed = stoi(words[i++]);
		LOL[count].range = stoi(words[i++]);
		LOL[count].positon = words[i];
		count++;
	}

	file.close();               // 파일 닫기
	


	while (1)
	{
		cout << "Search 챔피언이름 : 주어진 챔피언의 정보를 출력한다."<< endl
			<< "Insert : 새로운 챔피언의 정보를 입력받아 삽입한다." << endl
			<< "Delete 챔피언이름 : 주어진 챔피언에 대한 정보를 삭제한다." << endl
			<< "DeleteAll 포지션 : 주어진 위치의 모든 챔피언 정보를 삭제한다." << endl
			<< "PrintAll : 모든 챔피언의 정보를 배열에 저장된 순서대로 출력한다." << endl
			<< "FindMaxHp : 가장 체력이 큰 챔피언의 정보를 출력한다." << endl
		    << "SortByHp : 체력이 큰 챔피언부터 순서대로 저장한다 : " << endl;
		string command;
		cout << "명령어를 입력하세요." << endl;
		cin >> command;
		if (command == "Search")
		{

			bool found = false;
			string champ;
			cin >> champ;
			for (int i = 0; i < count; ++i)
			{
				if (LOL[i].name == champ)
				{
					cout << LOL[i].name << " "
						<< LOL[i].hp << " "
						<< LOL[i].mp << " "
						<< LOL[i].speed << " "
						<< LOL[i].range << " "
						<< LOL[i].positon << endl;

					found = true;
					
				}
			}
			if (!found)
			{
				cout << "그런 챔피언은 존재하지않습니다." << endl;
			}
			
		}
		else if (command == "Insert")
		{
			cin.ignore();

			cout << "새로운 챔피언의 이름을 입력하세요." << endl;
			getline(cin, LOL[count].name);
			cout << "새로운 챔피언의 체력을 입력하세요." << endl;
			cin >> LOL[count].hp;
			cout << "새로운 챔피언의 마나를 입력하세요." << endl;
			cin >> LOL[count].mp;
			cout << "새로운 챔피언의 이동속도를 입력하세요." << endl;
			cin >> LOL[count].speed;
			cout << "새로운 챔피언의 사거리를 입력하세요." << endl;
			cin >> LOL[count].range;
			cout << "새로운 챔피언의 포지션을 입력하세요." << endl;
			cin >> LOL[count].positon;
			count++;
			
		}
		else if (command == "Delete")
		{
			string champ;
			
			cin >> champ;

			int index = -1;

			for (int i = 0; i < count; ++i)
			{
				if (LOL[i].name == champ)
				{
					index = i;
					cout << "삭제되었습니다"<<endl;
				}
			}
			if (index == -1)
			{
				cout << "그런 챔피언은 없습니다." << endl;
				
			}
			for (int i = index; i < count - 1; ++i)
			{
				LOL[i] = LOL[i + 1];
			}
			count--;
			
		}
		else if (command == "DeleteAll")
		{
			string pos;
			cin >> pos;

			int index = -1;
			for (int i = 0; i < count; ++i)
			{
				if (LOL[i].positon == pos)
				{
					for (int j = i; j < count - 1; ++j)
					{

						{
							LOL[j] = LOL[j + 1];
						}
					}
						count--;
						i--;
				}
				
			}
			cout << "삭제되었습니다" << endl;
		}
		else if (command == "PrintAll")
		{
			for (int i = 0; i < count; ++i)
			{
				cout << LOL[i].name << " "
					<< LOL[i].hp << " "
					<< LOL[i].mp << " "
					<< LOL[i].speed << " "
					<< LOL[i].range << " "
					<< LOL[i].positon << endl << endl;
			}
		}
		else if (command == "FindMaxHp")
		{
			int maxhp = LOL[0].hp; 
			for (int i = 1; i < count; ++i)
			{
				
				if (LOL[i].hp > maxhp)
				{
					maxhp = LOL[i].hp;
				}


				
			}
			for (int i = 0; i < count; i++)
			{
				if (maxhp == LOL[i].hp)
				{
					cout << LOL[i].name << " "
						<< LOL[i].hp << " "
						<< LOL[i].mp << " "
						<< LOL[i].speed << " "
						<< LOL[i].range << " "
						<< LOL[i].positon << endl;
				}
			}
			
		}
		else if (command == "SortByHp")
		{
			for (int i = 0; i < count - 1; ++i)
			{
				for (int j = i + 1; j < count; ++j)
				{
					if (LOL[i].hp < LOL[j].hp) // hp 기준 내림차순
					{
						legueoflegendes temp = LOL[i];
						LOL[i] = LOL[j];
						LOL[j] = temp;
					}
				}
			}
		}
	}
}


