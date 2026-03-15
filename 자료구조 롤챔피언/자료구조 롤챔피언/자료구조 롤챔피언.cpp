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

	cout << "command를 입력하시오.:" << endl;
	int command;
	cin >> command;
	switch (command)
	{
	case 1:
	{
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
			}

		}
	}
	}
}