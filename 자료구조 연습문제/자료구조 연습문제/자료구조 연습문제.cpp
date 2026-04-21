#include <iostream>

using namespace std;

char board[19][19];
bool turn = false;
int wstonecount = 0;
int bstonecount = 0;
int garowboard[19];
int serowboard[19];
int garobboard[19];
int serobboard[19];


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
void printmulti()
{
	
	int globalmax = 0;
	
	for (int i = 0; i < 19; ++i)
	{
		int wmulti = 0, bmulti = 0;
		for (int j = 0; j < 19; ++j)
		{
			if (board[i][j] == '0') { wmulti++; bmulti = 0; }
			else if (board[i][j] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }
			if (wmulti > globalmax) { globalmax = wmulti; }
			if (bmulti > globalmax) { globalmax = bmulti; }
		}
	}
	cout << "가로최대 연속(" << globalmax << "개) 위치:" << endl;
	if (globalmax == 0) { cout << "없음" << endl; }
	else
	{
		for (int i = 0; i < 19; ++i)
		{
			int wmulti = 0, wstart = 0;
			int bmulti = 0, bstart = 0;
			for (int j = 0; j < 19; ++j)
			{
				if (board[i][j] == '0')
				{
					if (wmulti == 0) wstart = j;
					wmulti++; bmulti= 0;
					if (wmulti == globalmax)
					{
						cout << "흰 돌" << i << "번째 가로줄";
						for (int k = 0; k < globalmax; ++k)
							cout << "(" << wstart + k << "," << i << ")";
						cout << endl;
					}

				}
				else if (board[i][j] == 'X')
				{
					if (bmulti == 0) bstart = j;
					bmulti++; wmulti = 0;
					if (bmulti == globalmax)
					{
						cout << "검은 돌" << i << "번째 가로줄";
						for (int k = 0; k < globalmax; ++k)
							cout << "(" << bstart + k << "," << i << ")";
						cout << endl;
					}
				}
				else { wmulti = 0; bmulti = 0; }
			}
		}
	}
	for (int i = 0; i < 19;++i)
	{
		int wmulti = 0, wmax = 0;
		int bmulti = 0, bmax = 0;
		for (int j = 0; j < 19; ++j)
		{
			if (board[i][j] == '0') { wmulti++; bmulti = 0; }
			else if (board[i][j] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }
			if (wmulti > wmax) { wmax = wmulti; }
			if (bmulti > bmax) { bmax = bmulti; }
			
		}
		cout << i << "번째 가로줄";
		if (wmax == 0 && bmax == 0)cout << "없음" << endl;
		else if (wmax >= bmax) { cout << "흰 돌:" << wmax << endl; }
		else cout << "검은 돌:" << bmax << endl;
	}
	for (int i = 0; i < 19;++i)
	{
		int wmulti = 0, wmax = 0;
		int bmulti = 0, bmax = 0;
		for (int j = 0; j < 19; ++j)
		{
			if (board[j][i] == '0') { wmulti++; bmulti = 0; }
			else if (board[j][i] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }
			if (wmulti > wmax) { wmax = wmulti; }
			if (bmulti > bmax) { bmax = bmulti; }

		}
		cout << i << "번째 가로줄";
		if (wmax == 0 && bmax == 0)cout << "없음" << endl;
		else if (wmax >= bmax) { cout << "흰 돌:" << wmax << endl; }
		else cout << "검은 돌:" << bmax << endl;
	}
}
void crossline()
{
	for (int start = 0; start < 19;++start)
	{
		int wmulti = 0, wmax = 0;
		int bmulti = 0, bmax = 0;
		for (int k = 0; k < 19 && start + k < 19; ++k)
		{
			if (board[k][start + k] == '0') { wmulti++; bmulti = 0; }
			else if (board[k][start + k] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }
			if (wmulti > wmax) { wmax = wmulti; }
			if (bmulti > bmax) { bmax = bmulti; }

		}
		cout << "우하향 시작(" << start << ",0)";
		if (wmax == 0 && bmax == 0)cout << "없음" << endl;
		else if (wmax >= bmax) { cout << "흰 돌:" << wmax << endl; }
		else cout << "검은 돌:" << bmax << endl;
	}
	for (int start = 1; start < 19;++start)
	{
		int wmulti = 0, wmax = 0;
		int bmulti = 0, bmax = 0;
		for (int k = 0; k < 19 && start + k < 19; ++k)
		{
			if (board[start + k][k] == '0') { wmulti++; bmulti = 0; }
			else if (board[start + k][k] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }
			if (wmulti > wmax) { wmax = wmulti; }
			if (bmulti > bmax) { bmax = bmulti; }

		}
		cout  << "우하향 시작(0," << start << ")";
		if (wmax == 0 && bmax == 0)cout << "없음" << endl;
		else if (wmax >= bmax) { cout << "흰 돌:" << wmax << endl; }
		else cout << "검은 돌:" << bmax << endl;
	}
	for (int start = 0; start < 19;++start)
	{
		int wmulti = 0, wmax = 0;
		int bmulti = 0, bmax = 0;
		for (int k = 0; 18 - k >= 0 && start + k < 19; ++k)
		{
			if (board[18-k][start + k] == '0') { wmulti++; bmulti = 0; }
			else if (board[18 - k][start + k] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }
			if (wmulti > wmax) { wmax = wmulti; }
			if (bmulti > bmax) { bmax = bmulti; }

		}
		cout << "우상향 시작(" << start << ",18)";
		if (wmax == 0 && bmax == 0)cout << "없음" << endl;
		else if (wmax >= bmax) { cout << "흰 돌:" << wmax << endl; }
		else cout << "검은 돌:" << bmax << endl;
	}
	for (int start = 17; start >= 0;--start)
	{
		int wmulti = 0, wmax = 0;
		int bmulti = 0, bmax = 0;
		for (int k = 0; k < 19 && start - k >= 0; ++k)
		{
			if (board[start - k][k] == '0') { wmulti++; bmulti = 0; }
			else if (board[start - k][k] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }
			if (wmulti > wmax) { wmax = wmulti; }
			if (bmulti > bmax) { bmax = bmulti; }

		}
		cout << "우상향 시작(0," << start << ")";
		if (wmax == 0 && bmax == 0)cout << "없음" << endl;
		else if (wmax >= bmax) { cout << "흰 돌:" << wmax << endl; }
		else cout << "검은 돌:" << bmax << endl;
	}
}
int main()
{
	inboard();
	
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
						for (int i = 0; i < 19; ++i)
						{
							garowboard[i] = 0;
						}
						for (int i = 0; i < 19; ++i)
						{
							for (int j = 0; j < 19; ++j)
							{
								if (board[i][j] == '0')
								{
									garowboard[i]++;
								}
							}
						}
						for (int i = 0; i < 19; ++i)
						{
							cout << i << "번째 가로줄:" << garowboard[i] << endl;
						}
						for (int i = 0; i < 19; ++i)
						{
							serowboard[i] = 0;
						}
						for (int i = 0; i < 19; ++i)
						{
							for (int j = 0; j < 19; ++j)
							{
								if (board[j][i] == '0')
								{
									serowboard[i]++;
								}
							}
						}
						for (int i = 0; i < 19; ++i)
						{
							cout << i << "번째 세로줄:" << serowboard[i] << endl;
						}
						printmulti();
						crossline();
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
						for (int i = 0; i < 19; ++i)
						{
							garobboard[i] = 0;
						}
						for (int i = 0; i < 19; ++i)
						{
							for (int j = 0; j < 19; ++j)
							{
								if (board[i][j] == 'X')
								{
									garobboard[i]++;
								}
							}
						}
						for (int i = 0; i < 19; ++i)
						{
							cout << i << "번째 가로줄:" << garobboard[i] << endl;
						}
						for (int i = 0; i < 19; ++i)
						{
							serobboard[i] = 0;
						}
						for (int i = 0; i < 19; ++i)
						{
							for (int j = 0; j < 19; ++j)
							{
								if (board[j][i] == 'X')
								{
									serobboard[i]++;
								}
							}
						}
						for (int i = 0; i < 19; ++i)
						{
							cout << i << "번째 세로줄:" << serobboard[i] << endl;
						}
						printmulti();
						crossline();
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