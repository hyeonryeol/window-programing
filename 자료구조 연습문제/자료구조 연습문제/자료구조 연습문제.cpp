#include <iostream>
#include <fstream>
#include <string>
#include <stack>
using namespace std;

char board[19][19];
bool turn = false;
int wstonecount = 0;
int bstonecount = 0;
int garowboard[19];
int serowboard[19];
int garobboard[19];
int serobboard[19];

struct Gamestate {
	char board[19][19];
	bool turn;
	int wstonecount;
	int bstonecount;
};

stack<Gamestate> undostack;
stack<Gamestate> redostack;

Gamestate capture()
{
	Gamestate s;
	for (int i = 0; i < 19; ++i)
		for (int j = 0; j < 19; ++j)
			s.board[i][j] = board[i][j];
	s.turn = turn;
	s.wstonecount = wstonecount;
	s.bstonecount = bstonecount;
	return s;
}

void restorestate(const Gamestate& s)
{
	for (int i = 0; i < 19; ++i)
		for (int j = 0; j < 19; ++j)
			board[i][j] = s.board[i][j];
	turn = s.turn;
	wstonecount = s.wstonecount;
	bstonecount = s.bstonecount;
}
void savetofile()
{
	ofstream f("omok_save.txt");
	if (!f) { cout << "파일 저장 오류" << endl; return; }
	f << (int)turn << " " << wstonecount << " " << bstonecount << "\n";
	for (int i = 0; i < 19; ++i)
	{
		for (int j = 0; j < 19; ++j)
			f << board[i][j];
		f << "\n";
	}
	cout << "저장완료" << endl;
}
void inboard()
{
	for (int i = 0; i < 19; ++i)
		for (int j = 0; j < 19; ++j)
			board[i][j] = '+';
}

void printboard()
{
	for (int i = 0; i < 19; ++i)
	{
		for (int j = 0; j < 19; ++j)
			cout << board[i][j] << " ";
		cout << endl;
	}
}
void loadfromfile()
{
	ifstream f("omok_save.txt");
	if (!f) { cout << "파일 불러오기 오류" << endl; return; }
	
	int t;
	f >> t >> wstonecount >> bstonecount;
	turn=(bool)t;
	string row;
	for (int i = 0; i < 19; ++i)
	{
		f >> row;
		for (int j = 0; j < 19; ++j)
			board[i][j] = row[j];
	}

	while (!undostack.empty()) undostack.pop();
	while (!redostack.empty()) redostack.pop();
	cout << "불러오기 완료" << endl;
	printboard();
}
void position(int garo, int sero, char stone)
{
	board[garo][sero] = stone;
}

// 수정 1 2 3 4 반영
void printmulti()
{
	cout << "2번 구현" << endl;

	// 가로줄: 각 줄별 흰 검 최대 연속 좌표 출력, 동률이면 둘 다 출력
	for (int i = 0; i < 19; ++i)
	{
		int wmulti = 0, wmax = 0, wstart = 0, wbest = 0;
		int bmulti = 0, bmax = 0, bstart = 0, bbest = 0;

		for (int j = 0; j < 19; ++j)
		{
			if (board[i][j] == '0')
			{
				if (wmulti == 0) wstart = j;
				wmulti++; bmulti = 0;
				if (wmulti > wmax) { wmax = wmulti; wbest = wstart; }
			}
			else if (board[i][j] == 'X')
			{
				if (bmulti == 0) bstart = j;
				bmulti++; wmulti = 0;
				if (bmulti > bmax) { bmax = bmulti; bbest = bstart; }
			}
			else { wmulti = 0; bmulti = 0; }
		}

		cout << i << "번째 가로줄: ";
		if (wmax == 0 && bmax == 0) { cout << "없음" << endl; continue; }

		// 흰 돌 출력
		if (wmax > 0)
		{
			cout << "흰 돌 " << wmax << "개 ";
			for (int k = 0; k < wmax; k++)
				cout << "(" << wbest + k << "," << i << ") ";
		}
		if (wmax > 0 && bmax > 0) cout << "| ";
		// 검은 돌 출력 (동률이어도 출력)
		if (bmax > 0)
		{
			cout << "검은 돌 " << bmax << "개 ";
			for (int k = 0; k < bmax; k++)
				cout << "(" << bbest + k << "," << i << ") ";
		}
		cout << endl;
	}

	// 세로줄: 각 줄별 흰 검 최대 연속 좌표 출력, 동률이면 둘 다 출력
	for (int i = 0; i < 19; ++i)
	{
		int wmulti = 0, wmax = 0, wstart = 0, wbest = 0;
		int bmulti = 0, bmax = 0, bstart = 0, bbest = 0;

		for (int j = 0; j < 19; ++j)
		{
			if (board[j][i] == '0')
			{
				if (wmulti == 0) wstart = j;
				wmulti++; bmulti = 0;
				if (wmulti > wmax) { wmax = wmulti; wbest = wstart; }
			}
			else if (board[j][i] == 'X')
			{
				if (bmulti == 0) bstart = j;
				bmulti++; wmulti = 0;
				if (bmulti > bmax) { bmax = bmulti; bbest = bstart; }
			}
			else { wmulti = 0; bmulti = 0; }
		}

		cout << i << "번째 세로줄: ";
		if (wmax == 0 && bmax == 0) { cout << "없음" << endl; continue; }

		if (wmax > 0)
		{
			cout << "흰 돌 " << wmax << "개 ";
			for (int k = 0; k < wmax; k++)
				cout << "(" << i << "," << wbest + k << ") ";
		}
		if (wmax > 0 && bmax > 0) cout << "| ";
		if (bmax > 0)
		{
			cout << "검은 돌 " << bmax << "개 ";
			for (int k = 0; k < bmax; k++)
				cout << "(" << i << "," << bbest + k << ") ";
		}
		cout << endl;
	}
}

// 기능 1 과 기능 2 추가

struct StreakResult {
	int count;
	char color;
	int startIdx;
	int endLineIdx;
	int gapIdx;
};

StreakResult calcStreak(char line[], int len, int start, char color)
{
	StreakResult res = { 0, color, start, start - 1, -1 };
	if (start >= len || line[start] != color) return res;
	res.count = 1;
	res.endLineIdx = start;
	bool gapUsed = false;
	for (int k = start + 1; k < len; k++)
	{
		if (line[k] == color) { res.count++; res.endLineIdx = k; }
		else if (line[k] == '+' && !gapUsed && k + 1 < len && line[k + 1] == color)
		{
			gapUsed = true; res.gapIdx = k; k++;
			res.count++; res.endLineIdx = k;
		}
		else break;
	}
	return res;
}

void feature1_printMaxGap()
{
	cout << "\n[기능 1] 공백 1칸 포함 최대 연속 돌" << endl;

	int globalMax = 0;
	char line[19];
	int lr[19], lc[19];

	auto findMax = [&](int len) {
		for (int s = 0; s < len; s++)
			if (line[s] == '0' || line[s] == 'X')
			{
				StreakResult res = calcStreak(line, len, s, line[s]);
				if (res.count > globalMax) globalMax = res.count;
			}
		};

	struct SeenStreak { int sr, sc, er, ec; };
	SeenStreak seen[400];
	int seenCount = 0;

	auto alreadySeen = [&](int sr, int sc, int er, int ec) -> bool {
		for (int i = 0; i < seenCount; i++)
			if (seen[i].sr == sr && seen[i].sc == sc &&
				seen[i].er == er && seen[i].ec == ec) return true;
		return false;
		};

	auto printAllMax = [&](int len) {
		for (int s = 0; s < len; s++)
			if ((line[s] == '0' || line[s] == 'X') &&
				(s == 0 || line[s - 1] != line[s]))
			{
				StreakResult res = calcStreak(line, len, s, line[s]);
				if (res.count == globalMax)
				{
					int sr = lr[res.startIdx], sc = lc[res.startIdx];
					int er = lr[res.endLineIdx], ec = lc[res.endLineIdx];
					if (alreadySeen(sr, sc, er, ec)) continue;
					seen[seenCount++] = { sr, sc, er, ec };

					cout << (res.color == '0' ? "흰" : "검은") << " 돌 " << res.count << "개";
					if (res.gapIdx != -1) cout << " (공백 1칸 포함)";
					cout << "\n좌표: ";
					for (int i = res.startIdx; i <= res.endLineIdx; i++)
					{
						if (i == res.gapIdx) continue;
						cout << "(" << lc[i] << "," << lr[i] << ") ";
					}
					if (res.gapIdx != -1)
						cout << "[공백위치: (" << lc[res.gapIdx] << "," << lr[res.gapIdx] << ")]";
					cout << endl;
				}
			}
		};

	auto iterAllLines = [&](auto func) {
		for (int i = 0; i < 19; i++) {
			for (int j = 0; j < 19; j++) { line[j] = board[i][j]; lr[j] = i; lc[j] = j; }
			func(19);
		}
		for (int j = 0; j < 19; j++) {
			for (int i = 0; i < 19; i++) { line[i] = board[i][j]; lr[i] = i; lc[i] = j; }
			func(19);
		}
		for (int start = 0; start < 19; start++) {
			int len = 0;
			for (int k = 0; start + k < 19 && k < 19; k++) { line[len] = board[k][start + k]; lr[len] = k; lc[len] = start + k; len++; }
			func(len);
		}
		for (int start = 1; start < 19; start++) {
			int len = 0;
			for (int k = 0; start + k < 19 && k < 19; k++) { line[len] = board[start + k][k]; lr[len] = start + k; lc[len] = k; len++; }
			func(len);
		}
		for (int start = 0; start < 19; start++) {
			int len = 0;
			for (int k = 0; 18 - k >= 0 && start + k < 19; k++) { line[len] = board[18 - k][start + k]; lr[len] = 18 - k; lc[len] = start + k; len++; }
			func(len);
		}
		for (int start = 17; start >= 0; start--) {
			int len = 0;
			for (int k = 0; start - k >= 0 && k < 19; k++) { line[len] = board[start - k][k]; lr[len] = start - k; lc[len] = k; len++; }
			func(len);
		}
		};

	iterAllLines(findMax);
	if (globalMax == 0) { cout << "돌이 없습니다." << endl; return; }
	iterAllLines(printAllMax);
}

void feature2_printBlock(char opponentColor)
{
	cout << "\n[기능 2] " << (opponentColor == '0' ? "흰" : "검은") << " 돌 3·4 차단 위치" << endl;

	bool found = false;
	char line[19];
	int lr[19], lc[19];

	auto checkLine = [&](int len) {
		for (int s = 0; s < len; s++)
			if (line[s] == opponentColor && (s == 0 || line[s - 1] != opponentColor))
			{
				StreakResult res = calcStreak(line, len, s, opponentColor);
				if (res.count == 3 || res.count == 4)
				{
					found = true;
					cout << res.count << "개 연속 발견! 차단 위치: ";
					int before = res.startIdx - 1;
					if (before >= 0 && line[before] == '+')
						cout << "(" << lc[before] << "," << lr[before] << ") ";
					int after = res.endLineIdx + 1;
					if (after < len && line[after] == '+')
						cout << "(" << lc[after] << "," << lr[after] << ") ";
					if (res.gapIdx != -1)
						cout << "[공백메우기: (" << lc[res.gapIdx] << "," << lr[res.gapIdx] << ")]";
					cout << endl;
				}
			}
		};

	for (int i = 0; i < 19; i++) {
		for (int j = 0; j < 19; j++) { line[j] = board[i][j]; lr[j] = i; lc[j] = j; }
		checkLine(19);
	}
	for (int j = 0; j < 19; j++) {
		for (int i = 0; i < 19; i++) { line[i] = board[i][j]; lr[i] = i; lc[i] = j; }
		checkLine(19);
	}
	for (int start = 0; start < 19; start++) {
		int len = 0;
		for (int k = 0; start + k < 19 && k < 19; k++) { line[len] = board[k][start + k]; lr[len] = k; lc[len] = start + k; len++; }
		checkLine(len);
	}
	for (int start = 1; start < 19; start++) {
		int len = 0;
		for (int k = 0; start + k < 19 && k < 19; k++) { line[len] = board[start + k][k]; lr[len] = start + k; lc[len] = k; len++; }
		checkLine(len);
	}
	for (int start = 0; start < 19; start++) {
		int len = 0;
		for (int k = 0; 18 - k >= 0 && start + k < 19; k++) { line[len] = board[18 - k][start + k]; lr[len] = 18 - k; lc[len] = start + k; len++; }
		checkLine(len);
	}
	for (int start = 17; start >= 0; start--) {
		int len = 0;
		for (int k = 0; start - k >= 0 && k < 19; k++) { line[len] = board[start - k][k]; lr[len] = start - k; lc[len] = k; len++; }
		checkLine(len);
	}

	if (!found) cout << "차단이 필요한 위치 없음" << endl;
}

void crossline()
{
	for (int start = 0; start < 19; ++start)
	{
		int wmulti = 0, wmax = 0, bmulti = 0, bmax = 0;
		for (int k = 0; start + k < 19 && k < 19; ++k)
		{
			if (board[k][start + k] == '0') { wmulti++; bmulti = 0; }
			else if (board[k][start + k] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }
			if (wmulti > wmax) wmax = wmulti;
			if (bmulti > bmax) bmax = bmulti;
		}
		cout << "우하향 시작(" << start << ",0): ";
		if (wmax == 0 && bmax == 0) cout << "없음" << endl;
		else if (wmax > bmax) cout << "흰 돌 " << wmax << "개" << endl;
		else if (bmax > wmax) cout << "검은 돌 " << bmax << "개" << endl;
		else cout << "흰 돌 " << wmax << "개 | 검은 돌 " << bmax << "개" << endl;
	}

	for (int start = 1; start < 19; ++start)
	{
		int wmulti = 0, wmax = 0, bmulti = 0, bmax = 0;
		for (int k = 0; start + k < 19 && k < 19; ++k)
		{
			if (board[start + k][k] == '0') { wmulti++; bmulti = 0; }
			else if (board[start + k][k] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }
			if (wmulti > wmax) wmax = wmulti;
			if (bmulti > bmax) bmax = bmulti;
		}
		cout << "우하향 시작(0," << start << "): ";
		if (wmax == 0 && bmax == 0) cout << "없음" << endl;
		else if (wmax > bmax) cout << "흰 돌 " << wmax << "개" << endl;
		else if (bmax > wmax) cout << "검은 돌 " << bmax << "개" << endl;
		else cout << "흰 돌 " << wmax << "개 | 검은 돌 " << bmax << "개" << endl;
	}

	for (int start = 0; start < 19; ++start)
	{
		int wmulti = 0, wmax = 0, bmulti = 0, bmax = 0;
		for (int k = 0; 18 - k >= 0 && start + k < 19; ++k)
		{
			if (board[18 - k][start + k] == '0') { wmulti++; bmulti = 0; }
			else if (board[18 - k][start + k] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }
			if (wmulti > wmax) wmax = wmulti;
			if (bmulti > bmax) bmax = bmulti;
		}
		cout << "우상향 시작(" << start << ",18): ";
		if (wmax == 0 && bmax == 0) cout << "없음" << endl;
		else if (wmax > bmax) cout << "흰 돌 " << wmax << "개" << endl;
		else if (bmax > wmax) cout << "검은 돌 " << bmax << "개" << endl;
		else cout << "흰 돌 " << wmax << "개 | 검은 돌 " << bmax << "개" << endl;
	}

	for (int start = 17; start >= 0; --start)
	{
		int wmulti = 0, wmax = 0, bmulti = 0, bmax = 0;
		for (int k = 0; start - k >= 0 && k < 19; ++k)
		{
			if (board[start - k][k] == '0') { wmulti++; bmulti = 0; }
			else if (board[start - k][k] == 'X') { bmulti++; wmulti = 0; }
			else { wmulti = 0; bmulti = 0; }
			if (wmulti > wmax) wmax = wmulti;
			if (bmulti > bmax) bmax = bmulti;
		}
		cout << "우상향 시작(0," << start << "): ";
		if (wmax == 0 && bmax == 0) cout << "없음" << endl;
		else if (wmax > bmax) cout << "흰 돌 " << wmax << "개" << endl;
		else if (bmax > wmax) cout << "검은 돌 " << bmax << "개" << endl;
		else cout << "흰 돌 " << wmax << "개 | 검은 돌 " << bmax << "개" << endl;
	}
}

// 흰 돌과 검은 돌 개수를 매 차례마다 둘 다 출력하는 함수
void printAllCounts()
{
	// 흰 돌 가로
	for (int i = 0; i < 19; ++i) {
		garowboard[i] = 0;
		for (int j = 0; j < 19; ++j)
			if (board[i][j] == '0') garowboard[i]++;
	}
	// 검은 돌 가로
	for (int i = 0; i < 19; ++i) {
		garobboard[i] = 0;
		for (int j = 0; j < 19; ++j)
			if (board[i][j] == 'X') garobboard[i]++;
	}
	// 출력 (가로)
	for (int i = 0; i < 19; ++i)
		cout << i << "번째 가로줄 흰 돌:" << garowboard[i]
		<< " 검은 돌:" << garobboard[i] << endl;

	// 흰 돌 세로
	for (int i = 0; i < 19; ++i) {
		serowboard[i] = 0;
		for (int j = 0; j < 19; ++j)
			if (board[j][i] == '0') serowboard[i]++;
	}
	// 검은 돌 세로
	for (int i = 0; i < 19; ++i) {
		serobboard[i] = 0;
		for (int j = 0; j < 19; ++j)
			if (board[j][i] == 'X') serobboard[i]++;
	}
	// 출력 (세로)
	for (int i = 0; i < 19; ++i)
		cout << i << "번째 세로줄 흰 돌:" << serowboard[i]
		<< " 검은 돌:" << serobboard[i] << endl;
}

int main()
{
	inboard();
	printboard();

	while (1)
	{
		if (turn == false)
		{
			cout << "흰 돌을 놓을 위치를 입력하세요.:";
			int x, y;
			cin >> x >> y;
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
						cout << "이미 돌이 놓여있습니다." << endl;
					else
					{
						position(y, x, '0');
						printboard();
						wstonecount++;
						cout << "흰 돌의 개수: " << wstonecount
							<< " 검은돌의 개수: " << bstonecount << endl;

						printAllCounts();   // 흰 돌과 검은 돌 개수 둘 다 출력
						printmulti();
						crossline();
						feature1_printMaxGap();
						feature2_printBlock('0');
						turn = true;
					}
				}
				else
					cout << "정상적인 입력이 아닙니다." << endl;
			}
		}
		else
		{
			cout << "검은 돌을 놓을 위치를 입력하세요.:";
			int x, y;
			cin >> x >> y;
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
						cout << "이미 돌이 놓여있습니다." << endl;
					else
					{
						position(y, x, 'X');
						printboard();
						bstonecount++;
						cout << "흰 돌의 개수: " << wstonecount
							<< " 검은돌의 개수: " << bstonecount << endl;

						printAllCounts();   // 흰 돌과 검은 돌 개수 둘 다 출력
						printmulti();
						crossline();
						feature1_printMaxGap();
						feature2_printBlock('X');
						turn = false;
					}
				}
				else
					cout << "정상적인 입력이 아닙니다." << endl;
			}
		}
	}
}