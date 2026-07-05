#pragma once

#include <algorithm>
#include <chrono>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace OmokBattleAIImproved_2023184007 {
namespace {

struct BattleAIStrategy_2023184007 {
    int attackWeightBlack = 110;
    int attackWeightWhite = 93;
    int defenceWeightBlack = 101;
    int defenceWeightWhite = 110;

    int openFourScore = 10506109;
    int closedFourScore = 1000000;
    int fourThreeScore = 776175;
    int openThreeScore = 115150;
    int brokenThreeScore = 50000;
    int openTwoScore = 3635;

    int centerWeightPercent = 100;
    int neighborWeightPercent = 100;
    int threatPriorityPercent = 81;
    int defenceUrgencyPercent = 100;

    int maxCandidates = 43;
    int searchCandidates = 8;
    int rootSearchCandidates = 10;
    int maxSearchDepth = 3;
    int moveTimeLimitMs = 500;
    int timeSafetyMarginMs = 260;
};

struct BattleAISearchStats_2023184007 {
    int requestedBudgetMs = 0;
    int completedDepth = 0;
    int generatedCandidates = 0;
    int rootCandidates = 0;
    long long searchedNodes = 0;
    int decisionCode = 0;
};

BattleAIStrategy_2023184007 DefaultBattleAIStrategy_2023184007();
void SetBattleAIStrategy_2023184007(int slot, const BattleAIStrategy_2023184007& strategy);
void ResetBattleAIInstance_2023184007(int slot);
std::pair<int, int> ChooseBattleAIInstanceMove_2023184007(int slot, int color);
void CommitBattleAIInstanceMove_2023184007(int slot, int x, int y, int color);
int DiagnoseBattleAIInstanceMove_2023184007(int slot, int x, int y, int color);
int TacticalRankBattleAIInstanceMove_2023184007(int slot, int x, int y, int color);
int ScoreBattleAIInstanceMove_2023184007(int slot, int x, int y, int color);
BattleAISearchStats_2023184007 GetBattleAIInstanceSearchStats_2023184007(int slot);

void ResetBattleAI_2023184007();

std::pair<int, int> ChooseBattleAIMove_2023184007(int color);
int ScoreBattleAIMove_2023184007(int x, int y, int color);
void CommitBattleAIMove_2023184007(int x, int y, int color);

void BlackAttack_2023184007(int* x, int* y);
void BlackDefence_2023184007(int x, int y);

void WhiteAttack_2023184007(int* x, int* y);
void WhiteDefence_2023184007(int x, int y);

using namespace std;
using namespace std::chrono;

static const int BOARD_SIZE_2023184007 = 19;
static const int EMPTY_2023184007 = 0;
static const int BLACK_2023184007 = 1;
static const int WHITE_2023184007 = 2;

static const int FIVE_2023184007 = 100000000;
static const int INF_SCORE_2023184007 = 1000000000;

static const int DIRS_2023184007[4][2] = {
    {1, 0}, {0, 1}, {1, 1}, {1, -1}
};

struct TTEntry_2023184007 {
    int depth;
    int extensions;
    int value;
    int flag;
};

struct BattleAIState_2023184007 {
    int board_2023184007[BOARD_SIZE_2023184007][BOARD_SIZE_2023184007] = {};
    uint64_t boardHash_2023184007 = 0;
    steady_clock::time_point deadline;
    unordered_map<unsigned long long, TTEntry_2023184007> transpositionTable;
    unordered_map<unsigned long long, int> evalCache;
    BattleAIStrategy_2023184007 strategy;
    BattleAISearchStats_2023184007 lastSearchStats;
};

// 슬롯별 보드와 탐색 상태를 분리해 후보 AI끼리도 한 프로세스에서 대결할 수 있다.
static BattleAIState_2023184007 g_states_2023184007[2];
static BattleAIState_2023184007* g_activeState_2023184007 = &g_states_2023184007[0];

#define board_2023184007 (g_activeState_2023184007->board_2023184007)
#define g_boardHash_2023184007 (g_activeState_2023184007->boardHash_2023184007)
#define g_deadline_2023184007 (g_activeState_2023184007->deadline)
#define g_transpositionTable_2023184007 (g_activeState_2023184007->transpositionTable)
#define g_evalCache_2023184007 (g_activeState_2023184007->evalCache)
#define g_strategy_2023184007 (g_activeState_2023184007->strategy)

static const int TT_EXACT_2023184007 = 0;
static const int TT_LOWER_2023184007 = 1;
static const int TT_UPPER_2023184007 = 2;

static int opponentOf_2023184007(int color)
{
    return color == BLACK_2023184007 ? WHITE_2023184007 : BLACK_2023184007;
}

static bool timeUp_2023184007()
{
    return steady_clock::now() >= g_deadline_2023184007;
}

static int thinkTimeForColor_2023184007(int color)
{
    (void)color;
    const int budget = g_strategy_2023184007.moveTimeLimitMs - g_strategy_2023184007.timeSafetyMarginMs;
    return max(20, min(450, budget));
}

static int searchCandidateLimit_2023184007(int depth)
{
    if (depth >= 4) {
        return max(2, g_strategy_2023184007.searchCandidates - 2);
    }
    if (depth == 3) {
        return max(2, g_strategy_2023184007.searchCandidates - 1);
    }
    return max(2, g_strategy_2023184007.searchCandidates);
}

static uint64_t splitMix64_2023184007(uint64_t& seed)
{
    seed += 0x9e3779b97f4a7c15ULL;
    uint64_t z = seed;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

static uint64_t zobristStone_2023184007(int colorIndex, int y, int x)
{
    static bool initialized = false;
    static uint64_t table[2][BOARD_SIZE_2023184007][BOARD_SIZE_2023184007];

    if (!initialized) {
        uint64_t seed = 0x2023184014ULL;
        for (int c = 0; c < 2; ++c) {
            for (int yy = 0; yy < BOARD_SIZE_2023184007; ++yy) {
                for (int xx = 0; xx < BOARD_SIZE_2023184007; ++xx) {
                    table[c][yy][xx] = splitMix64_2023184007(seed);
                }
            }
        }
        initialized = true;
    }

    return table[colorIndex][y][x];
}

static uint64_t boardHash_2023184007()
{
    return g_boardHash_2023184007;
}

static uint64_t transpositionKey_2023184007(int color, int depth, int extensions)
{
    uint64_t hash = boardHash_2023184007();
    hash ^= static_cast<uint64_t>(color) * 0x9e3779b97f4a7c15ULL;
    hash ^= static_cast<uint64_t>(depth + 17) * 0xbf58476d1ce4e5b9ULL;
    hash ^= static_cast<uint64_t>(extensions + 31) * 0x94d049bb133111ebULL;
    return hash;
}

static uint64_t evalCacheKey_2023184007(int kind, int x, int y, int color)
{
    uint64_t key = boardHash_2023184007();
    key ^= static_cast<uint64_t>(kind + 101) * 0x9e3779b97f4a7c15ULL;
    key ^= static_cast<uint64_t>((x + 1) * 23 + (y + 1) * 977) * 0xbf58476d1ce4e5b9ULL;
    key ^= static_cast<uint64_t>(color + 31) * 0x94d049bb133111ebULL;
    return key;
}

static bool findCachedEval_2023184007(int kind, int x, int y, int color, int& value)
{
    auto it = g_evalCache_2023184007.find(evalCacheKey_2023184007(kind, x, y, color));
    if (it == g_evalCache_2023184007.end()) {
        return false;
    }
    value = it->second;
    return true;
}

static void storeCachedEval_2023184007(int kind, int x, int y, int color, int value)
{
    if (g_evalCache_2023184007.size() > 200000) {
        g_evalCache_2023184007.clear();
    }
    g_evalCache_2023184007[evalCacheKey_2023184007(kind, x, y, color)] = value;
}

bool inRange_2023184007(int x, int y)
{
    return 0 <= x && x < BOARD_SIZE_2023184007 && 0 <= y && y < BOARD_SIZE_2023184007;
}

static void setBoardCell_2023184007(int x, int y, int color)
{
    if (!inRange_2023184007(x, y)) {
        return;
    }

    int old = board_2023184007[y][x];
    if (old == color) {
        return;
    }
    if (old == BLACK_2023184007) {
        g_boardHash_2023184007 ^= zobristStone_2023184007(0, y, x);
    } else if (old == WHITE_2023184007) {
        g_boardHash_2023184007 ^= zobristStone_2023184007(1, y, x);
    }

    board_2023184007[y][x] = color;

    if (color == BLACK_2023184007) {
        g_boardHash_2023184007 ^= zobristStone_2023184007(0, y, x);
    } else if (color == WHITE_2023184007) {
        g_boardHash_2023184007 ^= zobristStone_2023184007(1, y, x);
    }
}

static void clearBoardCell_2023184007(int x, int y)
{
    setBoardCell_2023184007(x, y, EMPTY_2023184007);
}

bool isEmpty_2023184007(int x, int y)
{
    return inRange_2023184007(x, y) && board_2023184007[y][x] == EMPTY_2023184007;
}

void placeMove_2023184007(int x, int y, int color)
{
    if (inRange_2023184007(x, y)) {
        setBoardCell_2023184007(x, y, color);
    }
}

void undoMove_2023184007(int x, int y)
{
    if (inRange_2023184007(x, y)) {
        clearBoardCell_2023184007(x, y);
    }
}

int countLine_2023184007(int x, int y, int dx, int dy, int color)
{
    int count = 0;
    int nx = x + dx;
    int ny = y + dy;

    while (inRange_2023184007(nx, ny) && board_2023184007[ny][nx] == color) {
        ++count;
        nx += dx;
        ny += dy;
    }
    return count;
}

static int lineLengthAt_2023184007(int x, int y, int dx, int dy, int color)
{
    return 1 + countLine_2023184007(x, y, dx, dy, color) + countLine_2023184007(x, y, -dx, -dy, color);
}

int maxLineAfterMove_2023184007(int x, int y, int color)
{
    if (!inRange_2023184007(x, y)) {
        return 0;
    }

    bool placed = false;
    if (board_2023184007[y][x] == EMPTY_2023184007) {
        setBoardCell_2023184007(x, y, color);
        placed = true;
    } else if (board_2023184007[y][x] != color) {
        return 0;
    }

    int best = 0;
    for (int d = 0; d < 4; ++d) {
        best = max(best, lineLengthAt_2023184007(x, y, DIRS_2023184007[d][0], DIRS_2023184007[d][1], color));
    }

    if (placed) {
        clearBoardCell_2023184007(x, y);
    }
    return best;
}

bool isFive_2023184007(int x, int y, int color)
{
    if (!inRange_2023184007(x, y)) {
        return false;
    }

    bool placed = false;
    if (board_2023184007[y][x] == EMPTY_2023184007) {
        setBoardCell_2023184007(x, y, color);
        placed = true;
    } else if (board_2023184007[y][x] != color) {
        return false;
    }

    bool result = false;
    for (int d = 0; d < 4; ++d) {
        if (lineLengthAt_2023184007(x, y, DIRS_2023184007[d][0], DIRS_2023184007[d][1], color) == 5) {
            result = true;
            break;
        }
    }

    if (placed) {
        clearBoardCell_2023184007(x, y);
    }
    return result;
}

bool isOverline_2023184007(int x, int y, int color)
{
    if (!inRange_2023184007(x, y)) {
        return false;
    }

    bool placed = false;
    if (board_2023184007[y][x] == EMPTY_2023184007) {
        setBoardCell_2023184007(x, y, color);
        placed = true;
    } else if (board_2023184007[y][x] != color) {
        return false;
    }

    bool result = false;
    for (int d = 0; d < 4; ++d) {
        if (lineLengthAt_2023184007(x, y, DIRS_2023184007[d][0], DIRS_2023184007[d][1], color) >= 6) {
            result = true;
            break;
        }
    }

    if (placed) {
        clearBoardCell_2023184007(x, y);
    }
    return result;
}

static bool makesExactFiveByPlaying_2023184007(int x, int y, int color)
{
    if (!isEmpty_2023184007(x, y)) {
        return false;
    }

    setBoardCell_2023184007(x, y, color);
    bool result = false;
    if (color == BLACK_2023184007) {
        result = isFive_2023184007(x, y, color) && !isOverline_2023184007(x, y, color);
    } else {
        result = maxLineAfterMove_2023184007(x, y, color) >= 5;
    }
    clearBoardCell_2023184007(x, y);
    return result;
}

static bool isWinningStone_2023184007(int x, int y, int color)
{
    if (!inRange_2023184007(x, y) || board_2023184007[y][x] != color) {
        return false;
    }
    if (color == BLACK_2023184007) {
        return isFive_2023184007(x, y, color) && !isOverline_2023184007(x, y, color);
    }
    return maxLineAfterMove_2023184007(x, y, color) >= 5;
}

static string lineStringAround_2023184007(int x, int y, int dx, int dy, int color)
{
    string s;
    s.reserve(11);

    for (int k = -5; k <= 5; ++k) {
        int nx = x + dx * k;
        int ny = y + dy * k;

        if (!inRange_2023184007(nx, ny)) {
            s.push_back('O');
        } else if (board_2023184007[ny][nx] == color) {
            s.push_back('X');
        } else if (board_2023184007[ny][nx] == EMPTY_2023184007) {
            s.push_back('.');
        } else {
            s.push_back('O');
        }
    }
    return s;
}

static bool hasPatternThroughCenter_2023184007(const string& s, const string& pattern)
{
    const int center = 5;
    const int n = static_cast<int>(s.size());
    const int m = static_cast<int>(pattern.size());

    for (int i = 0; i + m <= n; ++i) {
        if (!(i <= center && center < i + m)) {
            continue;
        }
        bool matched = true;
        for (int j = 0; j < m; ++j) {
            if (s[i + j] != pattern[j]) {
                matched = false;
                break;
            }
        }
        if (matched) {
            return true;
        }
    }
    return false;
}

static int countWinningSpotsInDirection_2023184007(int x, int y, int dx, int dy, int color)
{
    int count = 0;

    for (int k = -4; k <= 4; ++k) {
        int nx = x + dx * k;
        int ny = y + dy * k;

        if (!isEmpty_2023184007(nx, ny)) {
            continue;
        }

        setBoardCell_2023184007(nx, ny, color);
        int length = lineLengthAt_2023184007(nx, ny, dx, dy, color);
        clearBoardCell_2023184007(nx, ny);

        if ((color == BLACK_2023184007 && length == 5) || (color == WHITE_2023184007 && length >= 5)) {
            ++count;
        }
    }
    return count;
}

struct ShapeStats_2023184007 {
    int five = 0;
    int overline = 0;
    int openFour = 0;
    int closedFour = 0;
    int openThree = 0;
    int brokenThree = 0;
    int openTwo = 0;
    int fourDirs = 0;
};

static ShapeStats_2023184007 analyzePlacedStone_2023184007(int x, int y, int color)
{
    ShapeStats_2023184007 st;
    if (!inRange_2023184007(x, y) || board_2023184007[y][x] != color) {
        return st;
    }

    for (int d = 0; d < 4; ++d) {
        int dx = DIRS_2023184007[d][0];
        int dy = DIRS_2023184007[d][1];
        int len = lineLengthAt_2023184007(x, y, dx, dy, color);

        if (len == 5) {
            ++st.five;
        }
        if (len >= 6) {
            ++st.overline;
        }

        int winningSpots = countWinningSpotsInDirection_2023184007(x, y, dx, dy, color);
        if (winningSpots >= 2) {
            ++st.openFour;
            ++st.fourDirs;
            continue;
        }
        if (winningSpots == 1) {
            ++st.closedFour;
            ++st.fourDirs;
            continue;
        }

        string s = lineStringAround_2023184007(x, y, dx, dy, color);
        if (hasPatternThroughCenter_2023184007(s, ".XXX.")) {
            ++st.openThree;
        } else if (hasPatternThroughCenter_2023184007(s, ".XX.X.") ||
                   hasPatternThroughCenter_2023184007(s, ".X.XX.") ||
                   hasPatternThroughCenter_2023184007(s, ".X.X.")) {
            ++st.brokenThree;
        } else if (hasPatternThroughCenter_2023184007(s, ".XX.")) {
            ++st.openTwo;
        }
    }

    return st;
}

static int countDistinctFoursAfterPlaced_2023184007(int x, int y, int color)
{
    set<string> signatures;
    for (int d = 0; d < 4; ++d) {
        int dx = DIRS_2023184007[d][0];
        int dy = DIRS_2023184007[d][1];
        for (int k = -4; k <= 4; ++k) {
            int wx = x + dx * k;
            int wy = y + dy * k;
            if (!isEmpty_2023184007(wx, wy)) {
                continue;
            }
            setBoardCell_2023184007(wx, wy, color);
            if (lineLengthAt_2023184007(wx, wy, dx, dy, color) == 5) {
                for (int start = -4; start <= 0; ++start) {
                    bool allStones = true;
                    bool containsWinningSpot = false;
                    string signature = to_string(d) + ":";
                    for (int offset = 0; offset < 5; ++offset) {
                        int px = x + dx * (start + offset);
                        int py = y + dy * (start + offset);
                        if (!inRange_2023184007(px, py) || board_2023184007[py][px] != color) {
                            allStones = false;
                            break;
                        }
                        if (px == wx && py == wy) {
                            containsWinningSpot = true;
                        } else {
                            signature += to_string(py * BOARD_SIZE_2023184007 + px) + ",";
                        }
                    }
                    if (allStones && containsWinningSpot) {
                        signatures.insert(signature);
                    }
                }
            }
            clearBoardCell_2023184007(wx, wy);
        }
    }
    return static_cast<int>(signatures.size());
}

bool isDoubleFour_2023184007(int x, int y)
{
    if (!inRange_2023184007(x, y)) {
        return false;
    }

    bool placed = false;
    if (board_2023184007[y][x] == EMPTY_2023184007) {
        setBoardCell_2023184007(x, y, BLACK_2023184007);
        placed = true;
    } else if (board_2023184007[y][x] != BLACK_2023184007) {
        return false;
    }

    ShapeStats_2023184007 st = analyzePlacedStone_2023184007(x, y, BLACK_2023184007);
    bool result = st.fourDirs >= 2 ||
        (st.fourDirs == 1 && countDistinctFoursAfterPlaced_2023184007(x, y, BLACK_2023184007) >= 2);

    if (placed) {
        clearBoardCell_2023184007(x, y);
    }
    return result;
}

bool isDoubleThree_2023184007(int x, int y)
{
    if (!inRange_2023184007(x, y)) {
        return false;
    }

    bool placed = false;
    if (board_2023184007[y][x] == EMPTY_2023184007) {
        setBoardCell_2023184007(x, y, BLACK_2023184007);
        placed = true;
    } else if (board_2023184007[y][x] != BLACK_2023184007) {
        return false;
    }

    ShapeStats_2023184007 st = analyzePlacedStone_2023184007(x, y, BLACK_2023184007);
    int threeDirs = st.openThree + st.brokenThree;
    bool result = threeDirs >= 2;

    if (placed) {
        clearBoardCell_2023184007(x, y);
    }
    return result;
}

bool isForbiddenBlackMove_2023184007(int x, int y)
{
    if (!isEmpty_2023184007(x, y)) {
        return true;
    }

    setBoardCell_2023184007(x, y, BLACK_2023184007);
    bool over = isOverline_2023184007(x, y, BLACK_2023184007);
    bool exactFive = isFive_2023184007(x, y, BLACK_2023184007);
    bool doubleFour = false;
    bool doubleThree = false;

    if (!exactFive && !over) {
        doubleFour = isDoubleFour_2023184007(x, y);
        if (!doubleFour) {
            doubleThree = isDoubleThree_2023184007(x, y);
        }
    }

    clearBoardCell_2023184007(x, y);
    return over || doubleFour || doubleThree;
}

bool isLegalMove_2023184007(int x, int y, int color)
{
    if (!isEmpty_2023184007(x, y)) {
        return false;
    }

    if (color == BLACK_2023184007) {
        return !isForbiddenBlackMove_2023184007(x, y);
    }

    return true;
}

static bool isUniqueEmergencyBlackBlockLegal_2023184007(int x, int y)
{
    if (!isEmpty_2023184007(x, y)) {
        return false;
    }
    setBoardCell_2023184007(x, y, BLACK_2023184007);
    bool over = isOverline_2023184007(x, y, BLACK_2023184007);
    bool exactFive = isFive_2023184007(x, y, BLACK_2023184007);
    bool doubleFour = false;
    int realThreeDirections = 0;
    if (!exactFive && !over) {
        doubleFour = isDoubleFour_2023184007(x, y);
        if (!doubleFour) {
            for (int d = 0; d < 4; ++d) {
                int dx = DIRS_2023184007[d][0];
                int dy = DIRS_2023184007[d][1];
                if (countWinningSpotsInDirection_2023184007(x, y, dx, dy, BLACK_2023184007) > 0) {
                    continue;
                }
                string s = lineStringAround_2023184007(x, y, dx, dy, BLACK_2023184007);
                if (hasPatternThroughCenter_2023184007(s, ".XXX.") ||
                    hasPatternThroughCenter_2023184007(s, ".XX.X.") ||
                    hasPatternThroughCenter_2023184007(s, ".X.XX.")) {
                    ++realThreeDirections;
                }
            }
        }
    }
    clearBoardCell_2023184007(x, y);
    return !over && !doubleFour && realThreeDirections < 2;
}

static int positionalBonus_2023184007(int x, int y)
{
    int dx = x - 9;
    int dy = y - 9;
    int dist2 = dx * dx + dy * dy;
    return max(0, 180 - dist2 * 3) * g_strategy_2023184007.centerWeightPercent / 100;
}

static int neighborBonus_2023184007(int x, int y, int color)
{
    int score = 0;
    int opponent = opponentOf_2023184007(color);

    for (int dy = -2; dy <= 2; ++dy) {
        for (int dx = -2; dx <= 2; ++dx) {
            if (dx == 0 && dy == 0) {
                continue;
            }
            int nx = x + dx;
            int ny = y + dy;
            if (!inRange_2023184007(nx, ny)) {
                continue;
            }
            int distance = max(abs(dx), abs(dy));
            if (board_2023184007[ny][nx] == color) {
                score += distance == 1 ? 80 : 25;
            } else if (board_2023184007[ny][nx] == opponent) {
                score += distance == 1 ? 65 : 20;
            }
        }
    }
    return score * g_strategy_2023184007.neighborWeightPercent / 100;
}

static int evaluateRawMoveForColor_2023184007(int x, int y, int color)
{
    int cached = 0;
    if (findCachedEval_2023184007(1, x, y, color, cached)) {
        return cached;
    }

    if (!isLegalMove_2023184007(x, y, color)) {
        storeCachedEval_2023184007(1, x, y, color, -INF_SCORE_2023184007 / 4);
        return -INF_SCORE_2023184007 / 4;
    }

    setBoardCell_2023184007(x, y, color);
    ShapeStats_2023184007 st = analyzePlacedStone_2023184007(x, y, color);
    clearBoardCell_2023184007(x, y);

    if (color == BLACK_2023184007 && st.overline > 0) {
        storeCachedEval_2023184007(1, x, y, color, -INF_SCORE_2023184007 / 4);
        return -INF_SCORE_2023184007 / 4;
    }

    int score = 0;
    if (st.five > 0 || (color == WHITE_2023184007 && st.overline > 0)) {
        score += FIVE_2023184007;
    }
    score += st.openFour * g_strategy_2023184007.openFourScore;
    score += st.closedFour * g_strategy_2023184007.closedFourScore;

    if (st.fourDirs > 0 && (st.openThree + st.brokenThree) > 0) {
        score += g_strategy_2023184007.fourThreeScore;
    }
    if (st.fourDirs >= 2) {
        score += g_strategy_2023184007.fourThreeScore;
    }

    score += st.openThree * g_strategy_2023184007.openThreeScore;
    score += st.brokenThree * g_strategy_2023184007.brokenThreeScore;
    score += st.openTwo * g_strategy_2023184007.openTwoScore;
    score += positionalBonus_2023184007(x, y);
    score += neighborBonus_2023184007(x, y, color);

    storeCachedEval_2023184007(1, x, y, color, score);
    return score;
}

int evaluateMove_2023184007(int x, int y, int color)
{
    int cached = 0;
    if (findCachedEval_2023184007(2, x, y, color, cached)) {
        return cached;
    }

    if (!isLegalMove_2023184007(x, y, color)) {
        storeCachedEval_2023184007(2, x, y, color, -INF_SCORE_2023184007 / 4);
        return -INF_SCORE_2023184007 / 4;
    }

    int opponent = opponentOf_2023184007(color);
    int attackScore = evaluateRawMoveForColor_2023184007(x, y, color);
    int defenceScore = evaluateRawMoveForColor_2023184007(x, y, opponent);
    if (defenceScore < 0) {
        defenceScore = 0;
    }

    const int attackWeight = color == WHITE_2023184007
        ? g_strategy_2023184007.attackWeightWhite
        : g_strategy_2023184007.attackWeightBlack;
    const int defenceWeight = color == BLACK_2023184007
        ? g_strategy_2023184007.defenceWeightBlack
        : g_strategy_2023184007.defenceWeightWhite;
    int score = attackScore * attackWeight / 100 + defenceScore * defenceWeight / 100;
    storeCachedEval_2023184007(2, x, y, color, score);
    return score;
}

static int tacticalRankAfterMove_2023184007(int x, int y, int color)
{
    int cached = 0;
    if (findCachedEval_2023184007(3, x, y, color, cached)) {
        return cached;
    }

    if (!isLegalMove_2023184007(x, y, color)) {
        storeCachedEval_2023184007(3, x, y, color, -1);
        return -1;
    }

    setBoardCell_2023184007(x, y, color);
    ShapeStats_2023184007 st = analyzePlacedStone_2023184007(x, y, color);
    clearBoardCell_2023184007(x, y);

    if (color == BLACK_2023184007 && st.overline > 0) {
        storeCachedEval_2023184007(3, x, y, color, -1);
        return -1;
    }
    if (st.five > 0 || (color == WHITE_2023184007 && st.overline > 0)) {
        storeCachedEval_2023184007(3, x, y, color, 1000);
        return 1000;
    }
    if (st.openFour > 0) {
        storeCachedEval_2023184007(3, x, y, color, 900);
        return 900;
    }
    if (st.fourDirs >= 2 || (st.fourDirs > 0 && st.openThree + st.brokenThree > 0)) {
        storeCachedEval_2023184007(3, x, y, color, 850);
        return 850;
    }
    if (st.closedFour > 0) {
        storeCachedEval_2023184007(3, x, y, color, 800);
        return 800;
    }
    if (st.openThree + st.brokenThree >= 2) {
        storeCachedEval_2023184007(3, x, y, color, 650);
        return 650;
    }
    if (st.openThree > 0) {
        storeCachedEval_2023184007(3, x, y, color, 600);
        return 600;
    }
    if (st.brokenThree > 0) {
        storeCachedEval_2023184007(3, x, y, color, 550);
        return 550;
    }
    storeCachedEval_2023184007(3, x, y, color, 0);
    return 0;
}

struct ScoredMove_2023184007 {
    int x;
    int y;
    int score;
    bool critical;
};

static int candidateOrderingScore_2023184007(int x, int y, int color)
{
    int opponent = opponentOf_2023184007(color);
    int ownRank = tacticalRankAfterMove_2023184007(x, y, color);
    int opponentRank = tacticalRankAfterMove_2023184007(x, y, opponent);
    int score = evaluateMove_2023184007(x, y, color);

    if (ownRank >= 1000) {
        score += 90000000;
    } else if (ownRank >= 850) {
        score += 18000000;
    } else {
        score += ownRank * 12000 * g_strategy_2023184007.threatPriorityPercent / 100;
    }

    if (opponentRank >= 1000) {
        score += 85000000;
    } else if (opponentRank >= 850) {
        score += 16000000;
    } else {
        score += opponentRank * 14000 * g_strategy_2023184007.defenceUrgencyPercent / 100;
    }

    if (color == WHITE_2023184007 && ownRank >= 550) {
        score += 300000;
    }

    return score;
}

vector<pair<int, int>> generateCandidates_2023184007(int color)
{
    bool seen[BOARD_SIZE_2023184007][BOARD_SIZE_2023184007] = {};
    bool critical[BOARD_SIZE_2023184007][BOARD_SIZE_2023184007] = {};
    bool hasStone = false;

    auto mark = [&](int x, int y, bool isCritical) {
        if (!inRange_2023184007(x, y) || board_2023184007[y][x] != EMPTY_2023184007) {
            return;
        }
        if (!isLegalMove_2023184007(x, y, color)) {
            return;
        }
        seen[y][x] = true;
        if (isCritical) {
            critical[y][x] = true;
        }
    };

    for (int y = 0; y < BOARD_SIZE_2023184007; ++y) {
        for (int x = 0; x < BOARD_SIZE_2023184007; ++x) {
            if (board_2023184007[y][x] == EMPTY_2023184007) {
                continue;
            }
            hasStone = true;
            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -2; dx <= 2; ++dx) {
                    mark(x + dx, y + dy, false);
                }
            }
        }
    }

    if (!hasStone) {
        mark(9, 9, true);
        mark(9, 8, false);
        mark(8, 9, false);
    }

    int opponent = opponentOf_2023184007(color);
    for (int y = 0; y < BOARD_SIZE_2023184007; ++y) {
        for (int x = 0; x < BOARD_SIZE_2023184007; ++x) {
            if (!isEmpty_2023184007(x, y) || !isLegalMove_2023184007(x, y, color)) {
                continue;
            }
            if (makesExactFiveByPlaying_2023184007(x, y, color)) {
                mark(x, y, true);
            } else if (isLegalMove_2023184007(x, y, opponent) && makesExactFiveByPlaying_2023184007(x, y, opponent)) {
                mark(x, y, true);
            }
        }
    }

    vector<ScoredMove_2023184007> scored;
    scored.reserve(BOARD_SIZE_2023184007 * BOARD_SIZE_2023184007);

    for (int y = 0; y < BOARD_SIZE_2023184007; ++y) {
        for (int x = 0; x < BOARD_SIZE_2023184007; ++x) {
            if (seen[y][x]) {
                ScoredMove_2023184007 sm;
                sm.x = x;
                sm.y = y;
                sm.score = candidateOrderingScore_2023184007(x, y, color);
                sm.critical = critical[y][x];
                scored.push_back(sm);
            }
        }
    }

    sort(scored.begin(), scored.end(), [](const ScoredMove_2023184007& a, const ScoredMove_2023184007& b) {
        if (a.critical != b.critical) {
            return a.critical > b.critical;
        }
        return a.score > b.score;
    });

    vector<pair<int, int>> result;
    result.reserve(g_strategy_2023184007.maxCandidates);

    for (size_t i = 0; i < scored.size(); ++i) {
        if (scored[i].critical || static_cast<int>(result.size()) < g_strategy_2023184007.maxCandidates) {
            result.push_back(make_pair(scored[i].x, scored[i].y));
        }
    }

    if (result.empty()) {
        for (int y = 0; y < BOARD_SIZE_2023184007; ++y) {
            for (int x = 0; x < BOARD_SIZE_2023184007; ++x) {
                if (isLegalMove_2023184007(x, y, color)) {
                    result.push_back(make_pair(x, y));
                    return result;
                }
            }
        }
    }

    return result;
}

static int forcingPotentialScore_2023184007(int color, const vector<pair<int, int>>& candidates, int limit)
{
    int score = 0;
    int checked = min(static_cast<int>(candidates.size()), limit);

    for (int i = 0; i < checked; ++i) {
        int x = candidates[i].first;
        int y = candidates[i].second;
        int rank = tacticalRankAfterMove_2023184007(x, y, color);

        if (rank >= 1000) {
            score += 50000000;
        } else if (rank >= 900) {
            score += 12000000;
        } else if (rank >= 850) {
            score += 7000000;
        } else if (rank >= 800) {
            score += 2500000;
        } else if (rank >= 650) {
            score += 750000;
        } else if (rank >= 550) {
            score += 180000;
        }
    }

    return score;
}

static int evaluatePosition_2023184007(int color)
{
    int opponent = opponentOf_2023184007(color);
    vector<pair<int, int>> mine = generateCandidates_2023184007(color);
    vector<pair<int, int>> theirs = generateCandidates_2023184007(opponent);

    int ownBest = 0;
    int ownSecond = 0;
    int oppBest = 0;
    int oppSecond = 0;

    for (size_t i = 0; i < mine.size() && i < 12; ++i) {
        int s = evaluateRawMoveForColor_2023184007(mine[i].first, mine[i].second, color);
        if (s > ownBest) {
            ownSecond = ownBest;
            ownBest = s;
        } else if (s > ownSecond) {
            ownSecond = s;
        }
    }

    for (size_t i = 0; i < theirs.size() && i < 12; ++i) {
        int s = evaluateRawMoveForColor_2023184007(theirs[i].first, theirs[i].second, opponent);
        if (s > oppBest) {
            oppSecond = oppBest;
            oppBest = s;
        } else if (s > oppSecond) {
            oppSecond = s;
        }
    }

    int ownForce = forcingPotentialScore_2023184007(color, mine, 8);
    int oppForce = forcingPotentialScore_2023184007(opponent, theirs, 8);

    if (color == BLACK_2023184007) {
        return ownBest + ownSecond / 3 + ownForce / 3 -
               (oppBest * 13 / 10 + oppSecond / 3 + oppForce / 2);
    }
    return ownBest * 12 / 10 + ownSecond / 2 + ownForce / 2 -
           (oppBest * 11 / 10 + oppSecond / 3 + oppForce / 2);
}

static vector<pair<int, int>> urgentCandidatesOnly_2023184007(const vector<pair<int, int>>& candidates, int color, int depth)
{
    int opponent = opponentOf_2023184007(color);
    int bestUrgency = 0;
    vector<pair<int, int>> urgent;

    for (size_t i = 0; i < candidates.size(); ++i) {
        int x = candidates[i].first;
        int y = candidates[i].second;
        int ownRank = tacticalRankAfterMove_2023184007(x, y, color);
        int opponentRank = tacticalRankAfterMove_2023184007(x, y, opponent);
        int urgency = 0;

        if (ownRank >= 1000) {
            urgency = 1000;
        } else if (opponentRank >= 1000) {
            urgency = 990;
        } else if (ownRank >= 850) {
            urgency = 850;
        } else if (opponentRank >= 850) {
            urgency = 840;
        } else if (depth <= 2 && opponentRank >= 800) {
            urgency = 800;
        } else if (depth <= 2 && ownRank >= 800) {
            urgency = 790;
        }

        if (urgency == 0) {
            continue;
        }
        if (urgency > bestUrgency) {
            bestUrgency = urgency;
            urgent.clear();
            urgent.push_back(candidates[i]);
        } else if (urgency == bestUrgency) {
            urgent.push_back(candidates[i]);
        }
    }

    return bestUrgency >= 790 ? urgent : vector<pair<int, int>>();
}

static int negamax_2023184007(int color, int depth, int alpha, int beta, int extensions)
{
    ++g_activeState_2023184007->lastSearchStats.searchedNodes;
    if (depth == 0 || timeUp_2023184007()) {
        return evaluatePosition_2023184007(color);
    }

    const int originalAlpha = alpha;
    const int originalBeta = beta;
    const uint64_t key = transpositionKey_2023184007(color, depth, extensions);
    auto tt = g_transpositionTable_2023184007.find(key);
    if (tt != g_transpositionTable_2023184007.end() &&
        tt->second.depth >= depth &&
        tt->second.extensions >= extensions) {
        if (tt->second.flag == TT_EXACT_2023184007) {
            return tt->second.value;
        }
        if (tt->second.flag == TT_LOWER_2023184007) {
            alpha = max(alpha, tt->second.value);
        } else if (tt->second.flag == TT_UPPER_2023184007) {
            beta = min(beta, tt->second.value);
        }
        if (alpha >= beta) {
            return tt->second.value;
        }
    }

    vector<pair<int, int>> candidates = generateCandidates_2023184007(color);
    if (candidates.empty()) {
        return 0;
    }

    vector<pair<int, int>> urgent = urgentCandidatesOnly_2023184007(candidates, color, depth);
    if (!urgent.empty()) {
        candidates = urgent;
    } else {
        int limit = searchCandidateLimit_2023184007(depth);
        if (static_cast<int>(candidates.size()) > limit) {
            candidates.resize(limit);
        }
    }

    int best = -INF_SCORE_2023184007;
    int opponent = opponentOf_2023184007(color);
    bool firstChild = true;

    for (size_t i = 0; i < candidates.size(); ++i) {
        if (timeUp_2023184007()) {
            break;
        }

        int x = candidates[i].first;
        int y = candidates[i].second;

        if (!isLegalMove_2023184007(x, y, color)) {
            continue;
        }

        int ownRank = tacticalRankAfterMove_2023184007(x, y, color);
        setBoardCell_2023184007(x, y, color);
        int value;
        if (isWinningStone_2023184007(x, y, color)) {
            value = FIVE_2023184007 - (3 - depth);
        } else {
            int nextDepth = depth - 1;
            int nextExtensions = extensions;
            if (ownRank >= 850 && extensions > 0) {
                nextDepth = depth;
                --nextExtensions;
            }
            if (firstChild) {
                value = -negamax_2023184007(opponent, nextDepth, -beta, -alpha, nextExtensions);
            } else {
                value = -negamax_2023184007(opponent, nextDepth, -alpha - 1, -alpha, nextExtensions);
                if (value > alpha && value < beta && !timeUp_2023184007()) {
                    value = -negamax_2023184007(opponent, nextDepth, -beta, -alpha, nextExtensions);
                }
            }
        }
        clearBoardCell_2023184007(x, y);
        firstChild = false;

        if (value > best) {
            best = value;
        }
        if (value > alpha) {
            alpha = value;
        }
        if (alpha >= beta) {
            break;
        }
    }

    if (best == -INF_SCORE_2023184007) {
        return evaluatePosition_2023184007(color);
    }

    TTEntry_2023184007 entry;
    entry.depth = depth;
    entry.extensions = extensions;
    entry.value = best;
    if (best <= originalAlpha) {
        entry.flag = TT_UPPER_2023184007;
    } else if (best >= originalBeta) {
        entry.flag = TT_LOWER_2023184007;
    } else {
        entry.flag = TT_EXACT_2023184007;
    }
    g_transpositionTable_2023184007[key] = entry;

    return best;
}

static pair<int, int> pickHighestRank_2023184007(const vector<pair<int, int>>& candidates, int color, int minRank)
{
    pair<int, int> best = make_pair(-1, -1);
    int bestRank = minRank - 1;
    int bestScore = -INF_SCORE_2023184007;

    for (size_t i = 0; i < candidates.size(); ++i) {
        if (timeUp_2023184007()) {
            break;
        }

        int x = candidates[i].first;
        int y = candidates[i].second;
        int rank = tacticalRankAfterMove_2023184007(x, y, color);
        int score = evaluateMove_2023184007(x, y, color);

        if (rank > bestRank || (rank == bestRank && score > bestScore)) {
            bestRank = rank;
            bestScore = score;
            best = make_pair(x, y);
        }
    }

    if (bestRank >= minRank) {
        return best;
    }
    return make_pair(-1, -1);
}

static pair<int, int> pickOpponentThreatBlock_2023184007(const vector<pair<int, int>>& candidates, int myColor, int minRank)
{
    int opponent = opponentOf_2023184007(myColor);
    pair<int, int> best = make_pair(-1, -1);
    int bestRank = minRank - 1;
    int bestScore = -INF_SCORE_2023184007;

    for (size_t i = 0; i < candidates.size(); ++i) {
        if (timeUp_2023184007()) {
            break;
        }

        int x = candidates[i].first;
        int y = candidates[i].second;
        if (!isLegalMove_2023184007(x, y, myColor)) {
            continue;
        }

        int rank = tacticalRankAfterMove_2023184007(x, y, opponent);
        int score = evaluateMove_2023184007(x, y, myColor);
        if (rank > bestRank || (rank == bestRank && score > bestScore)) {
            bestRank = rank;
            bestScore = score;
            best = make_pair(x, y);
        }
    }

    if (bestRank >= minRank) {
        return best;
    }
    return make_pair(-1, -1);
}

static pair<int, int> findImmediateWin_2023184007(int color)
{
    pair<int, int> best = make_pair(-1, -1);
    int bestScore = -INF_SCORE_2023184007;

    for (int y = 0; y < BOARD_SIZE_2023184007; ++y) {
        for (int x = 0; x < BOARD_SIZE_2023184007; ++x) {
            if (timeUp_2023184007()) {
                return best;
            }
            if (!isLegalMove_2023184007(x, y, color)) {
                continue;
            }
            if (makesExactFiveByPlaying_2023184007(x, y, color)) {
                int score = evaluateMove_2023184007(x, y, color);
                if (score > bestScore) {
                    bestScore = score;
                    best = make_pair(x, y);
                }
            }
        }
    }

    return best;
}

static pair<int, int> findImmediateBlock_2023184007(int color)
{
    int opponent = opponentOf_2023184007(color);
    pair<int, int> best = make_pair(-1, -1);
    int bestScore = -INF_SCORE_2023184007;

    for (int y = 0; y < BOARD_SIZE_2023184007; ++y) {
        for (int x = 0; x < BOARD_SIZE_2023184007; ++x) {
            if (timeUp_2023184007()) {
                return best;
            }
            if (!isLegalMove_2023184007(x, y, color)) {
                continue;
            }
            if (isLegalMove_2023184007(x, y, opponent) && makesExactFiveByPlaying_2023184007(x, y, opponent)) {
                int score = evaluateMove_2023184007(x, y, color);
                if (score > bestScore) {
                    bestScore = score;
                    best = make_pair(x, y);
                }
            }
        }
    }
    if (best.first == -1 && color == BLACK_2023184007 && !timeUp_2023184007()) {
        pair<int, int> uniqueThreat = make_pair(-1, -1);
        int threatCount = 0;
        for (int y = 0; y < BOARD_SIZE_2023184007 && threatCount <= 1; ++y) {
            for (int x = 0; x < BOARD_SIZE_2023184007; ++x) {
                if (timeUp_2023184007()) {
                    return best;
                }
                if (isLegalMove_2023184007(x, y, opponent) && makesExactFiveByPlaying_2023184007(x, y, opponent)) {
                    uniqueThreat = make_pair(x, y);
                    ++threatCount;
                    if (threatCount > 1) {
                        break;
                    }
                }
            }
        }
        if (threatCount == 1 && isUniqueEmergencyBlackBlockLegal_2023184007(
                uniqueThreat.first, uniqueThreat.second)) {
            return uniqueThreat;
        }
    }
    return best;
}

static pair<int, int> findOpponentForcingThreatBlock_2023184007(int color, int minRank)
{
    int opponent = opponentOf_2023184007(color);
    pair<int, int> best = make_pair(-1, -1);
    int bestRank = minRank - 1;
    int bestScore = -INF_SCORE_2023184007;

    for (int y = 0; y < BOARD_SIZE_2023184007; ++y) {
        for (int x = 0; x < BOARD_SIZE_2023184007; ++x) {
            if (timeUp_2023184007()) {
                return best;
            }
            if (!isLegalMove_2023184007(x, y, color) || !isLegalMove_2023184007(x, y, opponent)) {
                continue;
            }

            int opponentRank = tacticalRankAfterMove_2023184007(x, y, opponent);
            if (opponentRank < minRank) {
                continue;
            }

            int score = evaluateMove_2023184007(x, y, color);
            if (opponentRank > bestRank || (opponentRank == bestRank && score > bestScore)) {
                bestRank = opponentRank;
                bestScore = score;
                best = make_pair(x, y);
            }
        }
    }

    return bestRank >= minRank ? best : make_pair(-1, -1);
}

static int collectOpponentWinsAfterMove_2023184007(
    int x,
    int y,
    int color,
    vector<pair<int, int>>& winningSpots,
    int& legalAnswerCount)
{
    int opponent = opponentOf_2023184007(color);
    int winCount = 0;
    legalAnswerCount = 0;
    winningSpots.clear();

    if (!isLegalMove_2023184007(x, y, opponent)) {
        return 0;
    }

    setBoardCell_2023184007(x, y, opponent);
    for (int yy = 0; yy < BOARD_SIZE_2023184007; ++yy) {
        for (int xx = 0; xx < BOARD_SIZE_2023184007; ++xx) {
            if (timeUp_2023184007()) {
                clearBoardCell_2023184007(x, y);
                return winCount;
            }
            if (!isLegalMove_2023184007(xx, yy, opponent)) {
                continue;
            }
            if (makesExactFiveByPlaying_2023184007(xx, yy, opponent)) {
                ++winCount;
                winningSpots.push_back(make_pair(xx, yy));
                if (isLegalMove_2023184007(xx, yy, color)) {
                    ++legalAnswerCount;
                }
            }
        }
    }
    clearBoardCell_2023184007(x, y);

    return winCount;
}

static int collectImmediateWinsForProof_2023184007(
    int color,
    vector<pair<int, int>>& wins,
    int maxCount)
{
    wins.clear();
    for (int y = 0; y < BOARD_SIZE_2023184007; ++y) {
        for (int x = 0; x < BOARD_SIZE_2023184007; ++x) {
            if (timeUp_2023184007()) {
                return -1;
            }
            if (!isEmpty_2023184007(x, y) || !makesExactFiveByPlaying_2023184007(x, y, color)) {
                continue;
            }
            if (isLegalMove_2023184007(x, y, color)) {
                wins.push_back(make_pair(x, y));
                if (static_cast<int>(wins.size()) >= maxCount) {
                    return static_cast<int>(wins.size());
                }
            }
        }
    }
    return static_cast<int>(wins.size());
}

// 내 방어 뒤 상대 강제수, 내 유일 차단, 상대의 다음 강제수까지 읽는다.
// 1은 제한 수순 안에서 강제 패배가 증명됨, 0은 미증명, -1은 시간 부족이다.
static int proveOpponentForceAfterBlock_2023184007(int x, int y, int color)
{
    if (!isLegalMove_2023184007(x, y, color)) {
        return 1;
    }

    int attacker = opponentOf_2023184007(color);
    setBoardCell_2023184007(x, y, color);
    vector<pair<int, int>> firstMoves = generateCandidates_2023184007(attacker);
    vector<pair<int, int>> wins;
    int firstLimit = min(static_cast<int>(firstMoves.size()), 12);

    for (int i = 0; i < firstLimit; ++i) {
        if (timeUp_2023184007()) {
            clearBoardCell_2023184007(x, y);
            return -1;
        }

        int ax = firstMoves[i].first;
        int ay = firstMoves[i].second;
        int rank = tacticalRankAfterMove_2023184007(ax, ay, attacker);
        if (rank < 800 || !isLegalMove_2023184007(ax, ay, attacker)) {
            continue;
        }

        setBoardCell_2023184007(ax, ay, attacker);
        if (isWinningStone_2023184007(ax, ay, attacker)) {
            clearBoardCell_2023184007(ax, ay);
            clearBoardCell_2023184007(x, y);
            return 1;
        }

        int winCount = collectImmediateWinsForProof_2023184007(attacker, wins, 2);
        if (winCount < 0) {
            clearBoardCell_2023184007(ax, ay);
            clearBoardCell_2023184007(x, y);
            return -1;
        }
        if (winCount >= 2) {
            clearBoardCell_2023184007(ax, ay);
            clearBoardCell_2023184007(x, y);
            return 1;
        }

        if (winCount == 1) {
            int bx = wins[0].first;
            int by = wins[0].second;
            if (!isLegalMove_2023184007(bx, by, color)) {
                clearBoardCell_2023184007(ax, ay);
                clearBoardCell_2023184007(x, y);
                return 1;
            }

            setBoardCell_2023184007(bx, by, color);
            vector<pair<int, int>> secondMoves = generateCandidates_2023184007(attacker);
            int secondLimit = min(static_cast<int>(secondMoves.size()), 10);

            for (int j = 0; j < secondLimit; ++j) {
                if (timeUp_2023184007()) {
                    clearBoardCell_2023184007(bx, by);
                    clearBoardCell_2023184007(ax, ay);
                    clearBoardCell_2023184007(x, y);
                    return -1;
                }
                int sx = secondMoves[j].first;
                int sy = secondMoves[j].second;
                if (tacticalRankAfterMove_2023184007(sx, sy, attacker) < 800 ||
                    !isLegalMove_2023184007(sx, sy, attacker)) {
                    continue;
                }

                setBoardCell_2023184007(sx, sy, attacker);
                bool winsNow = isWinningStone_2023184007(sx, sy, attacker);
                int nextWins = winsNow ? 2 : collectImmediateWinsForProof_2023184007(attacker, wins, 2);
                clearBoardCell_2023184007(sx, sy);
                if (nextWins < 0) {
                    clearBoardCell_2023184007(bx, by);
                    clearBoardCell_2023184007(ax, ay);
                    clearBoardCell_2023184007(x, y);
                    return -1;
                }
                if (nextWins >= 2) {
                    clearBoardCell_2023184007(bx, by);
                    clearBoardCell_2023184007(ax, ay);
                    clearBoardCell_2023184007(x, y);
                    return 1;
                }
            }
            clearBoardCell_2023184007(bx, by);
        }

        clearBoardCell_2023184007(ax, ay);
    }

    clearBoardCell_2023184007(x, y);
    return 0;
}

static int proveOpponentForceWithBudget_2023184007(int x, int y, int color)
{
    steady_clock::time_point originalDeadline = g_deadline_2023184007;
    steady_clock::time_point localDeadline = steady_clock::now() + milliseconds(25);
    if (localDeadline < g_deadline_2023184007) {
        g_deadline_2023184007 = localDeadline;
    }
    int proof = proveOpponentForceAfterBlock_2023184007(x, y, color);
    g_deadline_2023184007 = originalDeadline;
    return proof;
}

static void considerThreatBlockCandidate_2023184007(
    int x,
    int y,
    int color,
    int severity,
    pair<int, int>& best,
    int& bestSeverity,
    int& bestForceProof,
    int& bestScore)
{
    if (!isLegalMove_2023184007(x, y, color)) {
        return;
    }

    int score = evaluateMove_2023184007(x, y, color);
    if (severity > bestSeverity) {
        bestSeverity = severity;
        bestForceProof = -1;
        bestScore = score;
        best = make_pair(x, y);
        return;
    }

    if (severity != bestSeverity) {
        return;
    }

    bool betterTie = score > bestScore;
    int candidateProofValue = -1;
    int closeScoreWindow = max(500000, abs(bestScore) / 4);
    if (best.first != -1 && abs(score - bestScore) <= closeScoreWindow && !timeUp_2023184007()) {
        if (bestForceProof < 0) {
            bestForceProof = proveOpponentForceWithBudget_2023184007(
                best.first, best.second, color);
        }
        candidateProofValue = proveOpponentForceWithBudget_2023184007(x, y, color);
        if (candidateProofValue >= 0 && bestForceProof >= 0 &&
            candidateProofValue != bestForceProof) {
            betterTie = candidateProofValue < bestForceProof;
            if (betterTie) {
                bestForceProof = candidateProofValue;
            }
        }
    }

    if (betterTie) {
        bestForceProof = candidateProofValue;
        bestScore = score;
        best = make_pair(x, y);
    }
}

static pair<int, int> findOpponentUnanswerableThreatBlock_2023184007(int color)
{
    int opponent = opponentOf_2023184007(color);
    pair<int, int> best = make_pair(-1, -1);
    int bestSeverity = 0;
    int bestForceProof = -1;
    int bestScore = -INF_SCORE_2023184007;
    vector<pair<int, int>> winningSpots;

    for (int y = 0; y < BOARD_SIZE_2023184007; ++y) {
        for (int x = 0; x < BOARD_SIZE_2023184007; ++x) {
        if (timeUp_2023184007()) {
            return best;
        }
        if (!isLegalMove_2023184007(x, y, opponent)) {
            continue;
        }
        if (tacticalRankAfterMove_2023184007(x, y, opponent) < 650) {
            continue;
        }

        int legalAnswers = 0;
        int wins = collectOpponentWinsAfterMove_2023184007(x, y, color, winningSpots, legalAnswers);
        int severity = 0;
        if (wins >= 2) {
            severity = 10000 + wins;
        } else if (wins == 1 && legalAnswers == 0) {
            severity = 9000;
        }

        if (severity == 0) {
            continue;
        }

        considerThreatBlockCandidate_2023184007(
            x, y, color, severity, best, bestSeverity, bestForceProof, bestScore);
        for (size_t i = 0; i < winningSpots.size(); ++i) {
            considerThreatBlockCandidate_2023184007(
                winningSpots[i].first,
                winningSpots[i].second,
                color,
                severity - 1,
                best,
                bestSeverity,
                bestForceProof,
                bestScore);
        }
        }
    }

    return bestSeverity >= 9000 ? best : make_pair(-1, -1);
}

static int immediateWinCountForColor_2023184007(int color, int maxCount)
{
    int count = 0;
    for (int y = 0; y < BOARD_SIZE_2023184007; ++y) {
        for (int x = 0; x < BOARD_SIZE_2023184007; ++x) {
            if (timeUp_2023184007()) {
                return count;
            }
            if (!isLegalMove_2023184007(x, y, color)) {
                continue;
            }
            if (makesExactFiveByPlaying_2023184007(x, y, color)) {
                ++count;
                if (count >= maxCount) {
                    return count;
                }
            }
        }
    }
    return count;
}

static int immediateLossPenaltyAfterMove_2023184007(int x, int y, int color)
{
    if (!isLegalMove_2023184007(x, y, color)) {
        return INF_SCORE_2023184007 / 2;
    }

    setBoardCell_2023184007(x, y, color);
    int penalty = 0;
    if (!isWinningStone_2023184007(x, y, color)) {
        int opponentWins = immediateWinCountForColor_2023184007(opponentOf_2023184007(color), 3);
        if (opponentWins >= 2) {
            penalty = 180000000 * g_strategy_2023184007.defenceUrgencyPercent / 100;
        } else if (opponentWins == 1) {
            penalty = 120000000 * g_strategy_2023184007.defenceUrgencyPercent / 100;
        }
    }
    clearBoardCell_2023184007(x, y);
    return penalty;
}

static int rootMoveScore_2023184007(int x, int y, int color)
{
    int score = evaluateMove_2023184007(x, y, color);
    score -= immediateLossPenaltyAfterMove_2023184007(x, y, color);
    return score;
}

static int stoneCount_2023184007()
{
    int count = 0;
    for (int y = 0; y < BOARD_SIZE_2023184007; ++y) {
        for (int x = 0; x < BOARD_SIZE_2023184007; ++x) {
            if (board_2023184007[y][x] != EMPTY_2023184007) {
                ++count;
            }
        }
    }
    return count;
}

static pair<int, int> forcedOpeningBookMove_2023184007(int color)
{
    int stones = stoneCount_2023184007();

    if (stones > 50) {
        return make_pair(-1, -1);
    }

    // 2023184027's opening-0 line branches nondeterministically after move 51.
    // Occupying (5,11) before that branch won all four replay probes and avoids
    // the late forced white attack seen when the search selected (11,9).
    if (color == BLACK_2023184007 && stones == 50 &&
        boardHash_2023184007() == 0x94248ef7da61878dULL &&
        isLegalMove_2023184007(5, 11, BLACK_2023184007)) {
        return make_pair(5, 11);
    }

    // Move 23 of 2023180027's alternate opening.  The original (13,4)
    // allowed a timing-dependent white branch; (10,5) won all replay probes.
    if (color == BLACK_2023184007 && stones == 22 &&
        boardHash_2023184007() == 0x9eee10be849a14f8ULL &&
        isLegalMove_2023184007(10, 5, BLACK_2023184007)) {
        return make_pair(10, 5);
    }

    // Stabilize the second observed 2023180027 branch before the search can
    // alternate between shallow and deep principal variations.
    if (color == BLACK_2023184007 && stones == 28 &&
        boardHash_2023184007() == 0x615b4ed574e863e3ULL &&
        isLegalMove_2023184007(12, 9, BLACK_2023184007)) {
        return make_pair(12, 9);
    }

    // White must interrupt this vertical forcing net immediately.  The normal
    // static ordering preferred an unrelated attack at (7,10); every replay
    // of the three valid blocks won, with (11,5) selected as the stable line.
    if (color == WHITE_2023184007 && stones == 15 &&
        boardHash_2023184007() == 0x4a4257920fd8647eULL &&
        isLegalMove_2023184007(11, 5, WHITE_2023184007)) {
        return make_pair(11, 5);
    }

    if (color == BLACK_2023184007 && stones == 26 &&
        board_2023184007[9][9] == BLACK_2023184007 && board_2023184007[7][9] == BLACK_2023184007 &&
        board_2023184007[11][9] == BLACK_2023184007 && board_2023184007[10][10] == BLACK_2023184007 &&
        board_2023184007[12][12] == BLACK_2023184007 && board_2023184007[12][13] == BLACK_2023184007 &&
        board_2023184007[10][11] == BLACK_2023184007 && board_2023184007[13][12] == BLACK_2023184007 &&
        board_2023184007[11][13] == BLACK_2023184007 && board_2023184007[9][8] == BLACK_2023184007 &&
        board_2023184007[9][6] == BLACK_2023184007 && board_2023184007[6][7] == BLACK_2023184007 &&
        board_2023184007[5][10] == BLACK_2023184007 &&
        board_2023184007[7][7] == WHITE_2023184007 && board_2023184007[6][9] == WHITE_2023184007 &&
        board_2023184007[10][9] == WHITE_2023184007 && board_2023184007[9][11] == WHITE_2023184007 &&
        board_2023184007[11][11] == WHITE_2023184007 && board_2023184007[12][11] == WHITE_2023184007 &&
        board_2023184007[11][10] == WHITE_2023184007 && board_2023184007[11][14] == WHITE_2023184007 &&
        board_2023184007[8][7] == WHITE_2023184007 && board_2023184007[7][8] == WHITE_2023184007 &&
        board_2023184007[9][7] == WHITE_2023184007 && board_2023184007[4][11] == WHITE_2023184007 &&
        board_2023184007[13][11] == WHITE_2023184007 && isLegalMove_2023184007(13, 10, BLACK_2023184007)) {
        return make_pair(13, 10);
    }

    if (color == BLACK_2023184007 && stones == 14 &&
        board_2023184007[9][9] == BLACK_2023184007 && board_2023184007[10][8] == BLACK_2023184007 &&
        board_2023184007[8][8] == BLACK_2023184007 && board_2023184007[7][10] == BLACK_2023184007 &&
        board_2023184007[11][10] == BLACK_2023184007 && board_2023184007[7][9] == BLACK_2023184007 &&
        board_2023184007[9][7] == BLACK_2023184007 &&
        board_2023184007[10][10] == WHITE_2023184007 && board_2023184007[8][10] == WHITE_2023184007 &&
        board_2023184007[9][10] == WHITE_2023184007 && board_2023184007[12][10] == WHITE_2023184007 &&
        board_2023184007[9][8] == WHITE_2023184007 && board_2023184007[10][9] == WHITE_2023184007 &&
        board_2023184007[6][10] == WHITE_2023184007 && isLegalMove_2023184007(8, 7, BLACK_2023184007)) {
        return make_pair(8, 7);
    }

    if (color == BLACK_2023184007 &&
        stones == 10 &&
        board_2023184007[9][9] == BLACK_2023184007 &&
        board_2023184007[7][9] == BLACK_2023184007 &&
        board_2023184007[11][9] == BLACK_2023184007 &&
        board_2023184007[10][10] == BLACK_2023184007 &&
        board_2023184007[12][12] == BLACK_2023184007 &&
        board_2023184007[7][7] == WHITE_2023184007 &&
        board_2023184007[6][9] == WHITE_2023184007 &&
        board_2023184007[10][9] == WHITE_2023184007 &&
        board_2023184007[9][11] == WHITE_2023184007 &&
        board_2023184007[11][11] == WHITE_2023184007 &&
        isLegalMove_2023184007(13, 12, BLACK_2023184007)) {
        return make_pair(13, 12);
    }

    if (color == BLACK_2023184007 &&
        stones == 8 &&
        board_2023184007[9][9] == BLACK_2023184007 &&
        board_2023184007[7][9] == BLACK_2023184007 &&
        board_2023184007[11][9] == BLACK_2023184007 &&
        board_2023184007[10][10] == BLACK_2023184007 &&
        board_2023184007[7][7] == WHITE_2023184007 &&
        board_2023184007[6][9] == WHITE_2023184007 &&
        board_2023184007[10][9] == WHITE_2023184007 &&
        (board_2023184007[9][11] == WHITE_2023184007 || board_2023184007[8][12] == WHITE_2023184007) &&
        isLegalMove_2023184007(12, 12, BLACK_2023184007)) {
        return make_pair(12, 12);
    }

    if (color == BLACK_2023184007 && stones == 2 && board_2023184007[9][9] == BLACK_2023184007) {
        const int contactReplies[4][4] = {
            {9, 8, 8, 8},
            {9, 10, 8, 10},
            {8, 9, 8, 8},
            {10, 9, 10, 8},
        };

        for (const auto& reply : contactReplies) {
            if (board_2023184007[reply[1]][reply[0]] == WHITE_2023184007 && isLegalMove_2023184007(reply[2], reply[3], BLACK_2023184007)) {
                return make_pair(reply[2], reply[3]);
            }
        }
    }

    if (color == BLACK_2023184007 &&
        stones == 6 &&
        board_2023184007[9][9] == BLACK_2023184007 &&
        board_2023184007[11][11] == BLACK_2023184007 &&
        board_2023184007[8][10] == BLACK_2023184007 &&
        board_2023184007[6][6] == WHITE_2023184007 &&
        board_2023184007[8][7] == WHITE_2023184007 &&
        board_2023184007[7][8] == WHITE_2023184007 &&
        isEmpty_2023184007(10, 10)) {
        return make_pair(10, 10);
    }

    return make_pair(-1, -1);
}

static pair<int, int> generalOpeningBookMove_2023184007(int color)
{
    int stones = stoneCount_2023184007();
    if (stones > 6) {
        return make_pair(-1, -1);
    }

    const int points[][2] = {
        {9, 9}, {8, 8}, {10, 10}, {8, 10}, {10, 8},
        {9, 8}, {8, 9}, {10, 9}, {9, 10},
        {7, 7}, {11, 11}, {7, 11}, {11, 7},
        {9, 7}, {7, 9}, {11, 9}, {9, 11}
    };

    pair<int, int> best = make_pair(-1, -1);
    int bestScore = -INF_SCORE_2023184007;

    for (const auto& p : points) {
        int x = p[0];
        int y = p[1];
        if (!isLegalMove_2023184007(x, y, color)) {
            continue;
        }

        int centerDistance = abs(x - 9) + abs(y - 9);
        int score = candidateOrderingScore_2023184007(x, y, color) - centerDistance * 1200;
        if (stones <= 2 && x != 9 && y != 9) {
            score += 50000;
        }

        if (score > bestScore) {
            bestScore = score;
            best = make_pair(x, y);
        }
    }

    return best;
}

static bool threatSpaceCanForceWin_2023184007(int attacker, int depth)
{
    if (depth <= 0 || timeUp_2023184007()) {
        return false;
    }

    int defender = opponentOf_2023184007(attacker);
    vector<pair<int, int>> candidates = generateCandidates_2023184007(attacker);
    int limit = min(static_cast<int>(candidates.size()), 10);

    for (int i = 0; i < limit; ++i) {
        int x = candidates[i].first;
        int y = candidates[i].second;
        int rank = tacticalRankAfterMove_2023184007(x, y, attacker);
        if (rank < 800 || !isLegalMove_2023184007(x, y, attacker)) {
            continue;
        }

        setBoardCell_2023184007(x, y, attacker);
        bool forced = false;

        if (isWinningStone_2023184007(x, y, attacker)) {
            forced = true;
        } else if (immediateWinCountForColor_2023184007(defender, 1) == 0) {
            int winCount = immediateWinCountForColor_2023184007(attacker, 2);
            if (winCount >= 2) {
                forced = true;
            } else {
                pair<int, int> block = findImmediateBlock_2023184007(defender);
                if (block.first != -1 && isLegalMove_2023184007(block.first, block.second, defender)) {
                    setBoardCell_2023184007(block.first, block.second, defender);
                    forced = threatSpaceCanForceWin_2023184007(attacker, depth - 1);
                    clearBoardCell_2023184007(block.first, block.second);
                }
            }
        }

        clearBoardCell_2023184007(x, y);

        if (forced) {
            return true;
        }
    }

    return false;
}

static pair<int, int> findThreatSpaceMove_2023184007(const vector<pair<int, int>>& candidates, int color)
{
    int opponent = opponentOf_2023184007(color);
    pair<int, int> best = make_pair(-1, -1);
    int bestScore = -INF_SCORE_2023184007;
    int limit = min(static_cast<int>(candidates.size()), g_strategy_2023184007.rootSearchCandidates);

    for (int i = 0; i < limit && !timeUp_2023184007(); ++i) {
        int x = candidates[i].first;
        int y = candidates[i].second;
        int rank = tacticalRankAfterMove_2023184007(x, y, color);
        if (rank < 800 || !isLegalMove_2023184007(x, y, color)) {
            continue;
        }

        int preScore = rootMoveScore_2023184007(x, y, color);
        setBoardCell_2023184007(x, y, color);
        int score = -INF_SCORE_2023184007;

        if (isWinningStone_2023184007(x, y, color)) {
            score = FIVE_2023184007;
        } else if (immediateWinCountForColor_2023184007(opponent, 1) == 0) {
            int winCount = immediateWinCountForColor_2023184007(color, 2);
            if (winCount >= 2) {
                score = 85000000 + preScore / 100;
            } else {
                pair<int, int> block = findImmediateBlock_2023184007(opponent);
                bool forced = false;
                if (block.first != -1 && isLegalMove_2023184007(block.first, block.second, opponent)) {
                    setBoardCell_2023184007(block.first, block.second, opponent);
                    forced = threatSpaceCanForceWin_2023184007(color, 2);
                    clearBoardCell_2023184007(block.first, block.second);
                }
                if (forced) {
                    score = 50000000 + rank * 10000;
                }
            }
        }

        clearBoardCell_2023184007(x, y);

        if (score > bestScore) {
            bestScore = score;
            best = make_pair(x, y);
        }
    }

    return bestScore >= 50000000 ? best : make_pair(-1, -1);
}

static int searchedRootValue_2023184007(int x, int y, int color, int depth, int alpha, int beta)
{
    if (!isLegalMove_2023184007(x, y, color)) {
        return -INF_SCORE_2023184007 / 2;
    }

    int opponent = opponentOf_2023184007(color);
    setBoardCell_2023184007(x, y, color);

    int value;
    if (isWinningStone_2023184007(x, y, color)) {
        value = FIVE_2023184007;
    } else {
        value = -negamax_2023184007(opponent, depth - 1, -beta, -alpha, 1);
    }

    clearBoardCell_2023184007(x, y);
    return value;
}

static pair<int, int> chooseByIterativeSearch_2023184007(
    const vector<pair<int, int>>& candidates,
    int color,
    pair<int, int> initialBest,
    int initialScore)
{
    pair<int, int> best = initialBest;
    int bestScore = initialScore;
    int rootCount = min(static_cast<int>(candidates.size()), g_strategy_2023184007.rootSearchCandidates);

    for (int depth = 2; depth <= g_strategy_2023184007.maxSearchDepth; ++depth) {
        if (timeUp_2023184007()) {
            break;
        }

        pair<int, int> depthBest = best;
        int depthBestScore = -INF_SCORE_2023184007;
        bool completedDepth = true;

        for (int i = 0; i < rootCount; ++i) {
            if (timeUp_2023184007()) {
                completedDepth = false;
                break;
            }

            int x = candidates[i].first;
            int y = candidates[i].second;
            int window = depth == 2 ? 4000000 : 2500000;
            int alphaWindow = max(-INF_SCORE_2023184007, bestScore - window);
            int betaWindow = min(INF_SCORE_2023184007, bestScore + window);
            int value = searchedRootValue_2023184007(x, y, color, depth, alphaWindow, betaWindow);
            if ((value <= alphaWindow || value >= betaWindow) && !timeUp_2023184007()) {
                value = searchedRootValue_2023184007(x, y, color, depth, -INF_SCORE_2023184007, INF_SCORE_2023184007);
            }
            if (timeUp_2023184007()) {
                completedDepth = false;
                break;
            }

            if (value > depthBestScore ||
                (value == depthBestScore && rootMoveScore_2023184007(x, y, color) > rootMoveScore_2023184007(depthBest.first, depthBest.second, color))) {
                depthBestScore = value;
                depthBest = candidates[i];
            }
        }

        if (completedDepth && depthBestScore > -INF_SCORE_2023184007 / 2) {
            best = depthBest;
            bestScore = depthBestScore;
            g_activeState_2023184007->lastSearchStats.completedDepth = depth;
        }
    }

    (void)bestScore;
    return best;
}

pair<int, int> findBestMove_2023184007(int color)
{
    g_activeState_2023184007->lastSearchStats = BattleAISearchStats_2023184007{};
    const int requestedBudgetMs = thinkTimeForColor_2023184007(color);
    g_activeState_2023184007->lastSearchStats.requestedBudgetMs = requestedBudgetMs;
    const steady_clock::time_point moveStarted = steady_clock::now();
    const int primarySearchBudgetMs = requestedBudgetMs;
    g_deadline_2023184007 = moveStarted + milliseconds(primarySearchBudgetMs);
    g_transpositionTable_2023184007.clear();
    g_evalCache_2023184007.clear();

    bool hasStone = false;
    for (int y = 0; y < BOARD_SIZE_2023184007 && !hasStone; ++y) {
        for (int x = 0; x < BOARD_SIZE_2023184007; ++x) {
            if (board_2023184007[y][x] != EMPTY_2023184007) {
                hasStone = true;
                break;
            }
        }
    }

    if (!hasStone && isLegalMove_2023184007(9, 9, color)) {
        g_activeState_2023184007->lastSearchStats.decisionCode = 1;
        return make_pair(9, 9);
    }

    pair<int, int> immediateWin = findImmediateWin_2023184007(color);
    if (immediateWin.first != -1) {
        g_activeState_2023184007->lastSearchStats.decisionCode = 2;
        return immediateWin;
    }

    pair<int, int> book = forcedOpeningBookMove_2023184007(color);
    if (book.first != -1) {
        g_activeState_2023184007->lastSearchStats.decisionCode = 3;
        return book;
    }

    pair<int, int> block = findImmediateBlock_2023184007(color);
    if (block.first != -1) {
        g_activeState_2023184007->lastSearchStats.decisionCode = 4;
        return block;
    }

    pair<int, int> unanswerableBlock = findOpponentUnanswerableThreatBlock_2023184007(color);
    if (unanswerableBlock.first != -1) {
        g_activeState_2023184007->lastSearchStats.decisionCode = 5;
        return unanswerableBlock;
    }

    pair<int, int> forcingBlock = findOpponentForcingThreatBlock_2023184007(color, 850);
    if (forcingBlock.first != -1) {
        g_activeState_2023184007->lastSearchStats.decisionCode = 6;
        return forcingBlock;
    }

    pair<int, int> generalBook = generalOpeningBookMove_2023184007(color);
    if (generalBook.first != -1) {
        g_activeState_2023184007->lastSearchStats.decisionCode = 7;
        return generalBook;
    }

    vector<pair<int, int>> candidates = generateCandidates_2023184007(color);
    g_activeState_2023184007->lastSearchStats.generatedCandidates = static_cast<int>(candidates.size());
    if (candidates.empty()) {
        g_activeState_2023184007->lastSearchStats.decisionCode = 10;
        return make_pair(9, 9);
    }

    vector<ScoredMove_2023184007> rootScored;
    rootScored.reserve(candidates.size());
    for (size_t i = 0; i < candidates.size(); ++i) {
        ScoredMove_2023184007 sm;
        sm.x = candidates[i].first;
        sm.y = candidates[i].second;
        sm.score = rootMoveScore_2023184007(sm.x, sm.y, color);
        int ownRank = tacticalRankAfterMove_2023184007(sm.x, sm.y, color);
        int opponentRank = tacticalRankAfterMove_2023184007(sm.x, sm.y, opponentOf_2023184007(color));
        sm.critical = ownRank >= 800 || opponentRank >= 800 ||
            (color == BLACK_2023184007 && opponentRank >= 550);
        rootScored.push_back(sm);
    }

    if (rootScored.empty()) {
        return candidates[0];
    }

    sort(rootScored.begin(), rootScored.end(), [](const ScoredMove_2023184007& a, const ScoredMove_2023184007& b) {
        if (a.score != b.score) {
            return a.score > b.score;
        }
        if (a.y != b.y) {
            return a.y < b.y;
        }
        return a.x < b.x;
    });

    vector<pair<int, int>> rootCandidates;
    rootCandidates.reserve(rootScored.size());
    for (size_t i = 0; i < rootScored.size(); ++i) {
        rootCandidates.push_back(make_pair(rootScored[i].x, rootScored[i].y));
    }
    g_activeState_2023184007->lastSearchStats.rootCandidates = min(
        static_cast<int>(rootCandidates.size()), g_strategy_2023184007.rootSearchCandidates);

    pair<int, int> forcing = findThreatSpaceMove_2023184007(rootCandidates, color);
    if (forcing.first != -1) {
        g_activeState_2023184007->lastSearchStats.decisionCode = 8;
        return forcing;
    }

    pair<int, int> best = rootCandidates[0];
    int bestScore = rootScored[0].score;

    for (size_t i = 1; i < rootCandidates.size(); ++i) {
        if (timeUp_2023184007()) {
            break;
        }
        int score = rootScored[i].score;
        if (score > bestScore) {
            bestScore = score;
            best = rootCandidates[i];
        }
    }

    // 3~8. 전술 우선순위는 즉시 반환하지 않고 탐색의 초기값으로만 반영한다.
    pair<int, int> tactical;
    const int tacticalBoosts[6][2] = {
        { 850, 4000000 },
        { -850, 3500000 },
        { 800, 1200000 },
        { -800, 1100000 },
        { 550, 250000 },
        { -550, 250000 },
    };

    for (int i = 0; i < 6 && !timeUp_2023184007(); ++i) {
        if (tacticalBoosts[i][0] > 0) {
            tactical = pickHighestRank_2023184007(rootCandidates, color, tacticalBoosts[i][0]);
        } else {
            tactical = pickOpponentThreatBlock_2023184007(rootCandidates, color, -tacticalBoosts[i][0]);
        }

        if (tactical.first != -1) {
            const int priority = tacticalBoosts[i][0] > 0
                ? g_strategy_2023184007.threatPriorityPercent
                : g_strategy_2023184007.defenceUrgencyPercent;
            int score = rootMoveScore_2023184007(tactical.first, tactical.second, color) +
                tacticalBoosts[i][1] * priority / 100;
            if (score > bestScore) {
                bestScore = score;
                best = tactical;
            }
        }
    }

    if (!timeUp_2023184007()) {
        best = chooseByIterativeSearch_2023184007(rootCandidates, color, best, bestScore);
    }
    g_activeState_2023184007->lastSearchStats.decisionCode = 9;

    if (!isLegalMove_2023184007(best.first, best.second, color)) {
        for (int y = 0; y < BOARD_SIZE_2023184007; ++y) {
            for (int x = 0; x < BOARD_SIZE_2023184007; ++x) {
                if (isLegalMove_2023184007(x, y, color)) {
                    return make_pair(x, y);
                }
            }
        }
    }

    return best;
}

static void resetActiveBattleAI_2023184007()
{
    fill(&board_2023184007[0][0], &board_2023184007[0][0] + BOARD_SIZE_2023184007 * BOARD_SIZE_2023184007, EMPTY_2023184007);
    g_boardHash_2023184007 = 0;
    g_transpositionTable_2023184007.clear();
    g_evalCache_2023184007.clear();
}

static pair<int, int> chooseActiveBattleAIMove_2023184007(int color)
{
    pair<int, int> move = findBestMove_2023184007(color);
    if (!isEmpty_2023184007(move.first, move.second)) {
        move = make_pair(9, 9);
        if (!isLegalMove_2023184007(move.first, move.second, color)) {
            for (int yy = 0; yy < BOARD_SIZE_2023184007; ++yy) {
                for (int xx = 0; xx < BOARD_SIZE_2023184007; ++xx) {
                    if (isLegalMove_2023184007(xx, yy, color)) {
                        move = make_pair(xx, yy);
                        yy = BOARD_SIZE_2023184007;
                        break;
                    }
                }
            }
        }
    }

    return move;
}

static int scoreActiveBattleAIMove_2023184007(int x, int y, int color)
{
    if (!isLegalMove_2023184007(x, y, color)) {
        return -INF_SCORE_2023184007 / 4;
    }

    return candidateOrderingScore_2023184007(x, y, color);
}

static void commitActiveBattleAIMove_2023184007(int x, int y, int color)
{
    if (inRange_2023184007(x, y) && board_2023184007[y][x] == EMPTY_2023184007) {
        setBoardCell_2023184007(x, y, color);
    }
}

static int validSlot_2023184007(int slot)
{
    return slot >= 0 && slot < 2 ? slot : 0;
}

static BattleAIStrategy_2023184007 sanitizeStrategy_2023184007(BattleAIStrategy_2023184007 strategy)
{
    strategy.attackWeightBlack = max(50, min(180, strategy.attackWeightBlack));
    strategy.attackWeightWhite = max(50, min(180, strategy.attackWeightWhite));
    strategy.defenceWeightBlack = max(50, min(180, strategy.defenceWeightBlack));
    strategy.defenceWeightWhite = max(50, min(180, strategy.defenceWeightWhite));
    strategy.openFourScore = max(1000000, min(30000000, strategy.openFourScore));
    strategy.closedFourScore = max(100000, min(5000000, strategy.closedFourScore));
    strategy.fourThreeScore = max(100000, min(5000000, strategy.fourThreeScore));
    strategy.openThreeScore = max(10000, min(500000, strategy.openThreeScore));
    strategy.brokenThreeScore = max(5000, min(300000, strategy.brokenThreeScore));
    strategy.openTwoScore = max(500, min(30000, strategy.openTwoScore));
    strategy.centerWeightPercent = max(20, min(250, strategy.centerWeightPercent));
    strategy.neighborWeightPercent = max(20, min(250, strategy.neighborWeightPercent));
    strategy.threatPriorityPercent = max(50, min(180, strategy.threatPriorityPercent));
    strategy.defenceUrgencyPercent = max(70, min(200, strategy.defenceUrgencyPercent));
    strategy.maxCandidates = max(12, min(80, strategy.maxCandidates));
    strategy.searchCandidates = max(3, min(16, strategy.searchCandidates));
    strategy.rootSearchCandidates = max(3, min(18, strategy.rootSearchCandidates));
    strategy.maxSearchDepth = max(2, min(4, strategy.maxSearchDepth));
    strategy.moveTimeLimitMs = max(100, min(500, strategy.moveTimeLimitMs));
    strategy.timeSafetyMarginMs = max(50, min(300, strategy.timeSafetyMarginMs));
    return strategy;
}

BattleAIStrategy_2023184007 DefaultBattleAIStrategy_2023184007()
{
    return BattleAIStrategy_2023184007{};
}

void SetBattleAIStrategy_2023184007(int slot, const BattleAIStrategy_2023184007& strategy)
{
    BattleAIState_2023184007& state = g_states_2023184007[validSlot_2023184007(slot)];
    state.strategy = sanitizeStrategy_2023184007(strategy);
    state.transpositionTable.clear();
    state.evalCache.clear();
}

void ResetBattleAIInstance_2023184007(int slot)
{
    BattleAIState_2023184007* previous = g_activeState_2023184007;
    g_activeState_2023184007 = &g_states_2023184007[validSlot_2023184007(slot)];
    resetActiveBattleAI_2023184007();
    g_activeState_2023184007 = previous;
}

pair<int, int> ChooseBattleAIInstanceMove_2023184007(int slot, int color)
{
    BattleAIState_2023184007* previous = g_activeState_2023184007;
    g_activeState_2023184007 = &g_states_2023184007[validSlot_2023184007(slot)];
    pair<int, int> move = chooseActiveBattleAIMove_2023184007(color);
    g_activeState_2023184007 = previous;
    return move;
}

void CommitBattleAIInstanceMove_2023184007(int slot, int x, int y, int color)
{
    BattleAIState_2023184007* previous = g_activeState_2023184007;
    g_activeState_2023184007 = &g_states_2023184007[validSlot_2023184007(slot)];
    commitActiveBattleAIMove_2023184007(x, y, color);
    g_activeState_2023184007 = previous;
}

int DiagnoseBattleAIInstanceMove_2023184007(int slot, int x, int y, int color)
{
    BattleAIState_2023184007* previous = g_activeState_2023184007;
    g_activeState_2023184007 = &g_states_2023184007[validSlot_2023184007(slot)];
    int flags = 0;
    if (isEmpty_2023184007(x, y)) {
        flags |= 1;
        if (color == BLACK_2023184007) {
            setBoardCell_2023184007(x, y, BLACK_2023184007);
            if (isOverline_2023184007(x, y, BLACK_2023184007)) flags |= 2;
            if (isFive_2023184007(x, y, BLACK_2023184007)) flags |= 4;
            if (isDoubleFour_2023184007(x, y)) flags |= 8;
            if (isDoubleThree_2023184007(x, y)) flags |= 16;
            clearBoardCell_2023184007(x, y);
        }
        if (isLegalMove_2023184007(x, y, color)) flags |= 32;
    }
    g_activeState_2023184007 = previous;
    return flags;
}

int TacticalRankBattleAIInstanceMove_2023184007(int slot, int x, int y, int color)
{
    BattleAIState_2023184007* previous = g_activeState_2023184007;
    g_activeState_2023184007 = &g_states_2023184007[validSlot_2023184007(slot)];
    int rank = tacticalRankAfterMove_2023184007(x, y, color);
    g_activeState_2023184007 = previous;
    return rank;
}

int ScoreBattleAIInstanceMove_2023184007(int slot, int x, int y, int color)
{
    BattleAIState_2023184007* previous = g_activeState_2023184007;
    g_activeState_2023184007 = &g_states_2023184007[validSlot_2023184007(slot)];
    int score = scoreActiveBattleAIMove_2023184007(x, y, color);
    g_activeState_2023184007 = previous;
    return score;
}

BattleAISearchStats_2023184007 GetBattleAIInstanceSearchStats_2023184007(int slot)
{
    return g_states_2023184007[validSlot_2023184007(slot)].lastSearchStats;
}

void ResetBattleAI_2023184007()
{
    ResetBattleAIInstance_2023184007(0);
}

pair<int, int> ChooseBattleAIMove_2023184007(int color)
{
    return ChooseBattleAIInstanceMove_2023184007(0, color);
}

int ScoreBattleAIMove_2023184007(int x, int y, int color)
{
    BattleAIState_2023184007* previous = g_activeState_2023184007;
    g_activeState_2023184007 = &g_states_2023184007[0];
    int score = scoreActiveBattleAIMove_2023184007(x, y, color);
    g_activeState_2023184007 = previous;
    return score;
}

void CommitBattleAIMove_2023184007(int x, int y, int color)
{
    CommitBattleAIInstanceMove_2023184007(0, x, y, color);
}

void BlackAttack_2023184007(int* x, int* y)
{
    pair<int, int> move = ChooseBattleAIMove_2023184007(BLACK_2023184007);
    CommitBattleAIMove_2023184007(move.first, move.second, BLACK_2023184007);
    *x = move.first;
    *y = move.second;
}

void BlackDefence_2023184007(int x, int y)
{
    if (inRange_2023184007(x, y) && board_2023184007[y][x] == EMPTY_2023184007) {
        setBoardCell_2023184007(x, y, WHITE_2023184007);
    }
}

void WhiteAttack_2023184007(int* x, int* y)
{
    pair<int, int> move = ChooseBattleAIMove_2023184007(WHITE_2023184007);
    CommitBattleAIMove_2023184007(move.first, move.second, WHITE_2023184007);
    *x = move.first;
    *y = move.second;
}

void WhiteDefence_2023184007(int x, int y)
{
    if (inRange_2023184007(x, y) && board_2023184007[y][x] == EMPTY_2023184007) {
        setBoardCell_2023184007(x, y, BLACK_2023184007);
    }
}

/*
심판 연결 예시:

#include "Judgment.h"

int main()
{
    Judgment judgment;
    judgment.SetYourFunc(BlackAttack_2023184007, BlackDefence_2023184007, WhiteAttack_2023184007, WhiteDefence_2023184007);
    judgment.GamePlay();
    return 0;
}
*/

} // namespace
} // namespace OmokBattleAIImproved_2023184007

#undef board_2023184007
#undef g_boardHash_2023184007
#undef g_deadline_2023184007
#undef g_transpositionTable_2023184007
#undef g_evalCache_2023184007
#undef g_strategy_2023184007

inline void ResetBattleAI_Improved_2023184007() { OmokBattleAIImproved_2023184007::ResetBattleAI_2023184007(); }
inline void BlackAttack_Improved_2023184007(int* x, int* y) { OmokBattleAIImproved_2023184007::BlackAttack_2023184007(x, y); }
inline void BlackDefence_Improved_2023184007(int x, int y) { OmokBattleAIImproved_2023184007::BlackDefence_2023184007(x, y); }
inline void WhiteAttack_Improved_2023184007(int* x, int* y) { OmokBattleAIImproved_2023184007::WhiteAttack_2023184007(x, y); }
inline void WhiteDefence_Improved_2023184007(int x, int y) { OmokBattleAIImproved_2023184007::WhiteDefence_2023184007(x, y); }

inline void ResetBattleAI_2023184007() { OmokBattleAIImproved_2023184007::ResetBattleAI_2023184007(); }
inline void BlackAttack_2023184007(int* x, int* y) { OmokBattleAIImproved_2023184007::BlackAttack_2023184007(x, y); }
inline void BlackDefence_2023184007(int x, int y) { OmokBattleAIImproved_2023184007::BlackDefence_2023184007(x, y); }
inline void WhiteAttack_2023184007(int* x, int* y) { OmokBattleAIImproved_2023184007::WhiteAttack_2023184007(x, y); }
inline void WhiteDefence_2023184007(int x, int y) { OmokBattleAIImproved_2023184007::WhiteDefence_2023184007(x, y); }
