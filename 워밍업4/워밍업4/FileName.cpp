#include <iostream>
#include <windows.h>
#include <conio.h>
#include <cstdlib>
#include <ctime>
using namespace std;

// 상수 
const int BoardSIZE = 20;   // 보드 크기
const int ITEM_COUNT = 10;  // 아이템 개수

// 칸 종류
const int EMPTY = 0;
const int OBSTACLE = 1;
const int ITEM = 2;
const int STONE1 = 3; // #
const int STONE2 = 4; // @

// 전역 변수
int board[BoardSIZE][BoardSIZE];
int stone1r, stone1c; // 돌1 위치
int stone2r, stone2c; // 돌2 위치
int score1, score2;   // 각 돌의 아이템 획득 수
int itemsLeft;        // 남은 아이템 수
int turn;             // 0=돌1 차례, 1=돌2 차례

// 장애물 크기 3개
int obsH[3] = { 3, 5, 4 }; // 높이
int obsW[3] = { 4, 2, 4 }; // 너비

//  색상 출력 
HANDLE hConsole;
void setColor(int color) { SetConsoleTextAttribute(hConsole, color); }

// 보드 출력
void printBoard()
{
    system("cls");
    cout << "  ";
    for (int j = 0; j < BoardSIZE; j++) cout << (j % 10) << " ";
    cout << endl;

    for (int i = 0; i < BoardSIZE; i++)
    {
        cout << (i % 10) << " ";
        for (int j = 0; j < BoardSIZE; j++)
        {
            if (board[i][j] == STONE1)
            {
                setColor(12); // 밝은 빨강
                cout << "# ";
            }
            else if (board[i][j] == STONE2)
            {
                setColor(11); // 밝은 하늘
                cout << "@ ";
            }
            else if (board[i][j] == ITEM)
            {
                setColor(14); // 노랑
                cout << "* ";
            }
            else if (board[i][j] == OBSTACLE)
            {
                setColor(8); // 회색
                cout << "■ ";
            }
            else
            {
                setColor(7); // 흰색
                cout << ". ";
            }
        }
        cout << endl;
    }
    setColor(7);

    cout << endl;
    cout << "# score: " << score1 << "  @ score: " << score2;
    cout << "  items left: " << itemsLeft << endl;
    if (turn == 0) cout << "  [ # 차례 ] wasd 이동 / r 리셋 / q 종료" << endl;
    else           cout << "  [ @ 차례 ] ijkl 이동 / r 리셋 / q 종료" << endl;
}

// 범위 안에 장애물 놓을 수 있는지 확인
bool canPlaceObs(int r, int c, int h, int w)
{
    if (r + h > BoardSIZE || c + w > BoardSIZE) return false;
    for (int i = r; i < r + h; i++)
        for (int j = c; j < c + w; j++)
            if (board[i][j] != EMPTY) return false;
    return true;
}

// 장애물 배치
void placeObstacles()
{
    for (int k = 0; k < 3; k++)
    {
        int r, c;
        do {
            r = rand() % BoardSIZE;
            c = rand() % BoardSIZE;
        } while (!canPlaceObs(r, c, obsH[k], obsW[k]));

        for (int i = r; i < r + obsH[k]; i++)
            for (int j = c; j < c + obsW[k]; j++)
                board[i][j] = OBSTACLE;
    }
}

// 아이템 배치
void placeItems()
{
    int placed = 0;
    while (placed < ITEM_COUNT)
    {
        int r = rand() % BoardSIZE;
        int c = rand() % BoardSIZE;
        if (board[r][c] == EMPTY)
        {
            board[r][c] = ITEM;
            placed++;
        }
    }
    itemsLeft = ITEM_COUNT;
}

//돌 초기 배치
void placeStone(int& r, int& c, int type)
{
    do {
        r = rand() % BoardSIZE;
        c = rand() % BoardSIZE;
    } while (board[r][c] != EMPTY);
    board[r][c] = type;
}

// 게임 초기화
void init()
{
    // 보드 초기화
    for (int i = 0; i < BoardSIZE; i++)
        for (int j = 0; j < BoardSIZE; j++)
            board[i][j] = EMPTY;

    score1 = 0;
    score2 = 0;
    turn = 0;

    placeObstacles();
    placeItems();
    placeStone(stone1r, stone1c, STONE1);
    placeStone(stone2r, stone2c, STONE2);
}

// 종료 출력
void printResult()
{
    setColor(7);
    cout << "\n=== 게임 종료 ===" << endl;
    cout << "# 아이템: " << score1 << "개" << endl;
    cout << "@ 아이템: " << score2 << "개" << endl;
    if (score1 > score2) cout << "# 승리!" << endl;
    else if (score2 > score1) cout << "@ 승리!" << endl;
    else                      cout << "무승부!" << endl;
}

// 돌 이동
// dr,dc: 방향 / r,c: 현재 위치 / score: 해당 돌 점수 / type: STONE1 or STONE2
bool moveStone(int dr, int dc, int& r, int& c, int& score, int type)
{
    int nr = (r + dr + BoardSIZE) % BoardSIZE; 
    int nc = (c + dc + BoardSIZE) % BoardSIZE;

    if (board[nr][nc] == OBSTACLE) return false; // 장애물 통과 불가
    if (board[nr][nc] == STONE1 || board[nr][nc] == STONE2) return false; // 다른 돌 위치

    board[r][c] = EMPTY; // 원래 자리 비우기

    if (board[nr][nc] == ITEM)
    {
        score++;
        itemsLeft--;
        board[nr][nc] = OBSTACLE; // 아이템 먹으면 장애물로 변환
    }

    r = nr;
    c = nc;
    board[r][c] = type;
    return true;
}

// 메인 
int main()
{
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    srand(time(NULL));
    init();
    printBoard();

    while (true)
    {
        char cmd = _getch();

        // 리셋
        if (cmd == 'r')
        {
            init();
            printBoard();
            continue;
        }

        // 종료
        if (cmd == 'q')
        {
            printResult();
            return 0;
        }

        bool moved = false;

        if (turn == 0) // 돌1 차례: wasd
        {
            if (cmd == 'w') moved = moveStone(-1, 0, stone1r, stone1c, score1, STONE1);
            else if (cmd == 's') moved = moveStone(1, 0, stone1r, stone1c, score1, STONE1);
            else if (cmd == 'a') moved = moveStone(0, -1, stone1r, stone1c, score1, STONE1);
            else if (cmd == 'd') moved = moveStone(0, 1, stone1r, stone1c, score1, STONE1);
            else continue; // 유효하지 않은 키면 차례 안 넘김

            if (moved) turn = 1; // 이동 성공 시 차례 교체
        }
        else // 돌2 차례: ijkl
        {
            if (cmd == 'i') moved = moveStone(-1, 0, stone2r, stone2c, score2, STONE2);
            else if (cmd == 'k') moved = moveStone(1, 0, stone2r, stone2c, score2, STONE2);
            else if (cmd == 'j') moved = moveStone(0, -1, stone2r, stone2c, score2, STONE2);
            else if (cmd == 'l') moved = moveStone(0, 1, stone2r, stone2c, score2, STONE2);
            else continue;

            if (moved) turn = 0;
        }

        printBoard();

        // 아이템 모두 소진 시 종료
        if (itemsLeft == 0)
        {
            printResult();
            return 0;
        }
    }
}