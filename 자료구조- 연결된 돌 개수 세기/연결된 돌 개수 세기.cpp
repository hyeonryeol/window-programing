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
	cout << "2번 구현" << endl;

	// 1단계: 최대값 먼저 찾기
	int globalmax = 0;
	for (int i = 0; i < 19; ++i)
	{
		int wmulti = 0, bmulti = 0;
		for (int j = 0; j < 19; ++j)
		{
			if (board[i][j] == '0') { wmulti++; bmulti = 0; }
			else if (board[i][j] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }
			if (wmulti > globalmax) globalmax = wmulti;
			if (bmulti > globalmax) globalmax = bmulti;
		}
	}

	// 2단계: 최대값과 같은 연속 구간 모두 출력
	cout << "가로 최대 연속 (" << globalmax << "개) 위치:" << endl;
	if (globalmax == 0) { cout << "없음" << endl; }
	else
	{
		for (int i = 0; i < 19; ++i)
		{
			int wmulti = 0, bmulti = 0;
			int wstart = 0, bstart = 0;
			for (int j = 0; j < 19; ++j)
			{
				if (board[i][j] == '0')
				{
					if (wmulti == 0) wstart = j;
					wmulti++; bmulti = 0;
					if (wmulti == globalmax)
					{
						cout << "흰 돌 " << i << "번째 가로줄 ";
						for (int k = 0; k < globalmax; ++k)
							cout << "(" << i << "," << wstart + k << ") ";
						cout << endl;
					}
				}
				else if (board[i][j] == 'X')
				{
					if (bmulti == 0) bstart = j;
					bmulti++; wmulti = 0;
					if (bmulti == globalmax)
					{
						cout << "검은 돌 " << i << "번째 가로줄 ";
						for (int k = 0; k < globalmax; ++k)
							cout << "(" << bstart + k << "," << i << ") ";
						cout << endl;
					}
				}
				else { wmulti = 0; bmulti = 0; }
			}
		}
	}


	for (int i = 0; i < 19; ++i)
	{
		int wmulti = 0, wmax = 0;
		int bmulti = 0, bmax = 0;

		for (int j = 0; j < 19; ++j)
		{
			if (board[j][i] == '0') { wmulti++; bmulti = 0; }
			else if (board[j][i] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }

			if (wmulti > wmax) wmax = wmulti;
			if (bmulti > bmax) bmax = bmulti;
		}

		cout << i << "번째 세로줄: ";
		if (wmax == 0 && bmax == 0) cout << "없음" << endl;
		else if (wmax >= bmax) cout << "흰 돌 " << wmax << "개" << endl;
		else cout << "검은 돌 " << bmax << "개" << endl;
	}
}
void crossline()
{
	// ↘ 방향 대각선
	for (int start = 0; start < 19; ++start)
	{
		int wmulti = 0, wmax = 0;
		int bmulti = 0, bmax = 0;

		// 맨 위 행에서 시작 (시작점: [0][start])
		for (int k = 0; start + k < 19 && k < 19; ++k)
		{
			if (board[k][start + k] == '0') { wmulti++; bmulti = 0; }
			else if (board[k][start + k] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }

			if (wmulti > wmax) wmax = wmulti;
			if (bmulti > bmax) bmax = bmulti;
		}
		cout << start << "번째 대각선: ";
		if (wmax == 0 && bmax == 0) cout << "없음" << endl;
		else if (wmax >= bmax) cout << "흰 돌 " << wmax << "개" << endl;
		else cout << "검은 돌 " << bmax << "개" << endl;
	}

	// 맨 왼쪽 열에서 시작 (시작점: [start][0]), start=0 제외(중복)
	for (int start = 1; start < 19; ++start)
	{
		int wmulti = 0, wmax = 0;
		int bmulti = 0, bmax = 0;

		for (int k = 0; start + k < 19 && k < 19; ++k)
		{
			if (board[start + k][k] == '0') { wmulti++; bmulti = 0; }
			else if (board[start + k][k] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }

			if (wmulti > wmax) wmax = wmulti;
			if (bmulti > bmax) bmax = bmulti;
		}
		cout << start << "번째 대각선: ";
		if (wmax == 0 && bmax == 0) cout << "없음" << endl;
		else if (wmax >= bmax) cout << "흰 돌 " << wmax << "개" << endl;
		else cout << "검은 돌 " << bmax << "개" << endl;
	}
	// ↗ 방향 대각선 (맨 아래 행에서 시작)
	for (int start = 0; start < 19; ++start)
	{
		int wmulti = 0, wmax = 0;
		int bmulti = 0, bmax = 0;

		// 시작점: [18][start], k번째 칸 → board[18-k][start+k]
		for (int k = 0; 18 - k >= 0 && start + k < 19; ++k)
		{
			if (board[18 - k][start + k] == '0') { wmulti++; bmulti = 0; }
			else if (board[18 - k][start + k] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }

			if (wmulti > wmax) wmax = wmulti;
			if (bmulti > bmax) bmax = bmulti;
		}
		cout << start << "번째 ↗대각선: ";
		if (wmax == 0 && bmax == 0) cout << "없음" << endl;
		else if (wmax >= bmax) cout << "흰 돌 " << wmax << "개" << endl;
		else cout << "검은 돌 " << bmax << "개" << endl;
	}

	// 맨 왼쪽 열에서 시작 (시작점: [start][0]), start=18 제외(중복)
	for (int start = 17; start >= 0; --start)
	{
		int wmulti = 0, wmax = 0;
		int bmulti = 0, bmax = 0;

		// 시작점: [start][0], k번째 칸 → board[start-k][k]
		for (int k = 0; start - k >= 0 && k < 19; ++k)
		{
			if (board[start - k][k] == '0') { wmulti++; bmulti = 0; }
			else if (board[start - k][k] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }

			if (wmulti > wmax) wmax = wmulti;
			if (bmulti > bmax) bmax = bmulti;
		}
		cout << start << "번째 ↗대각선(왼쪽열): ";
		if (wmax == 0 && bmax == 0) cout << "없음" << endl;
		else if (wmax >= bmax) cout << "흰 돌 " << wmax << "개" << endl;
		else cout << "검은 돌 " << bmax << "개" << endl;
	}
}
int main()
{
	inboard();
	printboard();
	while (1)
	{
		if (turn == false)
		{
			cout << "흰 돌을 놓을 위치를 입력하세요.:"; //(0,0)부터임
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
						cout << "흰 돌의 개수: " << wstonecount << " ";
						cout << "검은돌의 개수 " << bstonecount << endl;
						//
						for (int i = 0; i < 19; ++i)
						{
							garowboard[i] = 0;
						}
						for (int i = 0; i < 19; ++i)
						{
							for (int j = 0; j < 19; ++j)
								if (board[i][j] == '0')
								{
									garowboard[i]++;

								}
						}
						for (int i = 0; i < 19; ++i)
						{
							cout << i << "번째 가로줄 흰 돌:" << garowboard[i] << endl;
						}
						//
						for (int i = 0; i < 19; ++i)
						{
							serowboard[i] = 0;
						}
						for (int i = 0; i < 19; ++i)
						{
							for (int j = 0; j < 19; ++j)
								if (board[j][i] == '0')
								{
									serowboard[i]++;

								}
						}
						for (int i = 0; i < 19; ++i)
						{
							cout << i << "번째 세로줄 흰 돌:" << serowboard[i] << endl;

						}
						printmulti();
						crossline();
						turn = true;
					}
				}
				else
				{
					cout << "정상적인 입력이 아닙니다." << endl;
				}
			}

		}
		else if (turn == true)
		{
			cout << "검은 돌을 놓을 위치를 입력하세요.:";
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
						cout << "흰 돌의 개수: " << wstonecount << " ";
						cout << "검은돌의 개수: " << bstonecount << endl;
						//
						for (int i = 0; i < 19; ++i)
						{
							garobboard[i] = 0;
						}
						for (int i = 0; i < 19; ++i)
						{
							for (int j = 0; j < 19; ++j)
								if (board[i][j] == 'X')
								{
									garobboard[i]++;

								}
						}
						for (int i = 0; i < 19; ++i)
						{
							cout << i << "번째 가로줄 검은 돌:" << garobboard[i] << endl;
						}
						//
						for (int i = 0; i < 19; ++i)
						{
							serobboard[i] = 0;
						}
						for (int i = 0; i < 19; ++i)
						{
							for (int j = 0; j < 19; ++j)
								if (board[j][i] == 'X')
								{
									serobboard[i]++;

								}
						}
						for (int i = 0; i < 19; ++i)
						{
							cout << i << "번째 세로줄 검은 돌:" << serobboard[i] << endl;

						}
						printmulti();
						crossline();
						turn = false;
					}
				}
				else
				{
					cout << "정상적인 입력이 아닙니다." << endl;
				}
			}

		}

	}

}