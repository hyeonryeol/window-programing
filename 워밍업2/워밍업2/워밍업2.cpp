#include <iostream>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <conio.h>

using namespace std;

int main()
{
	int board[10][10];
	bool Blue[10] = { false }; // 0~9 파랑 선택 상태 (전부 false로 초기화
	int sum = 0;
	bool anyBlue = false;
	srand(time(NULL)); //난수 초기화

	for (int i = 0; i < 10; ++i)
	{

		int arr[10];
		for (int j = 0; j < 10; ++j)
		{
			arr[j] = j;
		}
		for (int j = 9; j > 0; j--)
		{
			int r = rand() % (j + 1); // 0부터 j까지 중 랜덤 인덱스 선택 

			int temp = arr[j]; //바꿔주는 친구
			arr[j] = arr[r];
			arr[r] = temp;
		}

		for (int j = 0; j < 10; ++j)
		{
			board[i][j] = arr[j];
		}

	}

	int cursorRow = rand() % 10;
	int cursorCol = rand() % 10;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //커서가 가리키는
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			if (i == cursorRow && j == cursorCol)
				SetConsoleTextAttribute(hConsole, 4); // 빨강
			else
				SetConsoleTextAttribute(hConsole, 7);  // 흰색(기본)

			cout << board[i][j] << " ";
		}

		cout << endl;
	}

	while (true)
	{

		char command;
		cout << "command:" << endl;
		command = _getch();
		switch (command)
		{


		case 75: //왼쪽 방향키
		{
			int next = (cursorCol - 1 + 10) % 10; //  다음 위치 미리 계산
			
				// 열 교환
				for (int i = 0; i < 10; i++)
				{
					int temp = board[i][cursorCol];
					board[i][cursorCol] = board[i][next];
					board[i][next] = temp;
				}
				cursorCol = next;


				// 보드 출력
				for (int i = 0; i < 10; ++i)
				{
					for (int j = 0; j < 10; ++j)
					{
						if (i == cursorRow && j == cursorCol)
							SetConsoleTextAttribute(hConsole, 4); // 빨강
						else if (Blue[board[i][j]])
							SetConsoleTextAttribute(hConsole, 9); // 파랑
						else
							SetConsoleTextAttribute(hConsole, 7); // 흰색
						cout << board[i][j] << " ";
					}
					cout << endl;
				}
			


			for (int i = 0; i < 10; ++i)
			{
				int tep = board[i][0];
				board[i][0] = board[i][9];
				board[i][9] = tep;
			}


			break;

		}
		case 77: //오른쪽 방향키
		{
			int next = (cursorCol + 1) % 10; //  다음 위치 미리 계산

			// 열 교환
			for (int i = 0; i < 10; i++)
			{
				int temp = board[i][cursorCol];
				board[i][cursorCol] = board[i][next];
				board[i][next] = temp;
			}
			cursorCol = next;  //  9면 0으로


			// 보드 출력
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < 10; ++j)
				{
					if (i == cursorRow && j == cursorCol)
						SetConsoleTextAttribute(hConsole, 4); // 빨강
					else if (Blue[board[i][j]])
						SetConsoleTextAttribute(hConsole, 9); // 파랑
					else
						SetConsoleTextAttribute(hConsole, 7); // 흰색
					cout << board[i][j] << " ";
				}
				cout << endl;
			}



			for (int i = 0; i < 10; ++i)
			{
				int temp = board[i][9];
				board[i][9] = board[i][0];
				board[i][0] = temp;

			}

			break;

		}
		case 72: //위쪽 방향키
		{
			int next = (cursorRow - 1 + 10) % 10; //  다음 위치 미리 계산
			// 열 교환
			for (int i = 0; i < 10; i++)
			{
				int temp = board[cursorRow][i];
				board[cursorRow][i] = board[next][i];
				board[next][i] = temp;
			}
			cursorRow = next;  // 9면 0으로

			// 보드 출력
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < 10; ++j)
				{
					if (i == cursorRow && j == cursorCol)
						SetConsoleTextAttribute(hConsole, 4); // 빨강
					else if (Blue[board[i][j]])
						SetConsoleTextAttribute(hConsole, 9); // 파랑
					else
						SetConsoleTextAttribute(hConsole, 7); // 흰색
					cout << board[i][j] << " ";
				}
				cout << endl;
			}


			for (int i = 0; i < 10; ++i)
			{
				int tep = board[0][i];
				board[0][i] = board[9][i];
				board[9][i] = tep;

			}

			break;

		}
		case 80: //아래쪽 방향키
		{
			int next = (cursorRow + 1) % 10;
			// 열 교환
			for (int i = 0; i < 10; i++)
			{
				int temp = board[cursorRow][i];
				board[cursorRow][i] = board[next][i];
				board[next][i] = temp;
			}
			cursorRow = next;  //  9면 0으로

			// 보드 출력
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < 10; ++j)
				{
					if (i == cursorRow && j == cursorCol)
						SetConsoleTextAttribute(hConsole, 4); // 빨강
					else if (Blue[board[i][j]])
						SetConsoleTextAttribute(hConsole, 9); // 파랑
					else
						SetConsoleTextAttribute(hConsole, 7); // 흰색
					cout << board[i][j] << " ";
				}
				cout << endl;
			}

			{
				for (int i = 0; i < 10; ++i)
				{
					int tep = board[9][i];
					board[9][i] = board[0][i];
					board[0][i] = tep;

				}
			}
			break;

		}

		case 'w':
		{
			if (cursorRow > 0)

				cursorRow--;
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < 10; ++j)
				{
					if (i == cursorRow && j == cursorCol)
						SetConsoleTextAttribute(hConsole, 4); // 빨강
					else
						SetConsoleTextAttribute(hConsole, 7);  // 흰색(기본)

					cout << board[i][j] << " ";
				}

				cout << endl;
			}
			break;
		}
		case 'a':
		{
			if (cursorCol > 0)
				cursorCol--;
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < 10; ++j)
				{
					if (i == cursorRow && j == cursorCol)
						SetConsoleTextAttribute(hConsole, 4); // 빨강
					else
						SetConsoleTextAttribute(hConsole, 7);  // 흰색(기본)

					cout << board[i][j] << " ";
				}

				cout << endl;
			}
			break;
		}
		case 's':
		{
			if (cursorRow < 10)
				cursorRow++;
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < 10; ++j)
				{
					if (i == cursorRow && j == cursorCol)
						SetConsoleTextAttribute(hConsole, 4); // 빨강
					else
						SetConsoleTextAttribute(hConsole, 7);  // 흰색(기본)

					cout << board[i][j] << " ";
				}

				cout << endl;
			}
			break;

		}
		case 'd':
		{
			if (cursorCol < 10)
				cursorCol++;
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < 10; ++j)
				{
					if (i == cursorRow && j == cursorCol)
						SetConsoleTextAttribute(hConsole, 4); // 빨강
					else
						SetConsoleTextAttribute(hConsole, 7);  // 흰색(기본)

					cout << board[i][j] << " ";
				}

				cout << endl;
			}
			break;
		}
		case 13:
		{
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < 10; ++j)
				{
					if (i == cursorRow && j == cursorCol)
						continue;  // 커서 위치 건너뜀
					else if (Blue[board[i][j]])           // 이 칸의 숫자가 파랑이면
					{
						sum += board[i][j];          // 더하기
						anyBlue = true;
					}
				}
			}
			sum += board[cursorRow][cursorCol];  // 커서 위치는 여기서 따로 더하기		

		}
		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 10; ++j)
			{
				if (i == cursorRow && j == cursorCol)
				{
					SetConsoleTextAttribute(hConsole, 4); // 빨강
					if (anyBlue)
						cout << sum << " ";        //합계 출력
					else
						cout << board[i][j] << " "; // 파랑 없으면 원래 숫자
				}
				else if (Blue[board[i][j]])
				{
					SetConsoleTextAttribute(hConsole, 9); // 파랑
					cout << board[i][j] << " ";
				}
				else
				{
					SetConsoleTextAttribute(hConsole, 7); // 흰색
					cout << board[i][j] << " ";
				}
			}
			cout << endl;
		}

		break;


		case 'q':
		{
			return 0;
		}
		case 'r':
		{
			srand(time(NULL)); //난수 초기화

			for (int i = 0; i < 10; ++i)
			{

				int arr[10];
				for (int j = 0; j < 10; ++j)
				{
					arr[j] = j;
				}
				for (int j = 9; j > 0; j--)
				{
					int r = rand() % (j + 1); // 0부터 j까지 중 랜덤 인덱스 선택 

					int temp = arr[j]; //바꿔주는 친구
					arr[j] = arr[r];
					arr[r] = temp;
				}

				for (int j = 0; j < 10; ++j)
				{
					board[i][j] = arr[j];
				}

			}

			int cursorRow = rand() % 10;
			int cursorCol = rand() % 10;

			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //커서가 가리키는
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < 10; ++j)
				{
					if (i == cursorRow && j == cursorCol)
						SetConsoleTextAttribute(hConsole, 4); // 빨강
					else
						SetConsoleTextAttribute(hConsole, 7);  // 흰색(기본)

					cout << board[i][j] << " ";
				}

				cout << endl;
			}

		}
		default:
			if (command >= '0' && command <= '9')
			{
				int num = command - '0'; //문자를 숫자로 바꿔주는 

				sum = 0;
				anyBlue = 0;

				for (int i = 0; i < 10; i++)
				{
					Blue[i] = false; //파랑을 해제
				}
				Blue[num] = true; //누른 숫자를 파랑으로
				for (int i = 0; i < 10; ++i)
				{
					for (int j = 0; j < 10; ++j)
					{
						if (i == cursorRow && j == cursorCol)
							SetConsoleTextAttribute(hConsole, 4); // 빨강
						else if (Blue[board[i][j]])
						{
							SetConsoleTextAttribute(hConsole, 9); // 파랑

						}
						else
							SetConsoleTextAttribute(hConsole, 7);  // 흰색(기본)



						cout << board[i][j] << " ";
					}

					cout << endl;
				}
			}
			break;
		}

	}
}



