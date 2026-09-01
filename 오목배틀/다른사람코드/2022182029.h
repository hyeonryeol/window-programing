#pragma once
int board_2022182029[19][19];

#define BLACK -1
#define WHITE 1

struct Move_2022182029 {
	int x_2022182029;
	int y_2022182029;
	int score_2022182029;
};

int dx_2022182029[4] = { 1, 0, 1, 1 };
int dy_2022182029[4] = { 0, 1, 1, -1 };

int CountStone_2022182029(int x, int y, int dx, int dy, int color)
{
	int cnt_2022182029 = 0;

	while (x >= 0 && x < 19 && y >= 0 && y < 19 && board_2022182029[y][x] == color)
	{
		++cnt_2022182029;
		x += dx;
		y += dy;
	}

	return cnt_2022182029;
}

int Evaluate_2022182029(int x, int y, int color)
{
    int score = 0;

    for (int dir = 0; dir < 4; ++dir) {
        int cnt1 = CountStone_2022182029(
            x + dx_2022182029[dir],
            y + dy_2022182029[dir],
            dx_2022182029[dir],
            dy_2022182029[dir],
            color);

        int cnt2 = CountStone_2022182029(
            x - dx_2022182029[dir],
            y - dy_2022182029[dir],
            -dx_2022182029[dir],
            -dy_2022182029[dir],
            color);

        int total = cnt1 + cnt2 + 1;

        bool open1 = false;
        bool open2 = false;

        int x1 = x + (cnt1 + 1) * dx_2022182029[dir];
        int y1 = y + (cnt1 + 1) * dy_2022182029[dir];

        int x2 = x - (cnt2 + 1) * dx_2022182029[dir];
        int y2 = y - (cnt2 + 1) * dy_2022182029[dir];

        if (x1 >= 0 && x1 < 19 &&
            y1 >= 0 && y1 < 19 &&
            board_2022182029[y1][x1] == 0)
            open1 = true;

        if (x2 >= 0 && x2 < 19 &&
            y2 >= 0 && y2 < 19 &&
            board_2022182029[y2][x2] == 0)
            open2 = true;

        int open = open1 + open2;

        if (total >= 5)
            return 1000000000;

        else if (total == 4) {
            if (open == 2)
                score += 10000000;
            else if (open == 1)
                score += 1000000;
        }

        else if (total == 3){
            if (open == 2)
                score += 100000;
            else if (open == 1)
                score += 10000;
        }

        else if (total == 2){
            if (open == 2)
                score += 1000;
            else if (open == 1)
                score += 100;
        }
    }
    return score;
}

bool NearStone_2022182029(int x, int y)
{
    for (int dy = -2; dy <= 2; dy++)
    {
        for (int dx = -2; dx <= 2; dx++)
        {
            int nx = x + dx;
            int ny = y + dy;

            if (nx < 0 || nx >= 19 || ny < 0 || ny >= 19)
                continue;

            if (board_2022182029[ny][nx] != 0)
                return true;
        }
    }

    return false;
}

void WhiteAttack_2022182029(int* x, int* y) {
    bool empty = true;
    for (int r = 0; r < 19; r++)
    {
        for (int c = 0; c < 19; c++)
        {
            if (board_2022182029[r][c] != 0)
                empty = false;
        }
    }

    if (empty)
    {
        *x = 9;
        *y = 9;
        board_2022182029[*y][*x] = WHITE;
        return;
    }
    int bestScore = -1;
    *x = 9;
    *y = 9;
    for (int r = 0; r < 19; ++r)
    {
        for (int c = 0; c < 19; ++c)
        {

            if (board_2022182029[r][c] != 0) continue;
            if (!NearStone_2022182029(c, r)) continue;

            board_2022182029[r][c] = WHITE;

            if (Evaluate_2022182029(c, r, WHITE) >= 1000000000)
            {
                *x = c;
                *y = r;
                board_2022182029[r][c] = WHITE;
                return;
            }

            board_2022182029[r][c] = BLACK;
            if (Evaluate_2022182029(c, r, BLACK) >= 1000000000)
            {
                board_2022182029[r][c] = WHITE;
                *x = c;
                *y = r;
                return;
            }

            board_2022182029[r][c] = WHITE;

            int score =
                Evaluate_2022182029(c, r, WHITE) * 3
                + Evaluate_2022182029(c, r, BLACK);

            board_2022182029[r][c] = 0;

            if (score > bestScore)
            {
                bestScore = score;
                *x = c;
                *y = r;
            }
            else if (score == bestScore)
            {
                if (rand() % 2)
                {
                    *x = c;
                    *y = r;
                }
            }
        }
    }


	board_2022182029[*y][*x] = WHITE;
}

void WhiteDefence_2022182029(int x, int y) {
	board_2022182029[y][x] = BLACK;
}

void BlackAttack_2022182029(int* x, int* y) {
    bool empty = true;
    for (int r = 0; r < 19; r++)
    {
        for (int c = 0; c < 19; c++)
        {
            if (board_2022182029[r][c] != 0)
                empty = false;
        }
    }

    if (empty)
    {
        *x = 9;
        *y = 9;
        board_2022182029[*y][*x] = BLACK;
        return;
    }
    int bestScore = -1;
    *x = 9;
    *y = 9;
    for (int r = 0; r < 19; ++r)
    {
        for (int c = 0; c < 19; ++c)
        {
            if (board_2022182029[r][c] != 0) continue;
            if (!NearStone_2022182029(c, r)) continue;

            board_2022182029[r][c] = BLACK;

            if (Evaluate_2022182029(c, r, BLACK) >= 1000000000)
            {
                *x = c;
                *y = r;
                board_2022182029[r][c] = BLACK;
                return;
            }

            board_2022182029[r][c] = WHITE;
            if (Evaluate_2022182029(c, r, WHITE) >= 1000000000)
            {
                board_2022182029[r][c] = BLACK;
                *x = c;
                *y = r;
                return;
            }

            board_2022182029[r][c] = BLACK;

            int score =
                Evaluate_2022182029(c, r, BLACK) * 3
                + Evaluate_2022182029(c, r, WHITE);

            board_2022182029[r][c] = 0;

            if (score > bestScore)
            {
                bestScore = score;
                *x = c;
                *y = r;
            }
            else if (score == bestScore)
            {
                if (rand() % 2)
                {
                    *x = c;
                    *y = r;
                }
            }
        }
    }

	board_2022182029[*y][*x] = BLACK;
}

void BlackDefence_2022182029(int x, int y) {
	board_2022182029[y][x] = WHITE;
}