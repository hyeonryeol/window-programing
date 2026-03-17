#include <iostream>

using namespace std;

char board[19][19];
bool turn = false;
int wstonecount = 0;
int bstonecount = 0;

void inboard()
{
	for (int i = 0; i < 19; ++i)
	{
		for (int j = 0; j < 19; ++j)
		{
			board[i][j] = '+';
		}

	}
}
void printboard()
{
	for (int i = 0; i < 19; ++i)
	{
		for (int j = 0; j < 19; ++j)
		{
			cout << board[i][j] << " ";
		}
		cout << endl;
	}
}
void position(int garo, int sero, char stone)
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
			cout << "흰 돌의 좌표를 입력하세요. 좌상귀(0,0)우하귀(18,18):";
			int x;
			int y;
			cin >> x;
			cin >> y;
			if (cin.fail())
			{
				cout << "정상적인 입력이 아닙니다." << endl;

				cin.clear();
				cin.ignore(1000, '\n');
			}
			else
			{
				if (x < 19 && y < 19)
				{
					if (board[y][x] != '+')
					{
						cout << "이미 돌이 놓여있습니다." << endl;
					}
					else
					{
						position(y, x, '0');
						printboard();
						wstonecount++;
						cout << "흰돌의 개수:" << wstonecount << " ";
						cout << "검은돌의 개수:" << bstonecount << endl;
						turn = true;
					}
				}
				else
				{
					cout << "숫자가 너무 큽니다." << endl;
				}

			}


		}
		else if (turn == true)
		{
			cout << "검은 돌의 좌표를 입력하세요. 좌상귀(0,0)우하귀(18,18):";
			int x;
			int y;
			cin >> x;
			cin >> y;
			if (cin.fail())
			{
				cout << "정상적인 입력이 아닙니다." << endl;

				cin.clear();
				cin.ignore(1000, '\n');
			}
			else
			{
				if (x < 19 && y < 19)
				{
					if (board[y][x] != '+')
					{
						cout << "이미 돌이 놓여있습니다." << endl;
					}
					else
					{
						position(y, x, 'X');
						printboard();
						bstonecount++;
						cout << "흰돌의 개수:" << wstonecount << " ";
						cout << "검은돌의 개수:" << bstonecount << endl;
						turn = false;
					}
				}
				else
				{
					cout << "숫자가 너무 큽니다." << endl;
				}
			}



		}

	}


}