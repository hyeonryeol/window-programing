#include <iostream>
#include <algorithm>

using namespace std;

char board[19][19];//보드 배열

bool turn = false;
void inboard()//보드 입력함수(기본 바둑판)
{
	for (int i = 0; i < 19;++i)
	{
		for (int j = 0; j < 19; ++j)
		{
			board[i][j] = '+';
		}
		cout << endl;
	}
}

void printboard() //보드 출력함수
{
	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			cout << board[i][j] << " ";
		}
		cout << endl;
	}
}
void position(int garo, int sero, char stone) //돌 위치 저장
{
	board[garo][sero] = stone;
}



int main()
{
	inboard();
	printboard();
	while (1)
	{
		if (turn == false)
		{
			int x, y;
			cout << "흰 돌을 놓을 곳을 입력하세요.(가로, 세로):";
			cin >> x;
			cin >> y;
			int a = x - 1; //배열이라 마이너스 해주기
			int b = y - 1;
			
			if (board[b][a] != '+')
			{
				cout << "이미 돌이 놓여있습니다. 다른 곳을 입력하세요.";
			}
			else
			{
				position(b, a, 'O');
				printboard();
				turn = true;
			}
			
			
		}
		if (turn == true)
		{
			int x, y;
			cout << "검은 돌을 놓을 곳을 입력하세요.(가로, 세로):";
			cin >> x;
			cin >> y;
			int a = x - 1; //배열이라 마이너스 해주기
			int b = y - 1;

			if (board[b][a] != '+')
			{
				cout << "이미 돌이 놓여있습니다. 다른 곳을 입력하세요.";
			}
			else
			{
				position(b, a, 'X');
				printboard();
				turn = false;
			}
		}
	}
}