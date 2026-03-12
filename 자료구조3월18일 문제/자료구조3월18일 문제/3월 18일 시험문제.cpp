#include <iostream>

using namespace std;

char board[19][19];//보드 배열

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


int main()
{
		inboard();
		printboard();
	while (1)
	{
		cout << "흰 돌을 놓을 곳을 입력하세요.(행, 열):";
		int x, y;
		cin >> x;
		cin >> y;
		int a = x - 1; //배열이라 마이너스 해주기
		int b = y - 1;
		board[a][b] = 'O';
		printboard();
	}
}