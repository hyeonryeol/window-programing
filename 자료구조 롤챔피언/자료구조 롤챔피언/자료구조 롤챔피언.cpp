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
		cout << "Search [챔피언이름] : 주어진 챔피언의 정보를 출력한다."<< endl
			<< "Insert : 새로운 챔피언의 정보를 입력받아 삽입한다." << endl
			<< "Delete[챔피언이름] : 주어진 챔피언에 대한 정보를 삭제한다." << endl
			<< "DeleteAll[position] : 주어진 위치의 모든 챔피언 정보를 삭제한다." << endl
			<< "PrintAll : 모든 챔피언의 정보를 배열에 저장된 순서대로 출력한다." << endl
			<< "FindMaxHp : 가장 체력이 큰 챔피언의 정보를 출력한다." << endl
		    << "SortByHp : 체력이 큰 챔피언부터 순서대로 저장한다 : " << endl;
		string command;
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
			
		}
		else if (command == "Delete")
		{
			string champ;
			cout << "삭제할 챔피언 이름을 입력하세요." << endl;
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
						count--;
					}
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
	}
}


