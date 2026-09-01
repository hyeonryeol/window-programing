#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <vector>

const int SIZE_2023180023 = 19;
const int EMPTY_2023180023 = 0;
const int BLACK_2023180023 = -1;
const int WHITE_2023180023 = 1;
const int INF_2023180023 = 1000000000;
const int WIN_SCORE_2023180023 = 900000000;
const int TIME_LIMIT_MS_2023180023 = 430;

int board_2023180023[SIZE_2023180023][SIZE_2023180023] = { 0 };

struct Move_2023180023 {
    int x;
    int y;
    int score;
};

struct Feature_2023180023 {
    int win;
    int overline;
    int four;
    int openFour;
    int closedFour;
    int openThree;
    int jumpThree;
    int doubleThree;
    int fourThree;
    int fourFour;
    int winMoves;
};

struct TTEntry_2023180023 {
    unsigned long long key;
    int depth;
    int color;
    int rootColor;
    int value;
    int flag;
};

std::chrono::high_resolution_clock::time_point startTime_2023180023;

const int TT_SIZE_2023180023 = 32768;
TTEntry_2023180023 tt_2023180023[TT_SIZE_2023180023] = { 0 };
int killerX_2023180023[16][2];
int killerY_2023180023[16][2];
int historyScore_2023180023[2][SIZE_2023180023][SIZE_2023180023] = { 0 };

int dx_2023180023[4] = { 1, 0, 1, 1 };
int dy_2023180023[4] = { 0, 1, 1, -1 };

bool inBoard_2023180023(int x, int y) {
    return 0 <= x && x < SIZE_2023180023 && 0 <= y && y < SIZE_2023180023;
}

int enemy_2023180023(int color) {
    return -color;
}

int colorIndex_2023180023(int color) {
    return color == BLACK_2023180023 ? 0 : 1;
}

bool timeout_2023180023() {
    auto now = std::chrono::high_resolution_clock::now();
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime_2023180023).count();
    return ms >= TIME_LIMIT_MS_2023180023;
}

unsigned long long mix64_2023180023(unsigned long long x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}

unsigned long long cellHash_2023180023(int x, int y, int color) {
    unsigned long long id = (unsigned long long)(y * SIZE_2023180023 + x + 1);
    unsigned long long c = (color == BLACK_2023180023) ? 0x123456789abcdefULL : 0xfedcba987654321ULL;
    return mix64_2023180023(id * 0x9e3779b97f4a7c15ULL ^ c);
}

unsigned long long boardHash_2023180023() {
    unsigned long long h = 0xcbf29ce484222325ULL;
    for (int y = 0; y < SIZE_2023180023; y++) {
        for (int x = 0; x < SIZE_2023180023; x++) {
            int color = board_2023180023[y][x];
            if (color != EMPTY_2023180023) h ^= cellHash_2023180023(x, y, color);
        }
    }
    return h;
}

void resetSearchTables_2023180023() {
    for (int i = 0; i < 16; i++) {
        killerX_2023180023[i][0] = killerX_2023180023[i][1] = -1;
        killerY_2023180023[i][0] = killerY_2023180023[i][1] = -1;
    }
}

int clampScore_2023180023(long long value) {
    if (value > INF_2023180023) return INF_2023180023;
    if (value < -INF_2023180023) return -INF_2023180023;
    return (int)value;
}

int countOneSide_2023180023(int x, int y, int color, int dirX, int dirY) {
    int count = 0;
    x += dirX;
    y += dirY;

    while (inBoard_2023180023(x, y) && board_2023180023[y][x] == color) {
        count++;
        x += dirX;
        y += dirY;
    }

    return count;
}

int lineCount_2023180023(int x, int y, int color, int dir) {
    return 1
        + countOneSide_2023180023(x, y, color, dx_2023180023[dir], dy_2023180023[dir])
        + countOneSide_2023180023(x, y, color, -dx_2023180023[dir], -dy_2023180023[dir]);
}

int openEnds_2023180023(int x, int y, int color, int dir) {
    int open = 0;
    int nx = x + dx_2023180023[dir];
    int ny = y + dy_2023180023[dir];

    while (inBoard_2023180023(nx, ny) && board_2023180023[ny][nx] == color) {
        nx += dx_2023180023[dir];
        ny += dy_2023180023[dir];
    }

    if (inBoard_2023180023(nx, ny) && board_2023180023[ny][nx] == EMPTY_2023180023) open++;

    nx = x - dx_2023180023[dir];
    ny = y - dy_2023180023[dir];

    while (inBoard_2023180023(nx, ny) && board_2023180023[ny][nx] == color) {
        nx -= dx_2023180023[dir];
        ny -= dy_2023180023[dir];
    }

    if (inBoard_2023180023(nx, ny) && board_2023180023[ny][nx] == EMPTY_2023180023) open++;

    return open;
}

bool isWin_2023180023(int x, int y, int color) {
    for (int dir = 0; dir < 4; dir++) {
        int count = lineCount_2023180023(x, y, color, dir);
        if (color == BLACK_2023180023) {
            if (count == 5) return true;
        }
        else {
            if (count >= 5) return true;
        }
    }

    return false;
}

bool isOverline_2023180023(int x, int y, int color) {
    for (int dir = 0; dir < 4; dir++) {
        if (lineCount_2023180023(x, y, color, dir) >= 6) return true;
    }

    return false;
}

bool linePattern_2023180023(int x, int y, int color, int dir, const int pattern[], int len) {
    for (int offset = 0; offset < len; offset++) {
        int sx = x - dx_2023180023[dir] * offset;
        int sy = y - dy_2023180023[dir] * offset;
        bool ok = true;

        for (int i = 0; i < len; i++) {
            int nx = sx + dx_2023180023[dir] * i;
            int ny = sy + dy_2023180023[dir] * i;

            if (!inBoard_2023180023(nx, ny)) {
                ok = false;
                break;
            }

            if (pattern[i] == 0 && board_2023180023[ny][nx] != EMPTY_2023180023) {
                ok = false;
                break;
            }

            if (pattern[i] == 1 && board_2023180023[ny][nx] != color) {
                ok = false;
                break;
            }
        }

        if (ok) return true;
    }

    return false;
}

bool hasOpenThreeDir_2023180023(int x, int y, int color, int dir) {
    int p1[5] = { 0, 1, 1, 1, 0 };
    return linePattern_2023180023(x, y, color, dir, p1, 5);
}

bool hasJumpThreeDir_2023180023(int x, int y, int color, int dir) {
    int p1[6] = { 0, 1, 1, 0, 1, 0 };
    int p2[6] = { 0, 1, 0, 1, 1, 0 };
    return linePattern_2023180023(x, y, color, dir, p1, 6)
        || linePattern_2023180023(x, y, color, dir, p2, 6);
}

int countOpenThree_2023180023(int x, int y, int color) {
    int count = 0;
    for (int dir = 0; dir < 4; dir++) {
        if (hasOpenThreeDir_2023180023(x, y, color, dir)) count++;
    }
    return count;
}

int countJumpThree_2023180023(int x, int y, int color) {
    int count = 0;
    for (int dir = 0; dir < 4; dir++) {
        if (hasJumpThreeDir_2023180023(x, y, color, dir)) count++;
    }
    return count;
}

int winMovesInDirAfterPlaced_2023180023(int x, int y, int color, int dir, bool seen[][SIZE_2023180023]) {
    int result = 0;

    for (int offset = -5; offset <= 5; offset++) {
        int nx = x + dx_2023180023[dir] * offset;
        int ny = y + dy_2023180023[dir] * offset;

        if (!inBoard_2023180023(nx, ny)) continue;
        if (board_2023180023[ny][nx] != EMPTY_2023180023) continue;

        board_2023180023[ny][nx] = color;
        bool win = isWin_2023180023(nx, ny, color);
        board_2023180023[ny][nx] = EMPTY_2023180023;

        if (win) {
            result++;
            seen[ny][nx] = true;
        }
    }

    return result;
}

Feature_2023180023 featuresAfterPlaced_2023180023(int x, int y, int color) {
    Feature_2023180023 f;
    std::memset(&f, 0, sizeof(f));

    f.win = isWin_2023180023(x, y, color) ? 1 : 0;
    f.overline = (color == BLACK_2023180023 && isOverline_2023180023(x, y, color)) ? 1 : 0;

    bool seen[SIZE_2023180023][SIZE_2023180023];
    std::memset(seen, 0, sizeof(seen));

    for (int dir = 0; dir < 4; dir++) {
        int wins = winMovesInDirAfterPlaced_2023180023(x, y, color, dir, seen);
        if (wins >= 1) f.four++;
        if (wins >= 2) f.openFour++;
        else if (wins == 1) f.closedFour++;

        if (hasOpenThreeDir_2023180023(x, y, color, dir)) f.openThree++;
        if (hasJumpThreeDir_2023180023(x, y, color, dir)) f.jumpThree++;
    }

    for (int yy = 0; yy < SIZE_2023180023; yy++) {
        for (int xx = 0; xx < SIZE_2023180023; xx++) {
            if (seen[yy][xx]) f.winMoves++;
        }
    }

    int threeCount = f.openThree + f.jumpThree;
    f.doubleThree = (threeCount >= 2) ? 1 : 0;
    f.fourThree = (f.four >= 1 && threeCount >= 1) ? 1 : 0;
    f.fourFour = (f.four >= 2) ? 1 : 0;

    return f;
}

bool isForbiddenBlack_2023180023(int x, int y) {
    if (!inBoard_2023180023(x, y)) return true;
    if (board_2023180023[y][x] != EMPTY_2023180023) return true;

    board_2023180023[y][x] = BLACK_2023180023;
    Feature_2023180023 f = featuresAfterPlaced_2023180023(x, y, BLACK_2023180023);
    board_2023180023[y][x] = EMPTY_2023180023;

    if (f.win) return false;
    if (f.overline) return true;
    if (f.fourFour) return true;
    if (f.doubleThree) return true;

    return false;
}

bool canPlace_2023180023(int x, int y, int color) {
    if (!inBoard_2023180023(x, y)) return false;
    if (board_2023180023[y][x] != EMPTY_2023180023) return false;
    if (color == BLACK_2023180023 && isForbiddenBlack_2023180023(x, y)) return false;
    return true;
}

bool hasAnyStone_2023180023() {
    for (int y = 0; y < SIZE_2023180023; y++) {
        for (int x = 0; x < SIZE_2023180023; x++) {
            if (board_2023180023[y][x] != EMPTY_2023180023) return true;
        }
    }
    return false;
}

int totalStoneCount_2023180023() {
    int count = 0;
    for (int y = 0; y < SIZE_2023180023; y++) {
        for (int x = 0; x < SIZE_2023180023; x++) {
            if (board_2023180023[y][x] != EMPTY_2023180023) count++;
        }
    }
    return count;
}

bool nearStoneRadius_2023180023(int x, int y, int radius) {
    for (int yy = y - radius; yy <= y + radius; yy++) {
        for (int xx = x - radius; xx <= x + radius; xx++) {
            if (inBoard_2023180023(xx, yy) && board_2023180023[yy][xx] != EMPTY_2023180023) return true;
        }
    }
    return false;
}

int centerScore_2023180023(int x, int y) {
    int dist = std::abs(x - 9) + std::abs(y - 9);
    return 80 - dist * 4;
}

long long featureAttackValue_2023180023(const Feature_2023180023& f, int color) {
    if (f.win) return WIN_SCORE_2023180023;
    if (color == BLACK_2023180023 && f.overline) return -WIN_SCORE_2023180023;

    long long score = 0;

    if (f.openFour > 0) score += 76000000LL + (long long)f.openFour * 5000000LL;
    if (f.fourFour) score += (color == WHITE_2023180023 ? 70000000LL : 2000000LL);
    if (f.fourThree) score += (color == WHITE_2023180023 ? 66000000LL : 62000000LL);
    if (f.closedFour > 0) score += (long long)f.closedFour * 12000000LL;
    if (f.winMoves >= 2) score += 52000000LL + (long long)f.winMoves * 3000000LL;

    if (f.doubleThree) {
        score += (color == WHITE_2023180023 ? 26000000LL : 800000LL);
    }

    score += (long long)f.openThree * (color == WHITE_2023180023 ? 4500000LL : 2600000LL);
    score += (long long)f.jumpThree * (color == WHITE_2023180023 ? 3300000LL : 1800000LL);

    return score;
}

long long runScoreAfterPlaced_2023180023(int x, int y, int color) {
    long long score = 0;

    for (int dir = 0; dir < 4; dir++) {
        int len = lineCount_2023180023(x, y, color, dir);
        int open = openEnds_2023180023(x, y, color, dir);

        if (color == BLACK_2023180023 && len >= 6) score -= 50000000LL;
        else if (len >= 5) score += (color == BLACK_2023180023 && len != 5) ? 0 : 120000000LL;
        else if (len == 4) score += (open == 2) ? 16000000LL : (open == 1 ? 5000000LL : 250000LL);
        else if (len == 3) score += (open == 2) ? 1500000LL : (open == 1 ? 220000LL : 20000LL);
        else if (len == 2) score += (open == 2) ? 80000LL : (open == 1 ? 10000LL : 1000LL);
        else score += (open == 2) ? 1200LL : 100LL;
    }

    return score;
}

long long attackValueAt_2023180023(int x, int y, int color) {
    if (!canPlace_2023180023(x, y, color)) return -WIN_SCORE_2023180023;

    board_2023180023[y][x] = color;
    Feature_2023180023 f = featuresAfterPlaced_2023180023(x, y, color);
    long long score = featureAttackValue_2023180023(f, color) + runScoreAfterPlaced_2023180023(x, y, color);
    board_2023180023[y][x] = EMPTY_2023180023;

    return score;
}

int evaluateMove_2023180023(int x, int y, int color) {
    if (!canPlace_2023180023(x, y, color)) return -INF_2023180023;

    int other = enemy_2023180023(color);
    long long attack = attackValueAt_2023180023(x, y, color);
    long long defence = 0;

    if (canPlace_2023180023(x, y, other)) {
        defence = attackValueAt_2023180023(x, y, other);
    }

    long long score;
    if (color == BLACK_2023180023) {
        score = attack * 100 / 100 + defence * 155 / 100;
    }
    else {
        score = attack * 150 / 100 + defence * 105 / 100;
    }

    int stones = totalStoneCount_2023180023();
    if (stones < 18) {
        score += (long long)centerScore_2023180023(x, y) * (20 - stones) * 1000LL;
    }
    else {
        score += centerScore_2023180023(x, y) * 50;
    }

    return clampScore_2023180023(score);
}

void pushMove_2023180023(std::vector<Move_2023180023>& moves, bool seen[][SIZE_2023180023], int x, int y, int color) {
    if (!inBoard_2023180023(x, y)) return;
    if (seen[y][x]) return;
    if (!canPlace_2023180023(x, y, color)) return;

    Move_2023180023 m;
    m.x = x;
    m.y = y;
    m.score = evaluateMove_2023180023(x, y, color);
    moves.push_back(m);
    seen[y][x] = true;
}

std::vector<Move_2023180023> generateMoves_2023180023(int color, int limit, bool includeTactical) {
    std::vector<Move_2023180023> moves;
    bool seen[SIZE_2023180023][SIZE_2023180023];
    std::memset(seen, 0, sizeof(seen));

    if (!hasAnyStone_2023180023()) {
        pushMove_2023180023(moves, seen, 9, 9, color);
        return moves;
    }

    for (int y = 0; y < SIZE_2023180023; y++) {
        for (int x = 0; x < SIZE_2023180023; x++) {
            if (timeout_2023180023()) break;
            if (board_2023180023[y][x] != EMPTY_2023180023) continue;
            if (!nearStoneRadius_2023180023(x, y, 2)) continue;
            pushMove_2023180023(moves, seen, x, y, color);
        }
    }

    if (includeTactical) {
        int other = enemy_2023180023(color);
        for (int y = 0; y < SIZE_2023180023; y++) {
            for (int x = 0; x < SIZE_2023180023; x++) {
                if (timeout_2023180023()) break;
                if (seen[y][x] || board_2023180023[y][x] != EMPTY_2023180023) continue;

                bool important = false;
                if (canPlace_2023180023(x, y, color)) {
                    board_2023180023[y][x] = color;
                    Feature_2023180023 f = featuresAfterPlaced_2023180023(x, y, color);
                    board_2023180023[y][x] = EMPTY_2023180023;
                    important = f.win || f.openFour || f.fourThree || f.fourFour;
                }

                if (!important && canPlace_2023180023(x, y, other)) {
                    board_2023180023[y][x] = other;
                    Feature_2023180023 f = featuresAfterPlaced_2023180023(x, y, other);
                    board_2023180023[y][x] = EMPTY_2023180023;
                    important = f.win || f.openFour || f.fourThree || f.fourFour;
                }

                if (important) pushMove_2023180023(moves, seen, x, y, color);
            }
        }
    }

    std::sort(moves.begin(), moves.end(), [](const Move_2023180023& a, const Move_2023180023& b) {
        if (a.score != b.score) return a.score > b.score;
        int da = std::abs(a.x - 9) + std::abs(a.y - 9);
        int db = std::abs(b.x - 9) + std::abs(b.y - 9);
        return da < db;
    });

    if (limit > 0 && (int)moves.size() > limit) moves.resize(limit);
    return moves;
}

void orderMovesForSearch_2023180023(std::vector<Move_2023180023>& moves, int color, int ply) {
    int ci = colorIndex_2023180023(color);
    for (size_t i = 0; i < moves.size(); i++) {
        int x = moves[i].x;
        int y = moves[i].y;
        int bonus = 0;
        if (ply >= 0 && ply < 16) {
            if (killerX_2023180023[ply][0] == x && killerY_2023180023[ply][0] == y) bonus += 30000000;
            if (killerX_2023180023[ply][1] == x && killerY_2023180023[ply][1] == y) bonus += 15000000;
        }
        bonus += historyScore_2023180023[ci][y][x] / 8;
        moves[i].score = clampScore_2023180023((long long)moves[i].score + bonus);
    }

    std::sort(moves.begin(), moves.end(), [](const Move_2023180023& a, const Move_2023180023& b) {
        return a.score > b.score;
    });
}

void recordKiller_2023180023(int ply, int x, int y) {
    if (ply < 0 || ply >= 16) return;
    if (killerX_2023180023[ply][0] == x && killerY_2023180023[ply][0] == y) return;
    killerX_2023180023[ply][1] = killerX_2023180023[ply][0];
    killerY_2023180023[ply][1] = killerY_2023180023[ply][0];
    killerX_2023180023[ply][0] = x;
    killerY_2023180023[ply][0] = y;
}

bool featureOfMove_2023180023(int x, int y, int color, Feature_2023180023& f) {
    if (!canPlace_2023180023(x, y, color)) return false;
    board_2023180023[y][x] = color;
    f = featuresAfterPlaced_2023180023(x, y, color);
    board_2023180023[y][x] = EMPTY_2023180023;
    return true;
}

bool findImmediateWin_2023180023(int color, Move_2023180023& best) {
    int bestScore = -INF_2023180023;

    for (int y = 0; y < SIZE_2023180023; y++) {
        for (int x = 0; x < SIZE_2023180023; x++) {
            if (timeout_2023180023()) return bestScore > -INF_2023180023;
            if (!canPlace_2023180023(x, y, color)) continue;

            board_2023180023[y][x] = color;
            bool win = isWin_2023180023(x, y, color);
            board_2023180023[y][x] = EMPTY_2023180023;

            if (win) {
                int score = evaluateMove_2023180023(x, y, color);
                if (score > bestScore) {
                    best.x = x;
                    best.y = y;
                    best.score = WIN_SCORE_2023180023;
                    bestScore = score;
                }
            }
        }
    }

    return bestScore > -INF_2023180023;
}

std::vector<Move_2023180023> immediateWinMoves_2023180023(int color, int limit) {
    std::vector<Move_2023180023> result;
    std::vector<Move_2023180023> moves = generateMoves_2023180023(color, 96, false);

    for (size_t i = 0; i < moves.size(); i++) {
        if (timeout_2023180023()) break;
        int x = moves[i].x;
        int y = moves[i].y;
        if (!canPlace_2023180023(x, y, color)) continue;

        board_2023180023[y][x] = color;
        bool win = isWin_2023180023(x, y, color);
        board_2023180023[y][x] = EMPTY_2023180023;

        if (win) {
            Move_2023180023 m;
            m.x = x;
            m.y = y;
            m.score = WIN_SCORE_2023180023;
            result.push_back(m);
            if (limit > 0 && (int)result.size() >= limit) break;
        }
    }

    return result;
}

bool findThreatMove_2023180023(int color, int mode, Move_2023180023& best) {
    int bestScore = -INF_2023180023;
    std::vector<Move_2023180023> moves = generateMoves_2023180023(color, 96, false);

    for (size_t i = 0; i < moves.size(); i++) {
        if (timeout_2023180023()) return bestScore > -INF_2023180023;
        int x = moves[i].x;
        int y = moves[i].y;
        if (!canPlace_2023180023(x, y, color)) continue;

        board_2023180023[y][x] = color;
        Feature_2023180023 f = featuresAfterPlaced_2023180023(x, y, color);
        long long value = featureAttackValue_2023180023(f, color) + runScoreAfterPlaced_2023180023(x, y, color);
        board_2023180023[y][x] = EMPTY_2023180023;

        bool ok = false;
        if (mode == 1) ok = (f.openFour > 0);
        else if (mode == 2) ok = (f.fourThree || f.fourFour || f.winMoves >= 2);
        else if (mode == 3) ok = (f.doubleThree || f.openThree + f.jumpThree >= 1);

        if (ok) {
            value += moves[i].score / 4;
            if (value > bestScore) {
                best.x = x;
                best.y = y;
                best.score = clampScore_2023180023(value);
                bestScore = best.score;
            }
        }
    }

    return bestScore > -INF_2023180023;
}

bool threatModeOk_2023180023(const Feature_2023180023& f, int color, int mode) {
    if (mode == 1) return f.openFour > 0;
    if (mode == 2) return f.fourThree || f.fourFour || f.winMoves >= 2;
    if (mode == 3) return f.doubleThree || f.openThree + f.jumpThree >= 1;
    return false;
}

long long dangerFromFeature_2023180023(const Feature_2023180023& f, int color) {
    if (f.win) return 820000000LL;
    if (f.openFour > 0) return 540000000LL + (long long)f.openFour * 20000000LL;
    if (f.fourFour || f.fourThree || f.winMoves >= 2) return 360000000LL + (long long)f.winMoves * 10000000LL;
    if (color == WHITE_2023180023 && f.doubleThree) return 90000000LL;
    if (f.closedFour > 0) return 70000000LL;
    if (f.openThree + f.jumpThree >= 2) return 45000000LL;
    if (f.openThree + f.jumpThree >= 1) return 16000000LL;
    return 0;
}

long long opponentDangerScore_2023180023(int opponentColor) {
    long long best = 0;
    int severeCount = 0;
    int mediumCount = 0;

    for (int y = 0; y < SIZE_2023180023; y++) {
        for (int x = 0; x < SIZE_2023180023; x++) {
            if (timeout_2023180023()) return best + (long long)severeCount * 25000000LL + (long long)mediumCount * 3000000LL;
            if (!canPlace_2023180023(x, y, opponentColor)) continue;

            board_2023180023[y][x] = opponentColor;
            Feature_2023180023 f = featuresAfterPlaced_2023180023(x, y, opponentColor);
            board_2023180023[y][x] = EMPTY_2023180023;

            long long danger = dangerFromFeature_2023180023(f, opponentColor);
            if (danger > best) best = danger;
            if (danger >= 300000000LL) severeCount++;
            else if (danger >= 45000000LL) mediumCount++;
        }
    }

    return best + (long long)severeCount * 25000000LL + (long long)mediumCount * 3000000LL;
}

bool findSafetyMove_2023180023(int myColor, long long threshold, Move_2023180023& best) {
    int opponent = enemy_2023180023(myColor);
    long long currentDanger = opponentDangerScore_2023180023(opponent);
    if (currentDanger < threshold) return false;

    std::vector<Move_2023180023> moves = generateMoves_2023180023(myColor, 34, true);
    if (moves.empty()) return false;

    long long bestValue = -900000000000LL;
    long long bestDanger = 900000000000LL;

    for (size_t i = 0; i < moves.size(); i++) {
        if (timeout_2023180023()) break;

        int x = moves[i].x;
        int y = moves[i].y;
        if (!canPlace_2023180023(x, y, myColor)) continue;

        board_2023180023[y][x] = myColor;

        long long value;
        long long danger;
        if (isWin_2023180023(x, y, myColor)) {
            danger = 0;
            value = WIN_SCORE_2023180023;
        }
        else {
            danger = opponentDangerScore_2023180023(opponent);
            value = -danger + (long long)moves[i].score / 2;
        }

        board_2023180023[y][x] = EMPTY_2023180023;

        if (danger < bestDanger || (danger == bestDanger && value > bestValue)) {
            bestDanger = danger;
            bestValue = value;
            best = moves[i];
            best.score = clampScore_2023180023(value);
        }
    }

    return bestDanger < currentDanger || currentDanger >= 300000000LL;
}

bool findBlockThreatMove_2023180023(int myColor, int mode, Move_2023180023& best) {
    int opponent = enemy_2023180023(myColor);
    long long bestDanger = 900000000000LL;
    long long bestValue = -900000000000LL;
    bool found = false;
    int checkedThreats = 0;
    std::vector<Move_2023180023> opponentMoves = generateMoves_2023180023(opponent, 96, false);

    for (size_t i = 0; i < opponentMoves.size(); i++) {
        if (timeout_2023180023()) return found;
        if (checkedThreats >= 20) break;

        int x = opponentMoves[i].x;
        int y = opponentMoves[i].y;
        if (!canPlace_2023180023(x, y, opponent)) continue;

        board_2023180023[y][x] = opponent;
        Feature_2023180023 f = featuresAfterPlaced_2023180023(x, y, opponent);
        long long opponentValue = featureAttackValue_2023180023(f, opponent) + runScoreAfterPlaced_2023180023(x, y, opponent);
        board_2023180023[y][x] = EMPTY_2023180023;

        if (!threatModeOk_2023180023(f, opponent, mode)) continue;
        checkedThreats++;
        if (!canPlace_2023180023(x, y, myColor)) continue;

        board_2023180023[y][x] = myColor;
        long long danger = opponentDangerScore_2023180023(opponent);
        board_2023180023[y][x] = EMPTY_2023180023;

        long long value = -danger + opponentValue + (long long)evaluateMove_2023180023(x, y, myColor) / 2;
        long long adjustedDanger = danger - opponentValue / 20;

        if (!found || adjustedDanger < bestDanger || (adjustedDanger == bestDanger && value > bestValue)) {
            found = true;
            bestDanger = adjustedDanger;
            bestValue = value;
            best.x = x;
            best.y = y;
            best.score = clampScore_2023180023(value);
        }
    }

    return found;
}

bool findLatentThreatBlock_2023180023(int myColor, long long threshold, Move_2023180023& best) {
    int opponent = enemy_2023180023(myColor);
    std::vector<Move_2023180023> opponentMoves = generateMoves_2023180023(opponent, 40, false);
    long long bestValue = -900000000000LL;
    bool found = false;

    for (size_t i = 0; i < opponentMoves.size(); i++) {
        if (timeout_2023180023()) break;

        int x = opponentMoves[i].x;
        int y = opponentMoves[i].y;
        if (!canPlace_2023180023(x, y, opponent)) continue;
        if (!canPlace_2023180023(x, y, myColor)) continue;

        board_2023180023[y][x] = opponent;
        Feature_2023180023 f = featuresAfterPlaced_2023180023(x, y, opponent);
        bool seed = f.closedFour > 0 || f.openThree > 0 || f.jumpThree > 0 ||
            f.doubleThree || f.fourThree || f.fourFour || f.winMoves > 0;

        long long immediate = seed ? dangerFromFeature_2023180023(f, opponent) : 0;
        long long future = seed ? opponentDangerScore_2023180023(opponent) : 0;
        long long shape = seed ? featureAttackValue_2023180023(f, opponent) + runScoreAfterPlaced_2023180023(x, y, opponent) : 0;
        board_2023180023[y][x] = EMPTY_2023180023;

        if (!seed) continue;

        bool urgent = future >= threshold;
        if (!urgent && immediate >= 45000000LL && future >= threshold / 2) urgent = true;
        if (!urgent && f.closedFour > 0 && (f.openThree + f.jumpThree) > 0) urgent = true;
        if (!urgent && myColor == BLACK_2023180023 && (f.openThree + f.jumpThree) > 0 && future >= 120000000LL) urgent = true;
        if (!urgent) continue;

        long long value = future * 2 + immediate * 3 + shape / 4 + (long long)opponentMoves[i].score / 2;
        if (!found || value > bestValue) {
            found = true;
            bestValue = value;
            best.x = x;
            best.y = y;
            best.score = clampScore_2023180023(value);
        }
    }

    return found;
}

bool findForbiddenBlockTrap_2023180023(int myColor, Move_2023180023& best) {
    if (myColor != BLACK_2023180023) return false;

    int opponent = enemy_2023180023(myColor);
    std::vector<Move_2023180023> opponentMoves = generateMoves_2023180023(opponent, 36, false);
    long long bestValue = -900000000000LL;
    bool found = false;

    for (size_t i = 0; i < opponentMoves.size(); i++) {
        if (timeout_2023180023()) break;

        int x = opponentMoves[i].x;
        int y = opponentMoves[i].y;
        if (!canPlace_2023180023(x, y, opponent)) continue;
        if (!canPlace_2023180023(x, y, myColor)) continue;

        board_2023180023[y][x] = opponent;
        Feature_2023180023 f = featuresAfterPlaced_2023180023(x, y, opponent);
        bool check = f.openFour || f.closedFour || f.fourThree || f.fourFour || f.winMoves > 0;
        std::vector<Move_2023180023> wins;
        if (check) wins = immediateWinMoves_2023180023(opponent, 3);

        bool trap = false;
        int forbiddenBlocks = 0;
        for (size_t j = 0; j < wins.size(); j++) {
            if (!canPlace_2023180023(wins[j].x, wins[j].y, myColor)) forbiddenBlocks++;
        }
        if (wins.size() >= 2 && forbiddenBlocks > 0) trap = true;
        if (wins.size() == 1 && forbiddenBlocks == 1) trap = true;

        long long value = featureAttackValue_2023180023(f, opponent) +
            runScoreAfterPlaced_2023180023(x, y, opponent) +
            (long long)forbiddenBlocks * 300000000LL +
            (long long)wins.size() * 90000000LL +
            (long long)opponentMoves[i].score / 2;
        board_2023180023[y][x] = EMPTY_2023180023;

        if (!trap) continue;
        if (!found || value > bestValue) {
            found = true;
            bestValue = value;
            best.x = x;
            best.y = y;
            best.score = clampScore_2023180023(value);
        }
    }

    return found;
}

std::vector<Move_2023180023> forcingMoves_2023180023(int color, int limit) {
    std::vector<Move_2023180023> result;
    std::vector<Move_2023180023> moves = generateMoves_2023180023(color, 64, false);

    for (size_t i = 0; i < moves.size(); i++) {
        if (timeout_2023180023()) break;
        int x = moves[i].x;
        int y = moves[i].y;
        if (!canPlace_2023180023(x, y, color)) continue;

        board_2023180023[y][x] = color;
        Feature_2023180023 f = featuresAfterPlaced_2023180023(x, y, color);
        long long value = featureAttackValue_2023180023(f, color) + runScoreAfterPlaced_2023180023(x, y, color);
        board_2023180023[y][x] = EMPTY_2023180023;

        bool forcing = f.win || f.openFour || f.fourThree || f.fourFour || f.winMoves >= 2;
        if (color == WHITE_2023180023 && (f.doubleThree || f.openThree + f.jumpThree >= 2)) forcing = true;

        if (forcing) {
            Move_2023180023 m;
            m.x = x;
            m.y = y;
            m.score = clampScore_2023180023(value + moves[i].score / 3);
            result.push_back(m);
        }
    }

    std::sort(result.begin(), result.end(), [](const Move_2023180023& a, const Move_2023180023& b) {
        return a.score > b.score;
    });

    if (limit > 0 && (int)result.size() > limit) result.resize(limit);
    return result;
}

bool threatSpaceWin_2023180023(int attacker, int defender, int depth) {
    if (timeout_2023180023()) return false;
    if (depth <= 0) return false;

    std::vector<Move_2023180023> attackerWins = immediateWinMoves_2023180023(attacker, 2);
    if (!attackerWins.empty()) return true;

    std::vector<Move_2023180023> defenderWins = immediateWinMoves_2023180023(defender, 1);
    if (!defenderWins.empty()) return false;

    std::vector<Move_2023180023> forces = forcingMoves_2023180023(attacker, 8);
    for (size_t i = 0; i < forces.size(); i++) {
        if (timeout_2023180023()) return false;
        int x = forces[i].x;
        int y = forces[i].y;
        if (!canPlace_2023180023(x, y, attacker)) continue;

        board_2023180023[y][x] = attacker;
        bool directWin = isWin_2023180023(x, y, attacker);
        std::vector<Move_2023180023> winsAfterForce = directWin
            ? std::vector<Move_2023180023>()
            : immediateWinMoves_2023180023(attacker, 3);

        bool winning = directWin || winsAfterForce.size() >= 2;

        if (!winning && winsAfterForce.size() == 1) {
            int bx = winsAfterForce[0].x;
            int by = winsAfterForce[0].y;
            if (!canPlace_2023180023(bx, by, defender)) {
                winning = true;
            }
            else {
                board_2023180023[by][bx] = defender;
                winning = threatSpaceWin_2023180023(attacker, defender, depth - 2);
                board_2023180023[by][bx] = EMPTY_2023180023;
            }
        }

        board_2023180023[y][x] = EMPTY_2023180023;
        if (winning) return true;
    }

    return false;
}

bool findThreatSpaceMove_2023180023(int color, Move_2023180023& best) {
    int defender = enemy_2023180023(color);
    std::vector<Move_2023180023> forces = forcingMoves_2023180023(color, 10);
    int bestScore = -INF_2023180023;

    for (size_t i = 0; i < forces.size(); i++) {
        if (timeout_2023180023()) break;
        int x = forces[i].x;
        int y = forces[i].y;
        if (!canPlace_2023180023(x, y, color)) continue;

        board_2023180023[y][x] = color;
        bool winning = isWin_2023180023(x, y, color);
        if (!winning) {
            std::vector<Move_2023180023> wins = immediateWinMoves_2023180023(color, 3);
            winning = wins.size() >= 2;
            if (!winning && wins.size() == 1) {
                int bx = wins[0].x;
                int by = wins[0].y;
                if (!canPlace_2023180023(bx, by, defender)) {
                    winning = true;
                }
                else {
                    board_2023180023[by][bx] = defender;
                    winning = threatSpaceWin_2023180023(color, defender, 6);
                    board_2023180023[by][bx] = EMPTY_2023180023;
                }
            }
        }
        board_2023180023[y][x] = EMPTY_2023180023;

        if (winning && forces[i].score > bestScore) {
            bestScore = forces[i].score;
            best = forces[i];
        }
    }

    return bestScore > -INF_2023180023;
}

long long fastColorRunScore_2023180023(int color) {
    long long score = 0;

    for (int dir = 0; dir < 4; dir++) {
        int dirX = dx_2023180023[dir];
        int dirY = dy_2023180023[dir];

        for (int y = 0; y < SIZE_2023180023; y++) {
            for (int x = 0; x < SIZE_2023180023; x++) {
                if (board_2023180023[y][x] != color) continue;

                int px = x - dirX;
                int py = y - dirY;
                if (inBoard_2023180023(px, py) && board_2023180023[py][px] == color) continue;

                int len = 1;
                int nx = x + dirX;
                int ny = y + dirY;
                while (inBoard_2023180023(nx, ny) && board_2023180023[ny][nx] == color) {
                    len++;
                    nx += dirX;
                    ny += dirY;
                }

                int open = 0;
                if (inBoard_2023180023(px, py) && board_2023180023[py][px] == EMPTY_2023180023) open++;
                if (inBoard_2023180023(nx, ny) && board_2023180023[ny][nx] == EMPTY_2023180023) open++;

                if (color == BLACK_2023180023 && len >= 6) score -= 20000000LL;
                else if (len >= 5) score += (color == BLACK_2023180023 && len != 5) ? 0 : 100000000LL;
                else if (len == 4) score += (open == 2) ? 9000000LL : (open == 1 ? 1800000LL : 0);
                else if (len == 3) score += (open == 2) ? 450000LL : (open == 1 ? 50000LL : 0);
                else if (len == 2) score += (open == 2) ? 22000LL : (open == 1 ? 3000LL : 0);
                else score += (open == 2) ? 200LL : 0;
            }
        }
    }

    return score;
}

long long bestPotential_2023180023(int color) {
    std::vector<Move_2023180023> moves = generateMoves_2023180023(color, 10, false);
    long long sum = 0;
    int used = 0;

    for (size_t i = 0; i < moves.size() && used < 3; i++, used++) {
        long long v = attackValueAt_2023180023(moves[i].x, moves[i].y, color);
        if (v > 0) sum += v / (used + 2);
    }

    return sum;
}

int evaluateBoard_2023180023(int rootColor) {
    int other = enemy_2023180023(rootColor);

    long long myScore = fastColorRunScore_2023180023(rootColor) + bestPotential_2023180023(rootColor);
    long long enemyScore = fastColorRunScore_2023180023(other) + bestPotential_2023180023(other);

    long long result;
    if (rootColor == BLACK_2023180023) {
        result = myScore * 100 / 100 - enemyScore * 140 / 100;
    }
    else {
        result = myScore * 125 / 100 - enemyScore * 105 / 100;
    }

    return clampScore_2023180023(result);
}

bool ttLookup_2023180023(unsigned long long key, int depth, int color, int rootColor, int alpha, int beta, int& value) {
    TTEntry_2023180023& e = tt_2023180023[key & (TT_SIZE_2023180023 - 1)];
    if (e.key != key || e.depth < depth || e.color != color || e.rootColor != rootColor) return false;

    if (e.flag == 0) {
        value = e.value;
        return true;
    }
    if (e.flag == 1 && e.value >= beta) {
        value = e.value;
        return true;
    }
    if (e.flag == 2 && e.value <= alpha) {
        value = e.value;
        return true;
    }
    return false;
}

void ttStore_2023180023(unsigned long long key, int depth, int color, int rootColor, int value, int flag) {
    TTEntry_2023180023& e = tt_2023180023[key & (TT_SIZE_2023180023 - 1)];
    if (e.key == 0 || depth >= e.depth) {
        e.key = key;
        e.depth = depth;
        e.color = color;
        e.rootColor = rootColor;
        e.value = value;
        e.flag = flag;
    }
}

int minimax_2023180023(int depth, int color, int rootColor, int alpha, int beta, int ply = 0) {
    if (timeout_2023180023()) return evaluateBoard_2023180023(rootColor);
    if (depth <= 0) return evaluateBoard_2023180023(rootColor);

    bool maximizing = (color == rootColor);
    int other = enemy_2023180023(color);
    int alphaStart = alpha;
    int betaStart = beta;
    unsigned long long key = boardHash_2023180023() ^ mix64_2023180023((unsigned long long)(depth * 17 + colorIndex_2023180023(color) * 3 + colorIndex_2023180023(rootColor)));
    int ttValue = 0;
    if (ttLookup_2023180023(key, depth, color, rootColor, alpha, beta, ttValue)) return ttValue;

    std::vector<Move_2023180023> myWins = immediateWinMoves_2023180023(color, 2);
    if (!myWins.empty()) {
        return maximizing ? (WIN_SCORE_2023180023 - depth) : (-WIN_SCORE_2023180023 + depth);
    }

    std::vector<Move_2023180023> enemyWins = immediateWinMoves_2023180023(other, 3);
    std::vector<Move_2023180023> moves;

    if (!enemyWins.empty()) {
        if (enemyWins.size() >= 2) {
            return maximizing ? (-WIN_SCORE_2023180023 + depth) : (WIN_SCORE_2023180023 - depth);
        }

        for (size_t i = 0; i < enemyWins.size(); i++) {
            int x = enemyWins[i].x;
            int y = enemyWins[i].y;
            if (!canPlace_2023180023(x, y, color)) continue;
            Move_2023180023 block;
            block.x = x;
            block.y = y;
            block.score = evaluateMove_2023180023(x, y, color);
            moves.push_back(block);
        }

        if (moves.empty()) {
            return maximizing ? (-WIN_SCORE_2023180023 + depth) : (WIN_SCORE_2023180023 - depth);
        }
    }
    else {
        moves = generateMoves_2023180023(color, depth >= 3 ? 9 : 12, false);
    }

    if (moves.empty()) return evaluateBoard_2023180023(rootColor);
    orderMovesForSearch_2023180023(moves, color, ply);

    if (maximizing) {
        int best = -INF_2023180023;
        int bestX = -1;
        int bestY = -1;

        for (size_t i = 0; i < moves.size(); i++) {
            if (timeout_2023180023()) break;
            int x = moves[i].x;
            int y = moves[i].y;

            board_2023180023[y][x] = color;
            int value = isWin_2023180023(x, y, color)
                ? WIN_SCORE_2023180023 - (4 - depth)
                : minimax_2023180023(depth - 1, enemy_2023180023(color), rootColor, alpha, beta, ply + 1);
            board_2023180023[y][x] = EMPTY_2023180023;

            if (value > best) {
                best = value;
                bestX = x;
                bestY = y;
            }
            if (value > alpha) alpha = value;
            if (alpha >= beta) {
                recordKiller_2023180023(ply, x, y);
                historyScore_2023180023[colorIndex_2023180023(color)][y][x] += depth * depth * 256;
                break;
            }
        }

        int flag = 0;
        if (best <= alphaStart) flag = 2;
        else if (best >= betaStart) flag = 1;
        if (!timeout_2023180023()) ttStore_2023180023(key, depth, color, rootColor, best, flag);
        if (bestX >= 0) historyScore_2023180023[colorIndex_2023180023(color)][bestY][bestX] += depth * 64;
        return best;
    }

    int best = INF_2023180023;
    int bestX = -1;
    int bestY = -1;

    for (size_t i = 0; i < moves.size(); i++) {
        if (timeout_2023180023()) break;
        int x = moves[i].x;
        int y = moves[i].y;

        board_2023180023[y][x] = color;
        int value = isWin_2023180023(x, y, color)
            ? -WIN_SCORE_2023180023 + (4 - depth)
            : minimax_2023180023(depth - 1, enemy_2023180023(color), rootColor, alpha, beta, ply + 1);
        board_2023180023[y][x] = EMPTY_2023180023;

        if (value < best) {
            best = value;
            bestX = x;
            bestY = y;
        }
        if (value < beta) beta = value;
        if (alpha >= beta) {
            recordKiller_2023180023(ply, x, y);
            historyScore_2023180023[colorIndex_2023180023(color)][y][x] += depth * depth * 256;
            break;
        }
    }

    int flag = 0;
    if (best <= alphaStart) flag = 2;
    else if (best >= betaStart) flag = 1;
    if (!timeout_2023180023()) ttStore_2023180023(key, depth, color, rootColor, best, flag);
    if (bestX >= 0) historyScore_2023180023[colorIndex_2023180023(color)][bestY][bestX] += depth * 64;
    return best;
}

Move_2023180023 fallbackMoveNoWrite_2023180023(int color) {
    Move_2023180023 best;
    best.x = 9;
    best.y = 9;
    best.score = -INF_2023180023;

    for (int radius = 0; radius <= 18; radius++) {
        for (int y = 9 - radius; y <= 9 + radius; y++) {
            for (int x = 9 - radius; x <= 9 + radius; x++) {
                if (!inBoard_2023180023(x, y)) continue;
                if (std::abs(x - 9) + std::abs(y - 9) > radius) continue;
                if (canPlace_2023180023(x, y, color)) {
                    best.x = x;
                    best.y = y;
                    best.score = centerScore_2023180023(x, y);
                    return best;
                }
            }
        }
    }

    for (int y = 0; y < SIZE_2023180023; y++) {
        for (int x = 0; x < SIZE_2023180023; x++) {
            if (board_2023180023[y][x] == EMPTY_2023180023) {
                best.x = x;
                best.y = y;
                best.score = -INF_2023180023 / 2;
                return best;
            }
        }
    }

    best.x = 0;
    best.y = 0;
    best.score = 0;
    return best;
}

Move_2023180023 chooseMoveBySearch_2023180023(int color) {
    Move_2023180023 best = fallbackMoveNoWrite_2023180023(color);
    std::vector<Move_2023180023> moves = generateMoves_2023180023(color, 16, true);

    if (moves.empty()) return best;
    orderMovesForSearch_2023180023(moves, color, 0);
    best = moves[0];

    int maxDepth = 6;

    for (int depth = 2; depth <= maxDepth; depth += 2) {
        if (timeout_2023180023()) break;

        Move_2023180023 depthBest = best;
        int depthBestScore = -INF_2023180023;
        int alpha = -INF_2023180023;
        int beta = INF_2023180023;

        for (size_t i = 0; i < moves.size(); i++) {
            if (timeout_2023180023()) break;

            int x = moves[i].x;
            int y = moves[i].y;

            board_2023180023[y][x] = color;
            int value = isWin_2023180023(x, y, color)
                ? WIN_SCORE_2023180023
                : minimax_2023180023(depth - 1, enemy_2023180023(color), color, alpha, beta);
            board_2023180023[y][x] = EMPTY_2023180023;

            value += moves[i].score / 1000;

            if (value > depthBestScore) {
                depthBestScore = value;
                depthBest = moves[i];
                depthBest.score = value;
            }
            if (value > alpha) alpha = value;
        }

        if (!timeout_2023180023()) best = depthBest;
    }

    return best;
}

bool tryOpening_2023180023(int color, Move_2023180023& best) {
    int stones = totalStoneCount_2023180023();

    if (stones == 0 && canPlace_2023180023(9, 9, color)) {
        best.x = 9;
        best.y = 9;
        best.score = WIN_SCORE_2023180023;
        return true;
    }

    if (stones == 1) {
        int whiteOrder[8][2] = {
            { 10, 8 }, { 8, 10 }, { 10, 10 }, { 7, 7 },
            { 9, 8 }, { 8, 9 }, { 10, 9 }, { 9, 10 }
        };
        int blackOrder[8][2] = {
            { 8, 8 }, { 10, 10 }, { 8, 10 }, { 10, 8 },
            { 9, 8 }, { 8, 9 }, { 10, 9 }, { 9, 10 }
        };
        int(*order)[2] = (color == WHITE_2023180023) ? whiteOrder : blackOrder;

        for (int i = 0; i < 8; i++) {
            int x = order[i][0];
            int y = order[i][1];
            if (canPlace_2023180023(x, y, color)) {
                best.x = x;
                best.y = y;
                best.score = 300000000;
                return true;
            }
        }
    }

    if (color == BLACK_2023180023 && stones == 2) {
        int selfLikeOrder[12][2] = {
            { 10, 11 }, { 8, 11 }, { 8, 7 }, { 9, 7 },
            { 8, 8 }, { 11, 11 }, { 7, 9 }, { 7, 10 },
            { 11, 10 }, { 7, 8 }, { 7, 7 }, { 11, 9 }
        };
        int sideOrder[12][2] = {
            { 10, 8 }, { 8, 8 }, { 10, 7 }, { 8, 11 },
            { 7, 8 }, { 11, 10 }, { 8, 7 }, { 10, 11 },
            { 9, 7 }, { 7, 9 }, { 9, 11 }, { 11, 9 }
        };
        int lowLeftOrder[12][2] = {
            { 10, 8 }, { 9, 7 }, { 8, 8 }, { 7, 10 },
            { 10, 7 }, { 8, 11 }, { 7, 8 }, { 11, 10 },
            { 8, 7 }, { 10, 11 }, { 7, 9 }, { 11, 9 }
        };
        int highRightOrder[12][2] = {
            { 11, 8 }, { 12, 10 }, { 10, 8 }, { 11, 10 },
            { 8, 10 }, { 10, 7 }, { 12, 9 }, { 8, 8 },
            { 7, 10 }, { 9, 7 }, { 12, 11 }, { 7, 9 }
        };
        int fallbackOrder[12][2] = {
            { 11, 10 }, { 8, 8 }, { 10, 8 }, { 8, 10 },
            { 10, 11 }, { 7, 10 }, { 10, 7 }, { 8, 11 },
            { 8, 7 }, { 7, 8 }, { 11, 11 }, { 7, 7 }
        };

        int(*order)[2] = fallbackOrder;
        if (inBoard_2023180023(10, 8) && board_2023180023[8][10] == WHITE_2023180023) {
            order = selfLikeOrder;
        }
        else if (inBoard_2023180023(9, 8) && board_2023180023[8][9] == WHITE_2023180023) {
            order = sideOrder;
        }
        else if (inBoard_2023180023(8, 7) && board_2023180023[7][8] == WHITE_2023180023) {
            order = lowLeftOrder;
        }
        else if (inBoard_2023180023(11, 9) && board_2023180023[9][11] == WHITE_2023180023) {
            order = highRightOrder;
        }

        for (int i = 0; i < 12; i++) {
            int x = order[i][0];
            int y = order[i][1];
            if (canPlace_2023180023(x, y, color)) {
                best.x = x;
                best.y = y;
                best.score = 250000000;
                return true;
            }
        }
    }

    if (color == WHITE_2023180023 && stones == 3) {
        int order[12][2] = {
            { 10, 10 }, { 10, 9 }, { 12, 10 }, { 12, 9 },
            { 9, 10 }, { 8, 9 }, { 11, 8 }, { 12, 11 },
            { 8, 8 }, { 9, 11 }, { 10, 7 }, { 8, 11 }
        };

        for (int i = 0; i < 12; i++) {
            int x = order[i][0];
            int y = order[i][1];
            if (canPlace_2023180023(x, y, color)) {
                best.x = x;
                best.y = y;
                best.score = 220000000;
                return true;
            }
        }
    }

    if (color == WHITE_2023180023 && stones == 7) {
        if (board_2023180023[9][9] == BLACK_2023180023 &&
            board_2023180023[6][10] == BLACK_2023180023 &&
            board_2023180023[11][10] == BLACK_2023180023 &&
            board_2023180023[7][9] == BLACK_2023180023 &&
            board_2023180023[8][10] == WHITE_2023180023 &&
            board_2023180023[10][10] == WHITE_2023180023 &&
            board_2023180023[9][11] == WHITE_2023180023 &&
            canPlace_2023180023(9, 11, color)) {
            best.x = 9;
            best.y = 11;
            best.score = 210000000;
            return true;
        }
    }

    return false;
}

Move_2023180023 chooseBlackMove_2023180023() {
    Move_2023180023 best = fallbackMoveNoWrite_2023180023(BLACK_2023180023);

    if (tryOpening_2023180023(BLACK_2023180023, best)) return best;

    if (findImmediateWin_2023180023(BLACK_2023180023, best)) return best;

    Move_2023180023 block;
    if (findImmediateWin_2023180023(WHITE_2023180023, block) && canPlace_2023180023(block.x, block.y, BLACK_2023180023)) {
        block.score = 860000000;
        return block;
    }

    Move_2023180023 enemyFour;
    if (findBlockThreatMove_2023180023(BLACK_2023180023, 1, enemyFour)) {
        enemyFour.score = 760000000;
        return enemyFour;
    }

    Move_2023180023 forbiddenTrap;
    if (findForbiddenBlockTrap_2023180023(BLACK_2023180023, forbiddenTrap)) {
        forbiddenTrap.score = 745000000;
        return forbiddenTrap;
    }

    Move_2023180023 enemyDouble;
    if (findBlockThreatMove_2023180023(BLACK_2023180023, 2, enemyDouble)) {
        enemyDouble.score = 730000000;
        return enemyDouble;
    }

    Move_2023180023 tssMove;
    if (findThreatSpaceMove_2023180023(BLACK_2023180023, tssMove)) {
        tssMove.score += 15000000;
        return tssMove;
    }

    Move_2023180023 enemyLatent;
    if (findLatentThreatBlock_2023180023(BLACK_2023180023, 220000000LL, enemyLatent)) {
        enemyLatent.score = 705000000;
        return enemyLatent;
    }

    Move_2023180023 safety;
    if (findSafetyMove_2023180023(BLACK_2023180023, 300000000LL, safety)) {
        safety.score += 4000000;
        return safety;
    }

    Move_2023180023 myFour;
    if (findThreatMove_2023180023(BLACK_2023180023, 1, myFour)) {
        myFour.score += 5000000;
        return myFour;
    }

    Move_2023180023 myDouble;
    if (findThreatMove_2023180023(BLACK_2023180023, 2, myDouble)) {
        myDouble.score += 3500000;
        return myDouble;
    }

    return chooseMoveBySearch_2023180023(BLACK_2023180023);
}

Move_2023180023 chooseWhiteMove_2023180023() {
    Move_2023180023 best = fallbackMoveNoWrite_2023180023(WHITE_2023180023);

    if (tryOpening_2023180023(WHITE_2023180023, best)) return best;

    if (findImmediateWin_2023180023(WHITE_2023180023, best)) return best;

    Move_2023180023 block;
    if (findImmediateWin_2023180023(BLACK_2023180023, block) && canPlace_2023180023(block.x, block.y, WHITE_2023180023)) {
        block.score = 850000000;
        return block;
    }

    Move_2023180023 myFour;
    if (findThreatMove_2023180023(WHITE_2023180023, 1, myFour)) {
        myFour.score += 7000000;
        return myFour;
    }

    Move_2023180023 tssMove;
    if (findThreatSpaceMove_2023180023(WHITE_2023180023, tssMove)) {
        tssMove.score += 22000000;
        return tssMove;
    }

    Move_2023180023 enemyFour;
    if (findBlockThreatMove_2023180023(WHITE_2023180023, 1, enemyFour)) {
        enemyFour.score = 710000000;
        return enemyFour;
    }

    Move_2023180023 safety;
    if (findSafetyMove_2023180023(WHITE_2023180023, 300000000LL, safety)) {
        safety.score += 3000000;
        return safety;
    }

    Move_2023180023 enemyDouble;
    if (findBlockThreatMove_2023180023(WHITE_2023180023, 2, enemyDouble)) {
        enemyDouble.score = 650000000;
        return enemyDouble;
    }

    Move_2023180023 myDouble;
    if (findThreatMove_2023180023(WHITE_2023180023, 2, myDouble)) {
        myDouble.score += 9000000;
        return myDouble;
    }

    Move_2023180023 myThree;
    if (findThreatMove_2023180023(WHITE_2023180023, 3, myThree)) {
        myThree.score += 2500000;
        return myThree;
    }

    return chooseMoveBySearch_2023180023(WHITE_2023180023);
}

void safeFallback_2023180023(int color, int* x, int* y) {
    Move_2023180023 best = fallbackMoveNoWrite_2023180023(color);
    *x = best.x;
    *y = best.y;

    if (canPlace_2023180023(*x, *y, color)) {
        board_2023180023[*y][*x] = color;
    }
    else if (inBoard_2023180023(*x, *y) && board_2023180023[*y][*x] == EMPTY_2023180023) {
        board_2023180023[*y][*x] = color;
    }
}

void BlackAttack_2023180023(int* x, int* y) {
    startTime_2023180023 = std::chrono::high_resolution_clock::now();
    resetSearchTables_2023180023();

    Move_2023180023 move = chooseBlackMove_2023180023();
    *x = move.x;
    *y = move.y;

    if (canPlace_2023180023(*x, *y, BLACK_2023180023)) {
        board_2023180023[*y][*x] = BLACK_2023180023;
    }
    else {
        safeFallback_2023180023(BLACK_2023180023, x, y);
    }
}

void BlackDefence_2023180023(int x, int y) {
    if (!inBoard_2023180023(x, y)) return;
    if (board_2023180023[y][x] != EMPTY_2023180023) return;
    board_2023180023[y][x] = WHITE_2023180023;
}

void WhiteAttack_2023180023(int* x, int* y) {
    startTime_2023180023 = std::chrono::high_resolution_clock::now();
    resetSearchTables_2023180023();

    Move_2023180023 move = chooseWhiteMove_2023180023();
    *x = move.x;
    *y = move.y;

    if (canPlace_2023180023(*x, *y, WHITE_2023180023)) {
        board_2023180023[*y][*x] = WHITE_2023180023;
    }
    else {
        safeFallback_2023180023(WHITE_2023180023, x, y);
    }
}

void WhiteDefence_2023180023(int x, int y) {
    if (!inBoard_2023180023(x, y)) return;
    if (board_2023180023[y][x] != EMPTY_2023180023) return;
    board_2023180023[y][x] = BLACK_2023180023;
}
