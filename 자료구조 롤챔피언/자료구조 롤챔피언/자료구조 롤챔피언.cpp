#include <iostream>
#include <string>
#include <fstream>

using namespace std;

struct legueoflegendes
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
	legueoflegendes LOL[172];
	int count = 0;

	ifstream file("testdata.txt");  // 파일 열기

	if (!file.is_open())        // 파일 열기 실패 확인
	{
		cout << "파일 열기 실패" << endl;

	}


	while (file >> LOL[count].name
		>> LOL[count].hp
		>> LOL[count].mp
		>> LOL[count].speed
		>> LOL[count].range
		>> LOL[count].positon) // 한 줄씩 읽기
	{
		++count;
	}

	file.close();               // 파일 닫기
	for (int i = 0; i < count; ++i)
	{
		cout << LOL[i].name << " "
			<< LOL[i].hp << " "
			<< LOL[i].mp << " "
			<< LOL[i].speed << " "
			<< LOL[i].range << " "
			<< LOL[i].positon << endl;
	}


	while (1)
	{
		cout << "숫자를 입력하시오.(1.search 2.Insert 3.delete ):" << endl;
		int command;
		cin >> command;
		switch (command)
		{
		case 1:
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
					break;
				}
			}
			if (!found)
			{
				cout << "그런 챔피언은 존재하지않습니다." << endl;
			}
			break;
		}
		case 2:
		{
			cout << "새로운 챔피언의 이름을 입력하세요." << endl;
			cin >> LOL[count].name;
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
			break;
		}
		case 3:
		{
			string champ;
			cout << "삭제할 챔피언 이름을 입력하세요." << endl;
			cin >> champ;

			int index = -1;

			for (int i = 0; i < count;++i)
			{
				if (LOL[i].name == champ)
				{
					index = i;
					break;
				}
			}
			if (index == -1)
			{
				cout << "그런 챔피언은 없습니다."<<endl;
				break;
			}
			for (int i = index; i < count - 1; ++i)
			{
				LOL[i] = LOL[i + 1];
			}
			count--;
			break;
		}
		}
	}

}