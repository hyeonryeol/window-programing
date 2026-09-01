#pragma once
int board_2020182001[19][19];

//흰돌		1
//검은돌	-1
#define BLACK -1
#define WHITE 1

void WhiteAttack_2020182001(int* x, int* y)
{
	// 직접 구현하신 돌 놓기 로직을 넣어주세요
	//
	//

	// 예시로 항상 [5,5]에만 돌을 두게함
	*x = 5;
	*y = 5;

	board_2020182001[*y][*x] = WHITE;
}
void WhiteDefence_2020182001(int x, int y)
{
	// 상대가 놓은 돌의 위치를 전달받아서 저장합니다
	board_2020182001[y][x] = BLACK;
}
void BlackAttack_2020182001(int* x, int* y)
{
	// 직접 구현하신 돌 놓기 로직을 넣어주세요
	//
	//

	// 예시로 항상 [5,5]에만 돌을 두게함
	*x = 5;
	*y = 5;

	board_2020182001[*y][*x] = BLACK;
}
void BlackDefence_2020182001(int x, int y)
{
	// 상대가 놓은 돌의 위치를 전달받아서 저장합니다
	board_2020182001[y][x] = WHITE;
}