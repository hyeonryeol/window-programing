#pragma once

#include <algorithm>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

const int SIZE_2023184014 = 19;
const int EMPTY_2023184014 = 0;
const int BLACK_2023184014 = -1;
const int WHITE_2023184014 = 1;
const int INF_2023184014 = 1000000000;
const int TIME_LIMIT_MS_2023184014 = 450;

// 내부 보드: 0은 빈칸, -1은 흑, 1은 백.
// 심판 좌표가 x,y 순서라서 실제 저장은 board[y][x]로 한다.
int board_2023184014[SIZE_2023184014][SIZE_2023184014] = { 0 };

struct Move_2023184014 {
    int x_2023184014;
    int y_2023184014;
    int score_2023184014;
};

std::chrono::high_resolution_clock::time_point startTime_2023184014;
bool seededRandom_2023184014 = false;

// 4개 방향만 검사한다. 반대 방향은 countOneSide에서 -dx, -dy로 같이 본다.
int dx_2023184014[4] = { 1, 0, 1, 1 };
int dy_2023184014[4] = { 0, 1, 1, -1 };

bool inBoard_2023184014(int x_2023184014, int y_2023184014) {
    return 0 <= x_2023184014 && x_2023184014 < SIZE_2023184014 &&
        0 <= y_2023184014 && y_2023184014 < SIZE_2023184014;
}

int opponent_2023184014(int color_2023184014) {
    return -color_2023184014;
}

bool timeout_2023184014() {
    auto now_2023184014 = std::chrono::high_resolution_clock::now();
    long long ms_2023184014 =
        std::chrono::duration_cast<std::chrono::milliseconds>(now_2023184014 - startTime_2023184014).count();
    return ms_2023184014 >= TIME_LIMIT_MS_2023184014;
}

void seedRandom_2023184014() {
    if (!seededRandom_2023184014) {
        srand((unsigned int)time(0));
        seededRandom_2023184014 = true;
    }
}

int countOneSide_2023184014(int x_2023184014, int y_2023184014, int color_2023184014,
    int dirX_2023184014, int dirY_2023184014) {
    int count_2023184014 = 0;
    x_2023184014 += dirX_2023184014;
    y_2023184014 += dirY_2023184014;

    while (inBoard_2023184014(x_2023184014, y_2023184014) &&
        board_2023184014[y_2023184014][x_2023184014] == color_2023184014) {
        count_2023184014++;
        x_2023184014 += dirX_2023184014;
        y_2023184014 += dirY_2023184014;
    }

    return count_2023184014;
}

int lineCount_2023184014(int x_2023184014, int y_2023184014, int color_2023184014,
    int dir_2023184014) {
    return 1 +
        countOneSide_2023184014(x_2023184014, y_2023184014, color_2023184014,
            dx_2023184014[dir_2023184014], dy_2023184014[dir_2023184014]) +
        countOneSide_2023184014(x_2023184014, y_2023184014, color_2023184014,
            -dx_2023184014[dir_2023184014], -dy_2023184014[dir_2023184014]);
}

int openEnds_2023184014(int x_2023184014, int y_2023184014, int color_2023184014,
    int dir_2023184014) {
    int result_2023184014 = 0;

    int nx_2023184014 = x_2023184014 + dx_2023184014[dir_2023184014];
    int ny_2023184014 = y_2023184014 + dy_2023184014[dir_2023184014];
    while (inBoard_2023184014(nx_2023184014, ny_2023184014) &&
        board_2023184014[ny_2023184014][nx_2023184014] == color_2023184014) {
        nx_2023184014 += dx_2023184014[dir_2023184014];
        ny_2023184014 += dy_2023184014[dir_2023184014];
    }
    if (inBoard_2023184014(nx_2023184014, ny_2023184014) &&
        board_2023184014[ny_2023184014][nx_2023184014] == EMPTY_2023184014) {
        result_2023184014++;
    }

    nx_2023184014 = x_2023184014 - dx_2023184014[dir_2023184014];
    ny_2023184014 = y_2023184014 - dy_2023184014[dir_2023184014];
    while (inBoard_2023184014(nx_2023184014, ny_2023184014) &&
        board_2023184014[ny_2023184014][nx_2023184014] == color_2023184014) {
        nx_2023184014 -= dx_2023184014[dir_2023184014];
        ny_2023184014 -= dy_2023184014[dir_2023184014];
    }
    if (inBoard_2023184014(nx_2023184014, ny_2023184014) &&
        board_2023184014[ny_2023184014][nx_2023184014] == EMPTY_2023184014) {
        result_2023184014++;
    }

    return result_2023184014;
}

bool isWin_2023184014(int x_2023184014, int y_2023184014, int color_2023184014) {
    for (int dir_2023184014 = 0; dir_2023184014 < 4; dir_2023184014++) {
        int count_2023184014 = lineCount_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014);
        if (color_2023184014 == BLACK_2023184014 && count_2023184014 == 5) return true;
        if (color_2023184014 == WHITE_2023184014 && count_2023184014 >= 5) return true;
    }
    return false;
}

// 흑은 장목이 금수라서 6목 이상이면 금지 후보로 본다.
// 백은 장목도 승리로 인정되므로 이 함수는 흑 금수 판단에서만 중요하다.
bool isOverline_2023184014(int x_2023184014, int y_2023184014, int color_2023184014) {
    for (int dir_2023184014 = 0; dir_2023184014 < 4; dir_2023184014++) {
        if (lineCount_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014) >= 6) {
            return true;
        }
    }
    return false;
}

// pattern에서 0은 빈칸, 1은 현재 색 돌을 의미한다.
// 열린3(01110), 띈3(011010, 010110), 띈4(11101 등)를 찾기 위한 공통 함수다.
bool linePattern_2023184014(int x_2023184014, int y_2023184014, int color_2023184014,
    int dir_2023184014, const int pattern_2023184014[], int len_2023184014) {
    for (int offset_2023184014 = 0; offset_2023184014 < len_2023184014; offset_2023184014++) {
        int startX_2023184014 = x_2023184014 - dx_2023184014[dir_2023184014] * offset_2023184014;
        int startY_2023184014 = y_2023184014 - dy_2023184014[dir_2023184014] * offset_2023184014;
        bool ok_2023184014 = true;

        for (int i_2023184014 = 0; i_2023184014 < len_2023184014; i_2023184014++) {
            int nx_2023184014 = startX_2023184014 + dx_2023184014[dir_2023184014] * i_2023184014;
            int ny_2023184014 = startY_2023184014 + dy_2023184014[dir_2023184014] * i_2023184014;

            if (!inBoard_2023184014(nx_2023184014, ny_2023184014)) {
                ok_2023184014 = false;
                break;
            }

            int need_2023184014 = pattern_2023184014[i_2023184014];
            if (need_2023184014 == 0 &&
                board_2023184014[ny_2023184014][nx_2023184014] != EMPTY_2023184014) {
                ok_2023184014 = false;
                break;
            }
            if (need_2023184014 == 1 &&
                board_2023184014[ny_2023184014][nx_2023184014] != color_2023184014) {
                ok_2023184014 = false;
                break;
            }
        }

        if (ok_2023184014) return true;
    }

    return false;
}

bool hasOpenThreeDir_2023184014(int x_2023184014, int y_2023184014, int color_2023184014,
    int dir_2023184014) {
    int p1_2023184014[5] = { 0, 1, 1, 1, 0 };
    int p2_2023184014[6] = { 0, 1, 1, 0, 1, 0 };
    int p3_2023184014[6] = { 0, 1, 0, 1, 1, 0 };

    return linePattern_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014, p1_2023184014, 5) ||
        linePattern_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014, p2_2023184014, 6) ||
        linePattern_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014, p3_2023184014, 6);
}

bool hasFourDir_2023184014(int x_2023184014, int y_2023184014, int color_2023184014,
    int dir_2023184014) {
    int count_2023184014 = lineCount_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014);
    int open_2023184014 = openEnds_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014);

    if (count_2023184014 == 4 && open_2023184014 >= 1) return true;

    int p1_2023184014[5] = { 1, 1, 1, 0, 1 };
    int p2_2023184014[5] = { 1, 1, 0, 1, 1 };
    int p3_2023184014[5] = { 1, 0, 1, 1, 1 };

    return linePattern_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014, p1_2023184014, 5) ||
        linePattern_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014, p2_2023184014, 5) ||
        linePattern_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014, p3_2023184014, 5);
}

int countOpenThree_2023184014(int x_2023184014, int y_2023184014, int color_2023184014) {
    int count_2023184014 = 0;
    for (int dir_2023184014 = 0; dir_2023184014 < 4; dir_2023184014++) {
        if (hasOpenThreeDir_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014)) {
            count_2023184014++;
        }
    }
    return count_2023184014;
}

int countFour_2023184014(int x_2023184014, int y_2023184014, int color_2023184014) {
    int count_2023184014 = 0;
    for (int dir_2023184014 = 0; dir_2023184014 < 4; dir_2023184014++) {
        if (hasFourDir_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014)) {
            count_2023184014++;
        }
    }
    return count_2023184014;
}

bool isForbiddenBlack_2023184014(int x_2023184014, int y_2023184014) {
    if (!inBoard_2023184014(x_2023184014, y_2023184014)) return true;
    if (board_2023184014[y_2023184014][x_2023184014] != EMPTY_2023184014) return true;

    board_2023184014[y_2023184014][x_2023184014] = BLACK_2023184014;

    bool forbidden_2023184014 = false;
    if (isOverline_2023184014(x_2023184014, y_2023184014, BLACK_2023184014)) {
        forbidden_2023184014 = true;
    }
    if (!forbidden_2023184014 && countFour_2023184014(x_2023184014, y_2023184014, BLACK_2023184014) >= 2) {
        forbidden_2023184014 = true;
    }
    if (!forbidden_2023184014 && countOpenThree_2023184014(x_2023184014, y_2023184014, BLACK_2023184014) >= 2) {
        forbidden_2023184014 = true;
    }

    board_2023184014[y_2023184014][x_2023184014] = EMPTY_2023184014;
    return forbidden_2023184014;
}

bool canPlace_2023184014(int x_2023184014, int y_2023184014, int color_2023184014) {
    if (!inBoard_2023184014(x_2023184014, y_2023184014)) return false;
    if (board_2023184014[y_2023184014][x_2023184014] != EMPTY_2023184014) return false;
    if (color_2023184014 == BLACK_2023184014 && isForbiddenBlack_2023184014(x_2023184014, y_2023184014)) return false;
    return true;
}

bool hasAnyStone_2023184014() {
    for (int y_2023184014 = 0; y_2023184014 < SIZE_2023184014; y_2023184014++) {
        for (int x_2023184014 = 0; x_2023184014 < SIZE_2023184014; x_2023184014++) {
            if (board_2023184014[y_2023184014][x_2023184014] != EMPTY_2023184014) return true;
        }
    }
    return false;
}

int totalStoneCount_2023184014() {
    int count_2023184014 = 0;
    for (int y_2023184014 = 0; y_2023184014 < SIZE_2023184014; y_2023184014++) {
        for (int x_2023184014 = 0; x_2023184014 < SIZE_2023184014; x_2023184014++) {
            if (board_2023184014[y_2023184014][x_2023184014] != EMPTY_2023184014) {
                count_2023184014++;
            }
        }
    }
    return count_2023184014;
}

bool nearStone_2023184014(int x_2023184014, int y_2023184014) {
    for (int yy_2023184014 = y_2023184014 - 2; yy_2023184014 <= y_2023184014 + 2; yy_2023184014++) {
        for (int xx_2023184014 = x_2023184014 - 2; xx_2023184014 <= x_2023184014 + 2; xx_2023184014++) {
            if (inBoard_2023184014(xx_2023184014, yy_2023184014) &&
                board_2023184014[yy_2023184014][xx_2023184014] != EMPTY_2023184014) {
                return true;
            }
        }
    }
    return false;
}

bool secondBlackOpeningMove_2023184014(Move_2023184014& move_2023184014) {
    int blackCount_2023184014 = 0;
    int whiteCount_2023184014 = 0;
    int blackX_2023184014 = -1;
    int blackY_2023184014 = -1;
    int whiteX_2023184014 = -1;
    int whiteY_2023184014 = -1;

    for (int y_2023184014 = 0; y_2023184014 < SIZE_2023184014; y_2023184014++) {
        for (int x_2023184014 = 0; x_2023184014 < SIZE_2023184014; x_2023184014++) {
            if (board_2023184014[y_2023184014][x_2023184014] == BLACK_2023184014) {
                blackCount_2023184014++;
                blackX_2023184014 = x_2023184014;
                blackY_2023184014 = y_2023184014;
            }
            else if (board_2023184014[y_2023184014][x_2023184014] == WHITE_2023184014) {
                whiteCount_2023184014++;
                whiteX_2023184014 = x_2023184014;
                whiteY_2023184014 = y_2023184014;
            }
        }
    }

    if (blackCount_2023184014 != 1 || whiteCount_2023184014 != 1) return false;

    int diffX_2023184014 = whiteX_2023184014 - blackX_2023184014;
    int diffY_2023184014 = whiteY_2023184014 - blackY_2023184014;
    if (std::abs(diffX_2023184014) > 2 || std::abs(diffY_2023184014) > 2) return false;
    if (diffX_2023184014 == 0 && diffY_2023184014 == 0) return false;

    // 흑 첫 수가 중앙이면 두 번째 흑은 중앙 기준 위쪽 대각 자리로 고정한다.
    // 우선 (10,7), 안 되면 (8,7)을 둔다.
    if (blackX_2023184014 == 9 && blackY_2023184014 == 9) {
        if (canPlace_2023184014(10, 7, BLACK_2023184014)) {
            move_2023184014.x_2023184014 = 10;
            move_2023184014.y_2023184014 = 7;
            move_2023184014.score_2023184014 = 800000000;
            return true;
        }
        if (canPlace_2023184014(8, 7, BLACK_2023184014)) {
            move_2023184014.x_2023184014 = 8;
            move_2023184014.y_2023184014 = 7;
            move_2023184014.score_2023184014 = 800000000;
            return true;
        }
    }

    int candidateX_2023184014[8];
    int candidateY_2023184014[8];

    candidateX_2023184014[0] = whiteX_2023184014 + diffX_2023184014;
    candidateY_2023184014[0] = whiteY_2023184014 + diffY_2023184014;
    candidateX_2023184014[1] = whiteX_2023184014 - diffY_2023184014;
    candidateY_2023184014[1] = whiteY_2023184014 + diffX_2023184014;
    candidateX_2023184014[2] = whiteX_2023184014 + diffY_2023184014;
    candidateY_2023184014[2] = whiteY_2023184014 - diffX_2023184014;
    candidateX_2023184014[3] = whiteX_2023184014 - diffX_2023184014;
    candidateY_2023184014[3] = whiteY_2023184014 - diffY_2023184014;

    candidateX_2023184014[4] = whiteX_2023184014 + 1;
    candidateY_2023184014[4] = whiteY_2023184014 - 1;
    candidateX_2023184014[5] = whiteX_2023184014 - 1;
    candidateY_2023184014[5] = whiteY_2023184014 + 1;
    candidateX_2023184014[6] = whiteX_2023184014 + 1;
    candidateY_2023184014[6] = whiteY_2023184014 + 1;
    candidateX_2023184014[7] = whiteX_2023184014 - 1;
    candidateY_2023184014[7] = whiteY_2023184014 - 1;

    int targetX_2023184014 = -1;
    int targetY_2023184014 = -1;

    // 위 고정 오프닝이 안 되면 백 기준으로 첫 흑과 대칭 또는 주변 대각 후보를 고른다.
    for (int i_2023184014 = 0; i_2023184014 < 8; i_2023184014++) {
        if (canPlace_2023184014(candidateX_2023184014[i_2023184014],
            candidateY_2023184014[i_2023184014], BLACK_2023184014)) {
            targetX_2023184014 = candidateX_2023184014[i_2023184014];
            targetY_2023184014 = candidateY_2023184014[i_2023184014];
            break;
        }
    }

    if (targetX_2023184014 == -1) return false;

    move_2023184014.x_2023184014 = targetX_2023184014;
    move_2023184014.y_2023184014 = targetY_2023184014;
    move_2023184014.score_2023184014 = 800000000;
    return true;
}

int centerScore_2023184014(int x_2023184014, int y_2023184014) {
    return 20 - (std::abs(x_2023184014 - 9) + std::abs(y_2023184014 - 9));
}

int evaluatePointAfterPlace_2023184014(int x_2023184014, int y_2023184014, int color_2023184014) {
    int score_2023184014 = 0;

    // 가장 큰 가중치: 즉시 승리.
    // 이후 열린4, 막힌4, 열린3 순서로 공격 가치가 내려간다.
    if (isWin_2023184014(x_2023184014, y_2023184014, color_2023184014)) {
        score_2023184014 += 10000000;
    }

    for (int dir_2023184014 = 0; dir_2023184014 < 4; dir_2023184014++) {
        int count_2023184014 = lineCount_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014);
        int open_2023184014 = openEnds_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014);

        if (count_2023184014 >= 5) score_2023184014 += 1000000;
        else if (count_2023184014 == 4 && open_2023184014 == 2) score_2023184014 += 300000;
        else if (count_2023184014 == 4 && open_2023184014 == 1) score_2023184014 += 80000;
        else if (count_2023184014 == 3 && open_2023184014 == 2) score_2023184014 += 30000;
        else if (count_2023184014 == 3 && open_2023184014 == 1) score_2023184014 += 5000;
        else if (count_2023184014 == 2 && open_2023184014 == 2) score_2023184014 += 1500;
        else if (count_2023184014 == 2 && open_2023184014 == 1) score_2023184014 += 300;
    }

    int fourCount_2023184014 = countFour_2023184014(x_2023184014, y_2023184014, color_2023184014);
    int openThreeCount_2023184014 = countOpenThree_2023184014(x_2023184014, y_2023184014, color_2023184014);

    // 백은 금수가 없어서 공격형으로 둔다.
    // 그래서 백의 4, 열린3, 쌍3, 4+3 패턴은 흑보다 더 크게 본다.
    int colorAttackBonus_2023184014 = (color_2023184014 == WHITE_2023184014) ? 2 : 1;

    score_2023184014 += fourCount_2023184014 * 90000 * colorAttackBonus_2023184014;
    score_2023184014 += openThreeCount_2023184014 * 28000 * colorAttackBonus_2023184014;
    if (fourCount_2023184014 >= 2) score_2023184014 += 450000 * colorAttackBonus_2023184014;
    if (fourCount_2023184014 >= 1 && openThreeCount_2023184014 >= 1) score_2023184014 += 260000 * colorAttackBonus_2023184014;
    if (openThreeCount_2023184014 >= 2) score_2023184014 += 180000 * colorAttackBonus_2023184014;
    score_2023184014 += centerScore_2023184014(x_2023184014, y_2023184014);

    return score_2023184014;
}

int evaluateMove_2023184014(int x_2023184014, int y_2023184014, int color_2023184014) {
    if (!canPlace_2023184014(x_2023184014, y_2023184014, color_2023184014)) {
        return -INF_2023184014;
    }

    int enemy_2023184014 = opponent_2023184014(color_2023184014);

    board_2023184014[y_2023184014][x_2023184014] = color_2023184014;
    int attack_2023184014 = evaluatePointAfterPlace_2023184014(x_2023184014, y_2023184014, color_2023184014);
    board_2023184014[y_2023184014][x_2023184014] = EMPTY_2023184014;

    int defence_2023184014 = 0;
    if (canPlace_2023184014(x_2023184014, y_2023184014, enemy_2023184014)) {
        board_2023184014[y_2023184014][x_2023184014] = enemy_2023184014;
        defence_2023184014 = evaluatePointAfterPlace_2023184014(x_2023184014, y_2023184014, enemy_2023184014);
        board_2023184014[y_2023184014][x_2023184014] = EMPTY_2023184014;
    }

    // 수 하나의 기본 점수 = 내가 그 칸에 둘 때 공격 가치 + 상대가 그 칸에 둘 때 위협 가치.
    // 백은 전체적으로 공격을 더 높게, 흑은 금수와 방어를 고려해서 조금 덜 공격적으로 둔다.
    if (color_2023184014 == WHITE_2023184014 && totalStoneCount_2023184014() <= 10) {
        return attack_2023184014 * 20 / 10 + defence_2023184014 * 3 / 10;
    }

    if (color_2023184014 == WHITE_2023184014) {
        return attack_2023184014 * 17 / 10 + defence_2023184014 * 5 / 10;
    }

    return attack_2023184014 * 12 / 10 + defence_2023184014 * 7 / 10;
}

int urgentThreatScore_2023184014(int x_2023184014, int y_2023184014, int enemy_2023184014) {
    if (!canPlace_2023184014(x_2023184014, y_2023184014, enemy_2023184014)) {
        return 0;
    }

    board_2023184014[y_2023184014][x_2023184014] = enemy_2023184014;

    int score_2023184014 = 0;
    int fourCount_2023184014 = countFour_2023184014(x_2023184014, y_2023184014, enemy_2023184014);
    int openThreeCount_2023184014 = countOpenThree_2023184014(x_2023184014, y_2023184014, enemy_2023184014);

    // 긴급 방어 점수.
    // 열린4는 거의 패배 직전이라 가장 크게, 열린3은 다음 수 열린4가 되기 쉬워 크게 둔다.
    // 막힌3은 위협도가 낮아서 일부러 낮게 둔다.
    if (isWin_2023184014(x_2023184014, y_2023184014, enemy_2023184014)) {
        score_2023184014 += 100000000;
    }
    score_2023184014 += fourCount_2023184014 * 1000000;
    score_2023184014 += openThreeCount_2023184014 * 350000;

    for (int dir_2023184014 = 0; dir_2023184014 < 4; dir_2023184014++) {
        int count_2023184014 = lineCount_2023184014(x_2023184014, y_2023184014, enemy_2023184014, dir_2023184014);
        int open_2023184014 = openEnds_2023184014(x_2023184014, y_2023184014, enemy_2023184014, dir_2023184014);

        if (count_2023184014 == 4 && open_2023184014 == 2) {
            score_2023184014 += 6000000;
        }
        else if (count_2023184014 == 4 && open_2023184014 == 1) {
            score_2023184014 += 1200000;
        }
        else if (count_2023184014 == 3 && open_2023184014 == 2) {
            score_2023184014 += 900000;
        }
        else if (count_2023184014 == 3 && open_2023184014 == 1) {
            score_2023184014 += 40000;
        }
    }

    board_2023184014[y_2023184014][x_2023184014] = EMPTY_2023184014;
    return score_2023184014;
}

bool urgentDefenseMove_2023184014(int color_2023184014, Move_2023184014& best_2023184014) {
    int enemy_2023184014 = opponent_2023184014(color_2023184014);
    int bestThreat_2023184014 = 0;
    int bestMoveScore_2023184014 = -INF_2023184014;

    for (int y_2023184014 = 0; y_2023184014 < SIZE_2023184014; y_2023184014++) {
        for (int x_2023184014 = 0; x_2023184014 < SIZE_2023184014; x_2023184014++) {
            if (board_2023184014[y_2023184014][x_2023184014] != EMPTY_2023184014) continue;
            if (!nearStone_2023184014(x_2023184014, y_2023184014)) continue;
            if (!canPlace_2023184014(x_2023184014, y_2023184014, color_2023184014)) continue;

            int threat_2023184014 = urgentThreatScore_2023184014(x_2023184014, y_2023184014, enemy_2023184014);
            int moveScore_2023184014 = evaluateMove_2023184014(x_2023184014, y_2023184014, color_2023184014);

            if (threat_2023184014 > bestThreat_2023184014 ||
                (threat_2023184014 == bestThreat_2023184014 && moveScore_2023184014 > bestMoveScore_2023184014)) {
                bestThreat_2023184014 = threat_2023184014;
                bestMoveScore_2023184014 = moveScore_2023184014;
                best_2023184014.x_2023184014 = x_2023184014;
                best_2023184014.y_2023184014 = y_2023184014;
                best_2023184014.score_2023184014 = threat_2023184014 + moveScore_2023184014 / 100;
            }
        }
    }

    // 180000 이상이면 일반 공격보다 먼저 막을 가치가 있는 위협으로 본다.
    return bestThreat_2023184014 >= 180000;
}

bool aggressiveFourMove_2023184014(int color_2023184014, Move_2023184014& best_2023184014) {
    int bestScore_2023184014 = -INF_2023184014;

    for (int y_2023184014 = 0; y_2023184014 < SIZE_2023184014; y_2023184014++) {
        for (int x_2023184014 = 0; x_2023184014 < SIZE_2023184014; x_2023184014++) {
            if (!canPlace_2023184014(x_2023184014, y_2023184014, color_2023184014)) continue;
            if (!nearStone_2023184014(x_2023184014, y_2023184014)) continue;

            board_2023184014[y_2023184014][x_2023184014] = color_2023184014;

            bool makesFour_2023184014 = false;
            int attackScore_2023184014 = evaluatePointAfterPlace_2023184014(x_2023184014, y_2023184014, color_2023184014);

            for (int dir_2023184014 = 0; dir_2023184014 < 4; dir_2023184014++) {
                int count_2023184014 = lineCount_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014);
                int open_2023184014 = openEnds_2023184014(x_2023184014, y_2023184014, color_2023184014, dir_2023184014);
                if (count_2023184014 == 4 && open_2023184014 >= 1) {
                    makesFour_2023184014 = true;
                    if (open_2023184014 == 2) attackScore_2023184014 += 600000;
                    else attackScore_2023184014 += 260000;
                }
            }

            board_2023184014[y_2023184014][x_2023184014] = EMPTY_2023184014;

            if (makesFour_2023184014 && attackScore_2023184014 > bestScore_2023184014) {
                bestScore_2023184014 = attackScore_2023184014;
                best_2023184014.x_2023184014 = x_2023184014;
                best_2023184014.y_2023184014 = y_2023184014;
                best_2023184014.score_2023184014 = attackScore_2023184014;
            }
        }
    }

    // 내 열린3을 4로 키우는 수는 강제 방어를 만들 수 있어서 방어보다 먼저 둘 수 있다.
    return bestScore_2023184014 >= 260000;
}

bool earlyWhiteOpenThreeMove_2023184014(Move_2023184014& best_2023184014) {
    int bestScore_2023184014 = -INF_2023184014;

    for (int y_2023184014 = 0; y_2023184014 < SIZE_2023184014; y_2023184014++) {
        for (int x_2023184014 = 0; x_2023184014 < SIZE_2023184014; x_2023184014++) {
            if (!canPlace_2023184014(x_2023184014, y_2023184014, WHITE_2023184014)) continue;
            if (!nearStone_2023184014(x_2023184014, y_2023184014)) continue;

            board_2023184014[y_2023184014][x_2023184014] = WHITE_2023184014;

            int openThreeCount_2023184014 = countOpenThree_2023184014(x_2023184014, y_2023184014, WHITE_2023184014);
            int fourCount_2023184014 = countFour_2023184014(x_2023184014, y_2023184014, WHITE_2023184014);
            int attackScore_2023184014 = evaluatePointAfterPlace_2023184014(x_2023184014, y_2023184014, WHITE_2023184014);

            if (openThreeCount_2023184014 >= 1) {
                attackScore_2023184014 += openThreeCount_2023184014 * 420000;
            }
            if (fourCount_2023184014 >= 1) {
                attackScore_2023184014 += fourCount_2023184014 * 900000;
            }
            if (openThreeCount_2023184014 >= 2) {
                attackScore_2023184014 += 700000;
            }
            if (openThreeCount_2023184014 >= 1 && fourCount_2023184014 >= 1) {
                attackScore_2023184014 += 800000;
            }

            board_2023184014[y_2023184014][x_2023184014] = EMPTY_2023184014;

            if ((openThreeCount_2023184014 >= 1 || fourCount_2023184014 >= 1) &&
                attackScore_2023184014 > bestScore_2023184014) {
                bestScore_2023184014 = attackScore_2023184014;
                best_2023184014.x_2023184014 = x_2023184014;
                best_2023184014.y_2023184014 = y_2023184014;
                best_2023184014.score_2023184014 = attackScore_2023184014;
            }
        }
    }

    // 백은 금수가 없으므로 열린3, 띈3을 적극적으로 만든다.
    return bestScore_2023184014 >= 180000;
}

std::vector<Move_2023184014> generateMoves_2023184014(int color_2023184014, int limit_2023184014) {
    std::vector<Move_2023184014> moves_2023184014;

    if (!hasAnyStone_2023184014()) {
        Move_2023184014 move_2023184014;
        move_2023184014.x_2023184014 = 9;
        move_2023184014.y_2023184014 = 9;
        move_2023184014.score_2023184014 = 100;
        moves_2023184014.push_back(move_2023184014);
        return moves_2023184014;
    }

    // 500ms 제한 때문에 모든 빈칸을 깊게 보지 않는다.
    // 기존 돌 주변 2칸 이내 후보만 만들고, 점수순 상위 limit개만 탐색한다.
    for (int y_2023184014 = 0; y_2023184014 < SIZE_2023184014; y_2023184014++) {
        for (int x_2023184014 = 0; x_2023184014 < SIZE_2023184014; x_2023184014++) {
            if (board_2023184014[y_2023184014][x_2023184014] == EMPTY_2023184014 &&
                nearStone_2023184014(x_2023184014, y_2023184014) &&
                canPlace_2023184014(x_2023184014, y_2023184014, color_2023184014)) {
                Move_2023184014 move_2023184014;
                move_2023184014.x_2023184014 = x_2023184014;
                move_2023184014.y_2023184014 = y_2023184014;
                move_2023184014.score_2023184014 = evaluateMove_2023184014(x_2023184014, y_2023184014, color_2023184014);
                moves_2023184014.push_back(move_2023184014);
            }
        }
    }

    std::sort(moves_2023184014.begin(), moves_2023184014.end(),
        [](const Move_2023184014& a_2023184014, const Move_2023184014& b_2023184014) {
            return a_2023184014.score_2023184014 > b_2023184014.score_2023184014;
        });

    if ((int)moves_2023184014.size() > limit_2023184014) {
        moves_2023184014.resize(limit_2023184014);
    }

    return moves_2023184014;
}

int evaluateBoard_2023184014(int myColor_2023184014) {
    int enemy_2023184014 = opponent_2023184014(myColor_2023184014);
    int myScore_2023184014 = 0;
    int enemyScore_2023184014 = 0;

    for (int y_2023184014 = 0; y_2023184014 < SIZE_2023184014; y_2023184014++) {
        for (int x_2023184014 = 0; x_2023184014 < SIZE_2023184014; x_2023184014++) {
            if (board_2023184014[y_2023184014][x_2023184014] == myColor_2023184014) {
                myScore_2023184014 += evaluatePointAfterPlace_2023184014(x_2023184014, y_2023184014, myColor_2023184014);
            }
            else if (board_2023184014[y_2023184014][x_2023184014] == enemy_2023184014) {
                enemyScore_2023184014 += evaluatePointAfterPlace_2023184014(x_2023184014, y_2023184014, enemy_2023184014);
            }
        }
    }

    return myScore_2023184014 - enemyScore_2023184014;
}

int negamax_2023184014(int depth_2023184014, int color_2023184014, int rootColor_2023184014,
    int alpha_2023184014, int beta_2023184014) {
    if (timeout_2023184014()) return evaluateBoard_2023184014(rootColor_2023184014);
    if (depth_2023184014 <= 0) return evaluateBoard_2023184014(rootColor_2023184014);

    // 알파베타 탐색. 깊을수록 후보를 조금만 남겨 시간초과를 피한다.
    std::vector<Move_2023184014> moves_2023184014 = generateMoves_2023184014(color_2023184014, depth_2023184014 >= 3 ? 10 : 8);
    if (moves_2023184014.empty()) return evaluateBoard_2023184014(rootColor_2023184014);

    int best_2023184014 = -INF_2023184014;

    for (size_t i_2023184014 = 0; i_2023184014 < moves_2023184014.size(); i_2023184014++) {
        if (timeout_2023184014()) break;

        int x_2023184014 = moves_2023184014[i_2023184014].x_2023184014;
        int y_2023184014 = moves_2023184014[i_2023184014].y_2023184014;

        board_2023184014[y_2023184014][x_2023184014] = color_2023184014;

        int value_2023184014;
        if (isWin_2023184014(x_2023184014, y_2023184014, color_2023184014)) {
            value_2023184014 = (color_2023184014 == rootColor_2023184014) ? 900000000 : -900000000;
        }
        else {
            value_2023184014 = -negamax_2023184014(depth_2023184014 - 1,
                opponent_2023184014(color_2023184014),
                rootColor_2023184014,
                -beta_2023184014,
                -alpha_2023184014);
        }

        board_2023184014[y_2023184014][x_2023184014] = EMPTY_2023184014;

        if (value_2023184014 > best_2023184014) best_2023184014 = value_2023184014;
        if (value_2023184014 > alpha_2023184014) alpha_2023184014 = value_2023184014;
        if (alpha_2023184014 >= beta_2023184014) break;
    }

    return best_2023184014;
}

Move_2023184014 chooseMove_2023184014(int color_2023184014) {
    seedRandom_2023184014();

    Move_2023184014 best_2023184014;
    best_2023184014.x_2023184014 = 9;
    best_2023184014.y_2023184014 = 9;
    best_2023184014.score_2023184014 = -INF_2023184014;

    std::vector<Move_2023184014> moves_2023184014 = generateMoves_2023184014(color_2023184014, 12);
    if (moves_2023184014.empty()) {
        for (int y_2023184014 = 0; y_2023184014 < SIZE_2023184014; y_2023184014++) {
            for (int x_2023184014 = 0; x_2023184014 < SIZE_2023184014; x_2023184014++) {
                if (canPlace_2023184014(x_2023184014, y_2023184014, color_2023184014)) {
                    best_2023184014.x_2023184014 = x_2023184014;
                    best_2023184014.y_2023184014 = y_2023184014;
                    best_2023184014.score_2023184014 = 0;
                    return best_2023184014;
                }
            }
        }
        return best_2023184014;
    }

    // 최고점과 5000점 이내인 애매한 후보는 랜덤으로 골라 매번 같은 판이 되는 것을 줄인다.
    // 단, 즉시승/즉시방어/긴급방어 같은 중요한 수는 아래에서 고정 우선 처리한다.
    int firstBestScore_2023184014 = moves_2023184014[0].score_2023184014;
    std::vector<Move_2023184014> nearBestMoves_2023184014;
    // 1순위: 내가 바로 이기는 수.
    for (size_t i_2023184014 = 0; i_2023184014 < moves_2023184014.size(); i_2023184014++) {
        if (firstBestScore_2023184014 - moves_2023184014[i_2023184014].score_2023184014 <= 5000) {
            nearBestMoves_2023184014.push_back(moves_2023184014[i_2023184014]);
        }
    }
    best_2023184014 = nearBestMoves_2023184014[rand() % nearBestMoves_2023184014.size()];

    for (size_t i_2023184014 = 0; i_2023184014 < moves_2023184014.size(); i_2023184014++) {
        int x_2023184014 = moves_2023184014[i_2023184014].x_2023184014;
        int y_2023184014 = moves_2023184014[i_2023184014].y_2023184014;
        board_2023184014[y_2023184014][x_2023184014] = color_2023184014;
        bool win_2023184014 = isWin_2023184014(x_2023184014, y_2023184014, color_2023184014);
        board_2023184014[y_2023184014][x_2023184014] = EMPTY_2023184014;
        if (win_2023184014) return moves_2023184014[i_2023184014];
    }

    // 2순위: 상대가 바로 이기는 수 차단.
    int enemy_2023184014 = opponent_2023184014(color_2023184014);
    for (size_t i_2023184014 = 0; i_2023184014 < moves_2023184014.size(); i_2023184014++) {
        int x_2023184014 = moves_2023184014[i_2023184014].x_2023184014;
        int y_2023184014 = moves_2023184014[i_2023184014].y_2023184014;
        if (canPlace_2023184014(x_2023184014, y_2023184014, enemy_2023184014)) {
            board_2023184014[y_2023184014][x_2023184014] = enemy_2023184014;
            bool enemyWin_2023184014 = isWin_2023184014(x_2023184014, y_2023184014, enemy_2023184014);
            board_2023184014[y_2023184014][x_2023184014] = EMPTY_2023184014;
            if (enemyWin_2023184014) return moves_2023184014[i_2023184014];
        }
    }

    // 3순위: 내 열린3을 4로 만들어 상대에게 강제 방어를 요구한다.
    Move_2023184014 aggressiveFour_2023184014 = best_2023184014;
    if (aggressiveFourMove_2023184014(color_2023184014, aggressiveFour_2023184014)) {
        return aggressiveFour_2023184014;
    }

    // 4순위: 상대 열린3, 띈3, 열린4 같은 위험한 공격을 막는다.
    Move_2023184014 urgentDefense_2023184014 = best_2023184014;
    if (urgentDefenseMove_2023184014(color_2023184014, urgentDefense_2023184014)) {
        return urgentDefense_2023184014;
    }

    // 5순위: 백은 금수가 없으므로 열린3/띈3을 더 적극적으로 만든다.
    Move_2023184014 earlyWhiteAttack_2023184014 = best_2023184014;
    if (color_2023184014 == WHITE_2023184014 &&
        earlyWhiteOpenThreeMove_2023184014(earlyWhiteAttack_2023184014)) {
        return earlyWhiteAttack_2023184014;
    }

    // 마지막: 3~4 ply 게임트리 탐색. 450ms가 가까워지면 현재 best를 반환한다.
    for (int depth_2023184014 = 3; depth_2023184014 <= 4; depth_2023184014++) {
        if (timeout_2023184014()) break;

        Move_2023184014 depthBest_2023184014 = best_2023184014;
        int depthBestScore_2023184014 = -INF_2023184014;
        std::vector<Move_2023184014> depthNearBestMoves_2023184014;

        for (size_t i_2023184014 = 0; i_2023184014 < moves_2023184014.size(); i_2023184014++) {
            if (timeout_2023184014()) break;

            int x_2023184014 = moves_2023184014[i_2023184014].x_2023184014;
            int y_2023184014 = moves_2023184014[i_2023184014].y_2023184014;

            board_2023184014[y_2023184014][x_2023184014] = color_2023184014;

            int value_2023184014;
            if (isWin_2023184014(x_2023184014, y_2023184014, color_2023184014)) {
                value_2023184014 = 900000000;
            }
            else {
                value_2023184014 = -negamax_2023184014(depth_2023184014 - 1,
                    opponent_2023184014(color_2023184014),
                    color_2023184014,
                    -INF_2023184014,
                    INF_2023184014);
            }

            board_2023184014[y_2023184014][x_2023184014] = EMPTY_2023184014;

            if (value_2023184014 > depthBestScore_2023184014) {
                depthBestScore_2023184014 = value_2023184014;
                depthBest_2023184014 = moves_2023184014[i_2023184014];
                depthBest_2023184014.score_2023184014 = value_2023184014;
                depthNearBestMoves_2023184014.clear();
                depthNearBestMoves_2023184014.push_back(depthBest_2023184014);
            }
            else if (depthBestScore_2023184014 - value_2023184014 <= 5000) {
                Move_2023184014 nearMove_2023184014 = moves_2023184014[i_2023184014];
                nearMove_2023184014.score_2023184014 = value_2023184014;
                depthNearBestMoves_2023184014.push_back(nearMove_2023184014);
            }
        }

        if (!timeout_2023184014()) {
            if (!depthNearBestMoves_2023184014.empty()) {
                depthBest_2023184014 =
                    depthNearBestMoves_2023184014[rand() % depthNearBestMoves_2023184014.size()];
            }
            best_2023184014 = depthBest_2023184014;
        }
    }

    return best_2023184014;
}

void BlackAttack_2023184014(int* x_2023184014, int* y_2023184014) {
    startTime_2023184014 = std::chrono::high_resolution_clock::now();

    Move_2023184014 move_2023184014;
    if (!secondBlackOpeningMove_2023184014(move_2023184014)) {
        move_2023184014 = chooseMove_2023184014(BLACK_2023184014);
    }

    *x_2023184014 = move_2023184014.x_2023184014;
    *y_2023184014 = move_2023184014.y_2023184014;

    if (inBoard_2023184014(*x_2023184014, *y_2023184014)) {
        board_2023184014[*y_2023184014][*x_2023184014] = BLACK_2023184014;
    }
}

void BlackDefence_2023184014(int x_2023184014, int y_2023184014) {
    if (inBoard_2023184014(x_2023184014, y_2023184014)) {
        board_2023184014[y_2023184014][x_2023184014] = WHITE_2023184014;
    }
}

void WhiteAttack_2023184014(int* x_2023184014, int* y_2023184014) {
    startTime_2023184014 = std::chrono::high_resolution_clock::now();

    Move_2023184014 move_2023184014 = chooseMove_2023184014(WHITE_2023184014);

    *x_2023184014 = move_2023184014.x_2023184014;
    *y_2023184014 = move_2023184014.y_2023184014;

    if (inBoard_2023184014(*x_2023184014, *y_2023184014)) {
        board_2023184014[*y_2023184014][*x_2023184014] = WHITE_2023184014;
    }
}

void WhiteDefence_2023184014(int x_2023184014, int y_2023184014) {
    if (inBoard_2023184014(x_2023184014, y_2023184014)) {
        board_2023184014[y_2023184014][x_2023184014] = BLACK_2023184014;
    }
}
