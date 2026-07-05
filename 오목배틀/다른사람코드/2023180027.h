#pragma once
#include <vector>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include <random>
#include <thread>
#include <atomic>
#include <cstring>
#include <mutex>
#include <iostream>
#include <string>

#ifndef NOMINMAX
#define NOMINMAX
#endif

using namespace std;
using namespace chrono;

#define BLACK_2023180027 -1
#define WHITE_2023180027 1
#define EMPTY_2023180027 0
#define SIZE_2023180027 19
#define INF_2023180027 100000000

inline int TIME_LIMIT_MS_2023180027 = 460;

// 유의미한 최적화 상수들 유지
const int SCORE5_2023180027 = 20000000;
const int SCORE4_2023180027 = 2145562;
const int SCORE3_2023180027 = 106101;
const int SCORE2_2023180027 = 1852;
const int POS_BONUS_2023180027 = 1;
const double DEF_WEIGHT_WHITE_2023180027 = 1.003441;
const double DEF_WEIGHT_BLACK_2023180027 = 3.356681;
const int FOUL_TRAP_BONUS_2023180027 = 128123;
const double AGGRESSION_FACTOR_2023180027 = 0.713497;
const double DEPTH_PENALTY_2023180027 = 0.778457;
const int MAX_MOVES_2023180027 = 12;

inline int CURRENT_TURN_2023180027 = 0;

inline int POS_2023180027(int x, int y) { return y * SIZE_2023180027 + x; }

int board_2023180027[361] = { 0, };
int myColor_2023180027 = EMPTY_2023180027;

int minX_2023180027 = SIZE_2023180027, maxX_2023180027 = 0;
int minY_2023180027 = SIZE_2023180027, maxY_2023180027 = 0;

void UpdateBoundingBox_2023180027(int x, int y) {
	if (x < minX_2023180027) minX_2023180027 = max(0, x - 2);
	if (x > maxX_2023180027) maxX_2023180027 = min(SIZE_2023180027 - 1, x + 2);
	if (y < minY_2023180027) minY_2023180027 = max(0, y - 2);
	if (y > maxY_2023180027) maxY_2023180027 = min(SIZE_2023180027 - 1, y + 2);
}

uint64_t zobristTable_2023180027[722];
uint64_t currentHash_2023180027 = 0;

void InitZobrist_2023180027() {
	mt19937_64 rng(12345);
	for (int i = 0; i < 361; i++) {
		zobristTable_2023180027[i * 2 + 0] = rng();
		zobristTable_2023180027[i * 2 + 1] = rng();
	}
}

void UpdateHash_2023180027(int x, int y, int color) {
	int colorIdx = (color == BLACK_2023180027) ? 0 : 1;
	currentHash_2023180027 ^= zobristTable_2023180027[POS_2023180027(x, y) * 2 + colorIdx];
}

enum TTFlag_2023180027 { TT_EXACT_2023180027, TT_LOWER_2023180027, TT_UPPER_2023180027 };
struct TTEntry_2023180027 {
	uint64_t hash;
	int depth;
	int flag;
	int score;
	pair<int, int> bestMove;
};

const int TT_SIZE_2023180027 = 8388593;
TTEntry_2023180027 tt_2023180027[TT_SIZE_2023180027];

pair<int, int> killerMoves_2023180027[100];
int history_2023180027[361];

high_resolution_clock::time_point startTime_2023180027;
inline std::atomic<bool> timeOut_2023180027(false);

struct MoveData_2023180027 {
	int x, y, score;
	bool operator<(const MoveData_2023180027& other) const { return score > other.score; }
};

inline std::mutex rootMutex_2023180027;
inline pair<int, int> bestRootMove_2023180027 = { -1, -1 };
inline int bestRootScore_2023180027 = -INF_2023180027;
inline int maxCompletedDepth_2023180027 = -1;

void CheckTime_2023180027() {
	if (timeOut_2023180027.load(std::memory_order_relaxed)) return;
	auto now = high_resolution_clock::now();
	if (duration_cast<milliseconds>(now - startTime_2023180027).count() > TIME_LIMIT_MS_2023180027) {
		timeOut_2023180027.store(true, std::memory_order_relaxed);
	}
}

bool IsFoul_2023180027(int x, int y, int* localBoard) {
	int dx[] = { 1, 0, 1, -1 }; int dy[] = { 0, 1, 1, 1 };
	int count3 = 0, count4 = 0;

	int p = POS_2023180027(x, y);
	localBoard[p] = BLACK_2023180027;

	for (int dir = 0; dir < 4; ++dir) {
		int line[9];
		int step = dy[dir] * SIZE_2023180027 + dx[dir];
		int current_p = p - step * 4;
		int nx = x - dx[dir] * 4;
		int ny = y - dy[dir] * 4;

		for (int i = -4; i <= 4; ++i) {
			if (nx < 0 || nx >= SIZE_2023180027 || ny < 0 || ny >= SIZE_2023180027) line[i + 4] = WHITE_2023180027;
			else line[i + 4] = localBoard[current_p];

			current_p += step;
			nx += dx[dir]; ny += dy[dir];
		}

		int continuous = 0, max_continuous = 0;
		for (int i = 0; i < 9; ++i) {
			if (line[i] == BLACK_2023180027) { continuous++; max_continuous = max(max_continuous, continuous); }
			else { continuous = 0; }
		}
		if (max_continuous == 5) { localBoard[p] = EMPTY_2023180027; return false; }
		if (max_continuous >= 6) { localBoard[p] = EMPTY_2023180027; return true; }

		bool found4 = false, found3 = false;
		for (int start = 0; start <= 4; ++start) {
			int b_count = 0, e_count = 0;
			for (int i = 0; i < 5; ++i) {
				if (line[start + i] == BLACK_2023180027) b_count++;
				else if (line[start + i] == EMPTY_2023180027) e_count++;
			}
			if (b_count == 4 && e_count == 1) found4 = true;
			if (b_count == 3 && e_count == 2) found3 = true;
		}
		if (found4) count4++; if (found3) count3++;
	}
	localBoard[p] = EMPTY_2023180027;
	if (count4 >= 2 || count3 >= 2) return true;
	return false;
}

// 🌟 상대방 코드와 동등 이상의 '초고속 패턴 식별 알고리즘' 적용
int FastEvaluateMove_2023180027(int x, int y, int color, int* localBoard) {
	int score = 0;
	int dx[] = { 1, 0, 1, -1 };
	int dy[] = { 0, 1, 1, 1 };
	int opp = -color;

	for (int dir = 0; dir < 4; ++dir) {
		int line[9];
		// 9칸 윈도우 배열 추출 (문자열 변환 없이 캐시 메모리에서 초고속 처리)
		for (int i = -4; i <= 4; ++i) {
			int nx = x + dx[dir] * i;
			int ny = y + dy[dir] * i;
			if (nx < 0 || nx >= SIZE_2023180027 || ny < 0 || ny >= SIZE_2023180027) {
				line[i + 4] = opp; // 벽은 상대 돌로 취급
			}
			else if (i == 0) {
				line[i + 4] = color; // 현재 놓을 위치
			}
			else {
				line[i + 4] = localBoard[POS_2023180027(nx, ny)];
			}
		}

		// 5목 확인 (PAT_FIVE)
		int max_contig = 0, contig = 0;
		for (int i = 0; i < 9; ++i) {
			if (line[i] == color) { contig++; max_contig = max(max_contig, contig); }
			else contig = 0;
		}
		if (max_contig >= 5) { score += SCORE5_2023180027; continue; }

		// 세부 패턴 변수
		bool open4 = false;
		int jump4 = 0, close4 = 0;
		int open3 = 0, jump3 = 0, close3 = 0;

		// 5칸씩 슬라이딩하며 내부 패턴 식별 (JUMP 계열 완벽 검출)
		for (int start = 0; start <= 4; ++start) {
			int st = 0, em = 0;
			for (int i = 0; i < 5; ++i) {
				if (line[start + i] == color) st++;
				else if (line[start + i] == EMPTY_2023180027) em++;
			}

			// 4목 패턴 분류 (PAT_OPEN4, PAT_JUMP4, PAT_CLOSE4)
			if (st == 4 && em == 1) {
				int empty_idx = -1;
				for (int i = 0; i < 5; ++i) if (line[start + i] == EMPTY_2023180027) empty_idx = start + i;

				if (empty_idx == start || empty_idx == start + 4) {
					// 연속된 4개일 때 양 끝 확인
					int left = (start > 0) ? line[start - 1] : opp;
					int right = (start + 5 < 9) ? line[start + 5] : opp;
					if (left == EMPTY_2023180027 && right == EMPTY_2023180027) open4 = true; // 양쪽이 열림 (OPEN4)
					else close4++; // 한쪽만 열림 (CLOSE4)
				}
				else {
					jump4++; // 중간이 비어있음 (JUMP4)
				}
			}
			// 3목 패턴 분류 (PAT_OPEN3, PAT_JUMP3, PAT_CLOSE3)
			else if (st == 3 && em == 2) {
				int e1 = -1, e2 = -1;
				for (int i = 0; i < 5; ++i) {
					if (line[start + i] == EMPTY_2023180027) {
						if (e1 == -1) e1 = start + i;
						else e2 = start + i;
					}
				}

				if (e1 == start && e2 == start + 4) open3++; // 완벽히 열린 3 (OPEN3)
				else if (e1 > start && e2 < start + 4) jump3++; // 희귀한 JUMP3
				else {
					int left = (start > 0) ? line[start - 1] : opp;
					int right = (start + 5 < 9) ? line[start + 5] : opp;
					if (left == EMPTY_2023180027 || right == EMPTY_2023180027) {
						if (e1 == start || e2 == start + 4) close3++; // 닫힌 3
						else jump3++; // 한칸 띄워진 열린 3 (JUMP3)
					}
					else {
						close3++;
					}
				}
			}
			else if (st == 2 && em == 3) {
				score += SCORE2_2023180027; // PAT_TWO
			}
		}

		// 탐색의 핵심: 패턴별 정확한 가중치 부여 (기존 임계점과 완벽 호환)
		if (open4) score += SCORE4_2023180027 + 500000;         // PAT_OPEN4
		else if (jump4 > 0) score += SCORE4_2023180027 + 250000; // PAT_JUMP4
		else if (close4 > 0) score += SCORE4_2023180027 + 50000; // PAT_CLOSE4
		else if (open3 > 0) score += SCORE3_2023180027 + 20000;  // PAT_OPEN3
		else if (jump3 > 0) score += SCORE3_2023180027 + 10000;  // PAT_JUMP3
		else if (close3 > 0) score += SCORE3_2023180027;         // PAT_CLOSE3
	}
	return score;
}

int EvaluateColor_2023180027(int color, int* localBoard, int* intersectionBonus = nullptr) {
	int score = 0;
	int dx[] = { 1, 0, 1, -1 }; int dy[] = { 0, 1, 1, 1 };

	for (int y = minY_2023180027; y <= maxY_2023180027; ++y) {
		int rowStart = y * SIZE_2023180027;
		for (int x = minX_2023180027; x <= maxX_2023180027; ++x) {
			int p = rowStart + x;

			for (int dir = 0; dir < 4; ++dir) {
				int nx = x + dx[dir] * 4; int ny = y + dy[dir] * 4;
				if (nx < 0 || nx >= SIZE_2023180027 || ny < 0 || ny >= SIZE_2023180027) continue;

				int stones = 0, empty = 0;
				int step = dy[dir] * SIZE_2023180027 + dx[dir];
				int current_p = p;

				for (int i = 0; i < 5; ++i) {
					if (localBoard[current_p] == color) stones++;
					else if (localBoard[current_p] == EMPTY_2023180027) empty++;
					current_p += step;
				}

				if (stones + empty == 5) {
					int lineScore = 0;
					if (stones == 5) lineScore = SCORE5_2023180027;
					else if (stones == 4) lineScore = SCORE4_2023180027;
					else if (stones == 3) {
						int end_p = p + step * 4;
						if (localBoard[p] == EMPTY_2023180027 && localBoard[end_p] == EMPTY_2023180027) {
							lineScore = SCORE3_2023180027 * 3;
						}
						else {
							lineScore = SCORE3_2023180027;
						}
					}
					else if (stones == 2) lineScore = SCORE2_2023180027;

					score += lineScore;
					if (intersectionBonus != nullptr && stones >= 2) {
						current_p = p;
						for (int i = 0; i < 5; ++i) {
							if (localBoard[current_p] == EMPTY_2023180027) intersectionBonus[current_p] += lineScore;
							current_p += step;
						}
					}
				}
			}
		}
	}
	return score;
}

int EvaluateBoard_2023180027(int* localBoard) {
	int interBonusMy[361] = { 0, };
	int interBonusOpp[361] = { 0, };

	int myScore = EvaluateColor_2023180027(myColor_2023180027, localBoard, interBonusMy);
	int oppScore = EvaluateColor_2023180027(-myColor_2023180027, localBoard, interBonusOpp);

	int maxMyBonus = 0, maxOppBonus = 0;
	int positionalScore = 0, foulTrapScore = 0;

	for (int y = minY_2023180027; y <= maxY_2023180027; ++y) {
		int rowStart = y * SIZE_2023180027;
		for (int x = minX_2023180027; x <= maxX_2023180027; ++x) {
			int p = rowStart + x;
			if (interBonusMy[p] > maxMyBonus) maxMyBonus = interBonusMy[p];
			if (interBonusOpp[p] > maxOppBonus) maxOppBonus = interBonusOpp[p];

			if (localBoard[p] != EMPTY_2023180027) {
				int dist = max(abs(x - 9), abs(y - 9));
				int posWeight = (10 - dist) * POS_BONUS_2023180027;
				if (localBoard[p] == myColor_2023180027) positionalScore += posWeight;
				else positionalScore -= posWeight;
			}
			else {
				int blackBonus = (myColor_2023180027 == BLACK_2023180027) ? interBonusMy[p] : interBonusOpp[p];
				if (blackBonus > 0 && IsFoul_2023180027(x, y, localBoard)) {
					if (myColor_2023180027 == WHITE_2023180027) foulTrapScore += FOUL_TRAP_BONUS_2023180027;
					else foulTrapScore -= FOUL_TRAP_BONUS_2023180027;
				}
			}
		}
	}
	myScore += maxMyBonus + positionalScore + foulTrapScore;
	oppScore += maxOppBonus;

	double defenseWeight = 1.0;
	if (myColor_2023180027 == BLACK_2023180027) {
		defenseWeight = 0.8;
	}
	else {
		defenseWeight = 2.0;
	}

	if (myScore > oppScore * 1.5 && oppScore < SCORE3_2023180027) {
		defenseWeight *= 0.7;
	}
	else if (oppScore > myScore * 1.2) {
		defenseWeight *= 1.5;
	}

	return myScore - (int)(oppScore * defenseWeight);
}

vector<MoveData_2023180027> GenerateMoves_2023180027(int depth, int* localBoard, int color, uint64_t localHash = 0) {
	vector<MoveData_2023180027> moves;
	moves.reserve(64);
	int dx[] = { -1, -1, -1, 0, 0, 1, 1, 1 }; int dy[] = { -1, 0, 1, -1, 1, -1, 0, 1 };

	static thread_local mt19937 move_rng(random_device{}());

	pair<int, int> ttBestMove = { -1, -1 };
	if (localHash != 0) {
		TTEntry_2023180027 ttEntry = tt_2023180027[localHash % TT_SIZE_2023180027];
		if (ttEntry.hash == localHash) {
			ttBestMove = ttEntry.bestMove;
		}
	}

	for (int y = minY_2023180027; y <= maxY_2023180027; ++y) {
		int rowStart = y * SIZE_2023180027;
		for (int x = minX_2023180027; x <= maxX_2023180027; ++x) {
			int p = rowStart + x;
			if (localBoard[p] != EMPTY_2023180027) continue;

			bool hasNeighbor = false;
			for (int d = 0; d < 8; d++) {
				int nx = x, ny = y;
				for (int step = 1; step <= 2; step++) {
					nx += dx[d]; ny += dy[d];
					if (nx >= 0 && nx < SIZE_2023180027 && ny >= 0 && ny < SIZE_2023180027) {
						if (localBoard[POS_2023180027(nx, ny)] != EMPTY_2023180027) {
							hasNeighbor = true; break;
						}
					}
				}
				if (hasNeighbor) break;
			}

			if (hasNeighbor) {
				if (timeOut_2023180027.load(std::memory_order_relaxed)) return moves;

				if (color == BLACK_2023180027 && IsFoul_2023180027(x, y, localBoard)) continue;

				int myMoveScore = FastEvaluateMove_2023180027(x, y, color, localBoard);
				int oppMoveScore = FastEvaluateMove_2023180027(x, y, -color, localBoard);

				int forcingBonus = 0;
				if (myMoveScore >= 20000000) forcingBonus += 100000000;
				else if (oppMoveScore >= 20000000) forcingBonus += 90000000;
				else if (myMoveScore >= 2600000) forcingBonus += 80000000;
				else if (myMoveScore >= 4000000) forcingBonus += 75000000;
				else if (myMoveScore >= 2300000) forcingBonus += 70000000;
				else if (myMoveScore >= 2100000) forcingBonus += 65000000;
				else if (oppMoveScore >= 2600000) forcingBonus += 60000000;
				else if (oppMoveScore >= 4000000) forcingBonus += 55000000;
				else if (oppMoveScore >= 2300000) forcingBonus += 50000000;
				else if (oppMoveScore >= 2100000) forcingBonus += 45000000;

				int synergyBonus = 0;
				if (oppMoveScore >= SCORE3_2023180027 && myMoveScore >= SCORE2_2023180027) synergyBonus += 2000000;
				else if (oppMoveScore >= SCORE2_2023180027 && myMoveScore >= SCORE3_2023180027) synergyBonus += 1000000;

				int safeDepth = (depth > 0) ? depth : 1;
				int noise = (CURRENT_TURN_2023180027 <= 15) ? (move_rng() % 5) : 0;

				double myWeight = (myColor_2023180027 == BLACK_2023180027) ? 2.0 : 1.2;
				double oppWeight = (myColor_2023180027 == BLACK_2023180027) ? 1.2 : 2.5;

				int tacScore = (int)(myMoveScore * myWeight) + (int)(oppMoveScore * oppWeight) + synergyBonus;
				int score = history_2023180027[p] + tacScore + forcingBonus + noise;

				if (killerMoves_2023180027[safeDepth * 2 + 0] == make_pair(x, y)) score += 10000;
				else if (killerMoves_2023180027[safeDepth * 2 + 1] == make_pair(x, y)) score += 5000;

				if (ttBestMove == make_pair(x, y)) score += 500000000;

				moves.push_back({ x, y, score });
			}
		}
	}

	int sortCount = min((int)moves.size(), MAX_MOVES_2023180027 + 5);
	partial_sort(moves.begin(), moves.begin() + sortCount, moves.end());
	return moves;
}

int QuiescenceSearch_2023180027(int alpha, int beta, int color, int* localBoard, int qDepth = 0) {
	static thread_local int qNodeCounter = 0;
	if (++qNodeCounter > 256) { CheckTime_2023180027(); qNodeCounter = 0; }
	if (timeOut_2023180027.load(std::memory_order_relaxed)) return 0;

	int eval = EvaluateBoard_2023180027(localBoard);
	int score = (color == myColor_2023180027) ? eval : -eval;

	if (abs(eval) >= SCORE5_2023180027 / 2) return score;
	if (qDepth >= 6) return score;

	if (score >= beta) return beta;
	if (score > alpha) alpha = score;

	vector<MoveData_2023180027> moves = GenerateMoves_2023180027(0, localBoard, color);

	for (auto& move : moves) {
		if (move.score < 2000000) continue;

		int p = POS_2023180027(move.x, move.y);
		localBoard[p] = color;

		int childScore = -QuiescenceSearch_2023180027(-beta, -alpha, -color, localBoard, qDepth + 1);

		localBoard[p] = EMPTY_2023180027;

		if (timeOut_2023180027.load(std::memory_order_relaxed)) return 0;

		if (childScore >= beta) return beta;
		if (childScore > alpha) alpha = childScore;
	}

	return alpha;
}

int PVS_Worker_2023180027(int depth, int alpha, int beta, int color, int* localBoard, uint64_t localHash, bool isNullMove = false, bool isRoot = false) {
	static thread_local int nodeCounter = 0;
	if (++nodeCounter > 256) { CheckTime_2023180027(); nodeCounter = 0; }
	if (timeOut_2023180027.load(std::memory_order_relaxed)) return 0;

	int ttIndex = localHash % TT_SIZE_2023180027;
	TTEntry_2023180027 ttEntry = tt_2023180027[ttIndex];

	if (ttEntry.hash == localHash && ttEntry.depth >= depth) {
		if (ttEntry.flag == TT_EXACT_2023180027) return ttEntry.score;
		if (ttEntry.flag == TT_LOWER_2023180027) alpha = max(alpha, ttEntry.score);
		if (ttEntry.flag == TT_UPPER_2023180027) beta = min(beta, ttEntry.score);
		if (alpha >= beta) return ttEntry.score;
	}

	int eval = EvaluateBoard_2023180027(localBoard);
	int score = (color == myColor_2023180027) ? eval : -eval;

	if (abs(eval) >= SCORE5_2023180027 / 2) return score;

	if (depth <= 0) {
		return QuiescenceSearch_2023180027(alpha, beta, color, localBoard, 0);
	}

	if (!isNullMove && depth >= 3) {
		if (eval >= beta) {
			int nullScore = -PVS_Worker_2023180027(depth - 3, -beta, -beta + 1, -color, localBoard, localHash, true, false);
			if (nullScore >= beta) return nullScore;
		}
	}

	vector<MoveData_2023180027> moves = GenerateMoves_2023180027(depth, localBoard, color, localHash);
	if (moves.empty()) return score;

	int dynamicMaxMoves = MAX_MOVES_2023180027;
	if (moves[0].score >= 2300000) dynamicMaxMoves += 5;
	if (moves.size() > dynamicMaxMoves) moves.resize(dynamicMaxMoves);

	int bestScore = -INF_2023180027;
	int alphaOrig = alpha; pair<int, int> bestMove = { -1, -1 };
	int moveCount = 0;

	for (auto& move : moves) {
		int p = POS_2023180027(move.x, move.y);
		localBoard[p] = color;
		int colorIdx = (color == BLACK_2023180027) ? 0 : 1;
		localHash ^= zobristTable_2023180027[p * 2 + colorIdx];

		int childScore;
		int extension = 0;
		if (move.score >= 40000000) extension = 1;

		if (moveCount == 0) {
			childScore = -PVS_Worker_2023180027(depth - 1 + extension, -beta, -alpha, -color, localBoard, localHash, false, false);
		}
		else {
			int reduction = 0;
			if (depth >= 3 && moveCount >= 4 && extension == 0) {
				if (move.score < 500000) {
					reduction = 1;
				}
			}

			childScore = -PVS_Worker_2023180027(depth - 1 - reduction + extension, -alpha - 1, -alpha, -color, localBoard, localHash, false, false);

			if (childScore > alpha && childScore < beta) {
				childScore = -PVS_Worker_2023180027(depth - 1 + extension, -beta, -alpha, -color, localBoard, localHash, false, false);
			}
		}

		localHash ^= zobristTable_2023180027[p * 2 + colorIdx];
		localBoard[p] = EMPTY_2023180027;
		moveCount++;

		if (timeOut_2023180027.load(std::memory_order_relaxed)) return 0;

		if (childScore > 10000000) childScore -= 1;
		else if (childScore < -10000000) childScore += 1;

		if (childScore > bestScore) { bestScore = childScore; bestMove = { move.x, move.y }; }
		if (childScore > alpha) {
			alpha = childScore;
			if (depth > 0) history_2023180027[p] += depth * depth;
		}
		if (alpha >= beta) {
			if (depth > 0 && killerMoves_2023180027[depth * 2 + 0] != make_pair(move.x, move.y)) {
				killerMoves_2023180027[depth * 2 + 1] = killerMoves_2023180027[depth * 2 + 0];
				killerMoves_2023180027[depth * 2 + 0] = { move.x, move.y };
			}
			break;
		}
	}

	if (!timeOut_2023180027.load(std::memory_order_relaxed)) {
		if (bestScore <= -INF_2023180027 + 100) bestScore += (depth > 0 ? depth : 0);

		tt_2023180027[ttIndex] = { localHash, depth, (bestScore <= alphaOrig) ? TT_UPPER_2023180027 : ((bestScore >= beta) ? TT_LOWER_2023180027 : TT_EXACT_2023180027), bestScore, bestMove };

		if (isRoot) {
			lock_guard<mutex> rootLock(rootMutex_2023180027);
			if (depth > maxCompletedDepth_2023180027 || (depth == maxCompletedDepth_2023180027 && bestScore > bestRootScore_2023180027)) {
				bestRootMove_2023180027 = bestMove; bestRootScore_2023180027 = bestScore; maxCompletedDepth_2023180027 = depth;
			}
		}
	}
	return bestScore;
}

void LazySMP_Worker(int color, int thread_id, int max_depth) {
	int localBoard[361];
	memcpy(localBoard, board_2023180027, sizeof(int) * 361);
	uint64_t localHash = currentHash_2023180027;

	int start_depth = 1 + (thread_id % 3);
	int prev_score = 0;

	for (int depth = start_depth; depth <= max_depth; depth++) {
		int alpha = -INF_2023180027;
		int beta = INF_2023180027;

		if (depth >= 4) {
			alpha = prev_score - 150000;
			beta = prev_score + 150000;
		}

		int score = PVS_Worker_2023180027(depth, alpha, beta, color, localBoard, localHash, false, true);
		if (timeOut_2023180027.load(std::memory_order_relaxed)) break;

		if (score <= alpha || score >= beta) {
			score = PVS_Worker_2023180027(depth, -INF_2023180027, INF_2023180027, color, localBoard, localHash, false, true);
			if (timeOut_2023180027.load(std::memory_order_relaxed)) break;
		}

		prev_score = score;
		if (score >= SCORE5_2023180027 / 2) break;
	}
}

pair<int, int> FindUrgentMove_2023180027() {
	int oppColor = -myColor_2023180027;

	pair<int, int> myMake5 = { -1, -1 }, oppMake5 = { -1, -1 };
	pair<int, int> myMakeOpen4 = { -1, -1 };
	pair<int, int> myMakeDouble4 = { -1, -1 }, oppMakeDouble4 = { -1, -1 };
	pair<int, int> myMake4_3 = { -1, -1 }, oppMake4_3 = { -1, -1 };

	pair<int, int> oppMakeOpen4 = { -1, -1 };
	int bestOppOpen4BlockScore = -1;

	pair<int, int> oppMakeDouble3 = { -1, -1 };
	int bestOppDouble3BlockScore = -1;

	for (int y = minY_2023180027; y <= maxY_2023180027; ++y) {
		int rowStart = y * SIZE_2023180027;
		for (int x = minX_2023180027; x <= maxX_2023180027; ++x) {
			int p = rowStart + x;
			if (board_2023180027[p] == EMPTY_2023180027) {

				if (myColor_2023180027 == BLACK_2023180027 && IsFoul_2023180027(x, y, board_2023180027)) continue;

				int myS = FastEvaluateMove_2023180027(x, y, myColor_2023180027, board_2023180027);
				int opS = FastEvaluateMove_2023180027(x, y, oppColor, board_2023180027);

				if (myS >= 20000000) myMake5 = { x, y };
				else if (myS >= 4000000) myMakeDouble4 = { x, y };
				else if (myS >= 2600000) myMakeOpen4 = { x, y };
				else if (myS >= 2300000) myMake4_3 = { x, y };

				if (opS >= 20000000) oppMake5 = { x, y };
				else if (opS >= 4000000) oppMakeDouble4 = { x, y };
				else if (opS >= 2600000) {
					if (myS > bestOppOpen4BlockScore) { bestOppOpen4BlockScore = myS; oppMakeOpen4 = { x, y }; }
				}
				else if (opS >= 2300000) oppMake4_3 = { x, y };
				else if (opS >= 250000) {
					if (myS > bestOppDouble3BlockScore) { bestOppDouble3BlockScore = myS; oppMakeDouble3 = { x, y }; }
				}
			}
		}
	}

	if (myMake5.first != -1) return myMake5;
	if (oppMake5.first != -1) return oppMake5;
	if (myMakeOpen4.first != -1) return myMakeOpen4;
	if (myMakeDouble4.first != -1) return myMakeDouble4;
	if (myMake4_3.first != -1) return myMake4_3;

	if (oppMakeOpen4.first != -1) return oppMakeOpen4;
	if (oppMakeDouble4.first != -1) return oppMakeDouble4;
	if (oppMake4_3.first != -1) return oppMake4_3;
	if (oppMakeDouble3.first != -1) return oppMakeDouble3;

	return { -1, -1 };
}

void SearchBestMove_2023180027(int* x, int* y, int color) {
	myColor_2023180027 = color;
	startTime_2023180027 = high_resolution_clock::now();
	timeOut_2023180027.store(false, std::memory_order_relaxed);

	bestRootMove_2023180027 = { -1, -1 };
	bestRootScore_2023180027 = -INF_2023180027;
	maxCompletedDepth_2023180027 = -1;

	minX_2023180027 = max(0, minX_2023180027 - 4); maxX_2023180027 = min(SIZE_2023180027 - 1, maxX_2023180027 + 4);
	minY_2023180027 = max(0, minY_2023180027 - 4); maxY_2023180027 = min(SIZE_2023180027 - 1, maxY_2023180027 + 4);

	int stoneCount = 0;
	int lastOpponentX = 9, lastOpponentY = 9;

	for (int i = 0; i < SIZE_2023180027; i++) {
		for (int j = 0; j < SIZE_2023180027; j++) {
			if (board_2023180027[POS_2023180027(j, i)] != EMPTY_2023180027) {
				UpdateBoundingBox_2023180027(j, i);
				stoneCount++;
				if (board_2023180027[POS_2023180027(j, i)] != color) {
					lastOpponentX = j; lastOpponentY = i;
				}
			}
		}
	}
	CURRENT_TURN_2023180027 = stoneCount;

	if (stoneCount == 0) {
		*x = SIZE_2023180027 / 2; *y = SIZE_2023180027 / 2;
		board_2023180027[POS_2023180027(*x, *y)] = color; UpdateHash_2023180027(*x, *y, color); UpdateBoundingBox_2023180027(*x, *y); return;
	}
	else if (stoneCount == 1 && color == WHITE_2023180027) {
		int nx = lastOpponentX + 1, ny = lastOpponentY + 1;
		if (nx >= SIZE_2023180027 || ny >= SIZE_2023180027) { nx = lastOpponentX - 1; ny = lastOpponentY - 1; }
		*x = nx; *y = ny;
		board_2023180027[POS_2023180027(*x, *y)] = color; UpdateHash_2023180027(*x, *y, color); UpdateBoundingBox_2023180027(*x, *y); return;
	}

	pair<int, int> urgentMove = FindUrgentMove_2023180027();
	if (urgentMove.first != -1) {
		*x = urgentMove.first; *y = urgentMove.second;
		board_2023180027[POS_2023180027(*x, *y)] = color; UpdateHash_2023180027(*x, *y, color); UpdateBoundingBox_2023180027(*x, *y); return;
	}

	int num_threads = thread::hardware_concurrency();
	if (num_threads == 0) num_threads = 4;

	vector<thread> workers;
	int max_search_depth = 22;

	for (int i = 0; i < num_threads; ++i) workers.push_back(thread(LazySMP_Worker, color, i, max_search_depth));
	for (auto& t : workers) { if (t.joinable()) t.join(); }

	pair<int, int> globalBestMove = bestRootMove_2023180027;

	if (globalBestMove.first < 0 || globalBestMove.first >= SIZE_2023180027 || globalBestMove.second < 0 || globalBestMove.second >= SIZE_2023180027 || board_2023180027[POS_2023180027(globalBestMove.first, globalBestMove.second)] != EMPTY_2023180027) {

		if (timeOut_2023180027.load(std::memory_order_relaxed)) {
			for (int dist = 0; dist < SIZE_2023180027; ++dist) {
				for (int i = 0; i < SIZE_2023180027; i++) {
					for (int j = 0; j < SIZE_2023180027; j++) {
						if (max(abs(i - 9), abs(j - 9)) == dist && board_2023180027[POS_2023180027(j, i)] == EMPTY_2023180027) {
							if (color == BLACK_2023180027 && IsFoul_2023180027(j, i, board_2023180027)) continue;
							globalBestMove = { j, i };
							goto found_safe_move;
						}
					}
				}
			}
		}
		else {
			timeOut_2023180027.store(false, std::memory_order_relaxed);
			int bestVal = -INF_2023180027;
			vector<MoveData_2023180027> backup = GenerateMoves_2023180027(1, board_2023180027, color, 0);
			for (auto& m : backup) {
				if (timeOut_2023180027.load(std::memory_order_relaxed)) break;
				board_2023180027[POS_2023180027(m.x, m.y)] = color;
				int val = (color == myColor_2023180027) ? EvaluateBoard_2023180027(board_2023180027) : -EvaluateBoard_2023180027(board_2023180027);
				board_2023180027[POS_2023180027(m.x, m.y)] = EMPTY_2023180027;
				if (val > bestVal) { bestVal = val; globalBestMove = { m.x, m.y }; }
			}
			if (globalBestMove.first < 0) {
				for (int dist = 0; dist < SIZE_2023180027; ++dist) {
					for (int i = 0; i < SIZE_2023180027; i++) {
						for (int j = 0; j < SIZE_2023180027; j++) {
							if (max(abs(i - 9), abs(j - 9)) == dist && board_2023180027[POS_2023180027(j, i)] == EMPTY_2023180027) {
								if (color == BLACK_2023180027 && IsFoul_2023180027(j, i, board_2023180027)) continue;
								globalBestMove = { j, i }; goto found_safe_move;
							}
						}
					}
				}
			}
		}
	found_safe_move:;
	}

	*x = globalBestMove.first; *y = globalBestMove.second;
	board_2023180027[POS_2023180027(*x, *y)] = color; UpdateHash_2023180027(*x, *y, color); UpdateBoundingBox_2023180027(*x, *y);
}

void BlackAttack_2023180027(int* x, int* y) {
	static bool init = false; if (!init) { InitZobrist_2023180027(); init = true; }
	SearchBestMove_2023180027(x, y, BLACK_2023180027);
}
void BlackDefence_2023180027(int x, int y) {
	if (x >= 0 && x < SIZE_2023180027 && y >= 0 && y < SIZE_2023180027) {
		board_2023180027[POS_2023180027(x, y)] = WHITE_2023180027; UpdateHash_2023180027(x, y, WHITE_2023180027); UpdateBoundingBox_2023180027(x, y);
	}
}
void WhiteAttack_2023180027(int* x, int* y) {
	static bool init = false; if (!init) { InitZobrist_2023180027(); init = true; }
	SearchBestMove_2023180027(x, y, WHITE_2023180027);
}
void WhiteDefence_2023180027(int x, int y) {
	if (x >= 0 && x < SIZE_2023180027 && y >= 0 && y < SIZE_2023180027) {
		board_2023180027[POS_2023180027(x, y)] = BLACK_2023180027; UpdateHash_2023180027(x, y, BLACK_2023180027); UpdateBoundingBox_2023180027(x, y);
	}
}

void InitGame_2023180027() {
	ZeroMemory(board_2023180027, sizeof(board_2023180027));
	minX_2023180027 = SIZE_2023180027; maxX_2023180027 = 0;
	minY_2023180027 = SIZE_2023180027; maxY_2023180027 = 0;
	ZeroMemory(history_2023180027, sizeof(history_2023180027));
	ZeroMemory(killerMoves_2023180027, sizeof(killerMoves_2023180027));

	ZeroMemory(tt_2023180027, sizeof(tt_2023180027));
	currentHash_2023180027 = 0;
	InitZobrist_2023180027();
	CURRENT_TURN_2023180027 = 0;
}