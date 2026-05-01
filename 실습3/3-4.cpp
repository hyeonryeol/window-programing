#include <windows.h>
#include <cstdlib>
#include <ctime>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";



constexpr int CELL = 20;   // 셀 하나의 픽셀 크기
constexpr int COLS = 20;   // 가로 칸 수
constexpr int ROWS = 20;   // 세로 칸 수
constexpr int OFFSET = 20;   // 보드 시작 여백



constexpr int TOTAL = COLS * ROWS;
constexpr int BOARD_LEFT = OFFSET;
constexpr int BOARD_TOP = OFFSET;
constexpr int BOARD_RIGHT = OFFSET + COLS * CELL;
constexpr int BOARD_BOTTOM = OFFSET + ROWS * CELL;
constexpr int BOARD_MAX_X = BOARD_RIGHT - CELL; // 클램프 최대 x
constexpr int BOARD_MAX_Y = BOARD_BOTTOM - CELL; // 클램프 최대 y
constexpr int PLAYER_IDX = (COLS / 2) * ROWS;   // 플레이어 기준 격자 인덱스
constexpr int WIN_W = BOARD_RIGHT + 380;
constexpr int WIN_H = BOARD_BOTTOM + 380;

int mx[100] = {};
int my[100] = {};
int obcount = 60;
int crashList[60] = {};
int crashCount = 0;
int g_rect[TOTAL][4];
int g_player[TOTAL][4];
int g_ob[TOTAL][4];
int erasecount = 0;
HBRUSH boardbrush;
HBRUSH playerbrush;
HBRUSH obbrush;
int randcolor[100] = {};
int obposition[100] = {};
int px = 0;
int py = 0;
int obx[100] = {};
int oby[100] = {};
int rotDir = 0; // 0=위, 1=오른쪽, 2=아래, 3=왼쪽

void makeboard()
{
	for (int i = 0; i < COLS; ++i)
		for (int j = 0; j < ROWS; ++j)
		{
			int c = i * ROWS + j;
			g_rect[c][0] = i * CELL + OFFSET;
			g_rect[c][1] = j * CELL + OFFSET;
			g_rect[c][2] = g_rect[c][0] + CELL;
			g_rect[c][3] = g_rect[c][1] + CELL;
		}
}

void player()
{
	for (int i = 0; i < COLS; ++i)
		for (int j = 0; j < ROWS; ++j)
		{
			int c = i * ROWS + j;
			g_player[c][0] = i * CELL + OFFSET;
			g_player[c][1] = j * CELL + OFFSET;
			g_player[c][2] = g_player[c][0] + CELL;
			g_player[c][3] = g_player[c][1] + CELL;
		}
}

void ob()
{
	for (int i = 0; i < COLS; ++i)
		for (int j = 0; j < ROWS; ++j)
		{
			int c = i * ROWS + j;
			g_ob[c][0] = i * CELL + OFFSET;
			g_ob[c][1] = j * CELL + OFFSET;
			g_ob[c][2] = g_ob[c][0] + CELL;
			g_ob[c][3] = g_ob[c][1] + CELL;
		}
}

bool inCrashList(int idx)
{
	for (int i = 0; i < crashCount; ++i)
		if (crashList[i] == idx) return true;
	return false;
}

void updateCarriedObstacles()
{
	int playerLeft = g_player[PLAYER_IDX][0] + px;
	int playerTop = g_player[PLAYER_IDX][1] + py;

	for (int ci = 0; ci < crashCount; ++ci)
	{
		int idx = crashList[ci];
		int targetX, targetY;

		switch (rotDir)
		{
		case 0: targetX = playerLeft;                  targetY = playerTop - CELL * (ci + 1); break; // 위
		case 1: targetX = playerLeft + CELL * (ci + 1); targetY = playerTop;                   break; // 오른쪽
		case 2: targetX = playerLeft;                  targetY = playerTop + CELL * (ci + 1); break; // 아래
		case 3: targetX = playerLeft - CELL * (ci + 1); targetY = playerTop;                   break; // 왼쪽
		default: targetX = playerLeft; targetY = playerTop - CELL * (ci + 1); break;
		}

		// 보드 범위 클램프
		if (targetX < BOARD_LEFT)  targetX = BOARD_LEFT;
		if (targetX > BOARD_MAX_X) targetX = BOARD_MAX_X;
		if (targetY < BOARD_TOP)   targetY = BOARD_TOP;
		if (targetY > BOARD_MAX_Y) targetY = BOARD_MAX_Y;

		obx[idx] = targetX - g_ob[obposition[idx]][0];
		oby[idx] = targetY - g_ob[obposition[idx]][1];
	}
}

void CALLBACK PlayerTimerProc(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime)
{
	py += CELL;

	// 1) 첫 픽업
	if (crashCount == 0)
	{
		for (int i = 0; i < 60; ++i)
		{
			if (oby[i] <= -9000) continue;
			if (g_ob[obposition[i]][3] + oby[i] == g_player[PLAYER_IDX][1] + py &&
				g_player[PLAYER_IDX][0] + px == g_ob[obposition[i]][0] + obx[i])
			{
				crashList[crashCount++] = i;
				rotDir = 0;
			}
		}
	}

	// 2) 위치 업데이트
	updateCarriedObstacles();

	// 3) 체인 픽업
	if (crashCount > 0)
	{
		bool changed = true;
		while (changed)
		{
			changed = false;
			int topIdx = crashList[crashCount - 1];
			int stackTop = g_ob[obposition[topIdx]][1] + oby[topIdx];
			int stackLeft = g_ob[obposition[topIdx]][0] + obx[topIdx];
			int stackRight = g_ob[obposition[topIdx]][2] + obx[topIdx];

			for (int i = 0; i < 60; ++i)
			{
				if (oby[i] <= -9000 || inCrashList(i)) continue;
				int bottom = g_ob[obposition[i]][3] + oby[i];
				int iLeft = g_ob[obposition[i]][0] + obx[i];
				int iRight = g_ob[obposition[i]][2] + obx[i];

				if (bottom == stackTop && iLeft < stackRight && iRight > stackLeft)
				{
					crashList[crashCount++] = i;
					updateCarriedObstacles();
					changed = true;
					break;
				}
			}
		}
	}

	// 4) 바닥 도달 시 리셋
	if (g_player[PLAYER_IDX][3] + py > BOARD_BOTTOM)
	{
		py = 0;
		crashCount = 0;
		rotDir = 0;
	}

	// 5) 줄 지우기 로직
	erasecount = 0;
	int lastrow[100] = {};

	for (int i = 0; i < 60; ++i)
		if (oby[i] > -9000 && g_ob[obposition[i]][3] + oby[i] == BOARD_BOTTOM)
			lastrow[erasecount++] = i;

	for (int i = 60; i < obcount; ++i)
		if (my[i] > -9000 && my[i] + CELL == BOARD_BOTTOM)
			lastrow[erasecount++] = i;

	if (erasecount > 0)
	{
		// COLS개의 고유 x 칸이 전부 채워졌는지 확인
		bool filledX[COLS] = {};
		for (int i = 0; i < erasecount; ++i)
		{
			int idx = lastrow[i];
			int blockX = (idx < 60)
				? g_ob[obposition[idx]][0] + obx[idx]
				: mx[idx];
			int col = (blockX - OFFSET) / CELL;
			if (col >= 0 && col < COLS) filledX[col] = true;
		}

		bool fullRow = true;
		for (int i = 0; i < COLS; ++i)
			if (!filledX[i]) { fullRow = false; break; }

		if (fullRow)
		{
			bool sameColor = true;
			int  refColor = randcolor[lastrow[0]];
			for (int i = 1; i < erasecount; ++i)
				if (randcolor[lastrow[i]] != refColor) { sameColor = false; break; }

			if (sameColor)
				for (int i = 0; i < erasecount; ++i)
				{
					int idx = lastrow[i];
					if (idx < 60) oby[idx] = -9999;
					else          my[idx] = -9999;
				}
		}
	}

	InvalidateRect(hWnd, NULL, false);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = CLASS_NAME;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	RegisterClassExW(&wc);

	HWND hWnd = CreateWindowExW(
		0, CLASS_NAME, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WIN_W, WIN_H,
		nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) return 0;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC      hdc, mDC;
	HBITMAP  hBitmap;
	RECT     rt;

	GetClientRect(hWnd, &rt);
	switch (msg)
	{
	case WM_CREATE:
	{
		srand((unsigned)time(NULL));
		for (int i = 0; i < 60; ++i)
		{
			randcolor[i] = rand() % 4 + 1;
			obposition[i] = rand() % TOTAL;
			if (randcolor[i] == 1) randcolor[i] = RGB(255, 0, 0);
			if (randcolor[i] == 2) randcolor[i] = RGB(0, 255, 0);
			if (randcolor[i] == 3) randcolor[i] = RGB(0, 0, 255);
			if (randcolor[i] == 4) randcolor[i] = RGB(0, 255, 255);
		}
		playerbrush = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
		boardbrush = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
		makeboard();
		player();
		ob();
		SetTimer(hWnd, 1, 500, (TIMERPROC)PlayerTimerProc);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		mx[obcount] = (LOWORD(lParam) / CELL) * CELL;
		my[obcount] = (HIWORD(lParam) / CELL) * CELL;
		if (mx[obcount] >= BOARD_LEFT && mx[obcount] < BOARD_RIGHT &&
			my[obcount] >= BOARD_TOP && my[obcount] < BOARD_BOTTOM)
		{
			randcolor[obcount] = RGB(255, 0, 0);
			obcount++;
			InvalidateRect(hWnd, nullptr, false);
		}
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		int clickX = LOWORD(lParam);
		int clickY = HIWORD(lParam);

		for (int i = 0; i < 60; ++i)
		{
			if (clickX >= g_ob[obposition[i]][0] + obx[i] &&
				clickX <= g_ob[obposition[i]][2] + obx[i] &&
				clickY >= g_ob[obposition[i]][1] + oby[i] &&
				clickY <= g_ob[obposition[i]][3] + oby[i])
				oby[i] = -9999;
		}
		for (int i = 60; i < obcount; ++i)
		{
			if (clickX >= mx[i] && clickX <= mx[i] + CELL &&
				clickY >= my[i] && clickY <= my[i] + CELL)
				my[i] = -9999;
		}
		InvalidateRect(hWnd, nullptr, false);
		return 0;
	}
	case WM_KEYDOWN:
	{
		if (wParam == VK_LEFT) { px -= CELL; updateCarriedObstacles(); }
		if (wParam == VK_RIGHT) { px += CELL; updateCarriedObstacles(); }
		if (wParam == VK_RETURN && crashCount > 0)
		{
			rotDir = (rotDir + 1) % 4;
			updateCarriedObstacles();
		}
		if (wParam == 'Q') PostQuitMessage(0);
		if (wParam == 'R')
		{
			crashCount = 0;
			rotDir = 0;
			obcount = 60;
			px = 0; py = 0;
			for (int i = 0; i < 60; ++i) { oby[i] = 0; obx[i] = 0; }
			for (int i = 0; i < 60; ++i)
			{
				randcolor[i] = rand() % 4 + 1;
				obposition[i] = rand() % TOTAL;
				if (randcolor[i] == 1) randcolor[i] = RGB(255, 0, 0);
				if (randcolor[i] == 2) randcolor[i] = RGB(0, 255, 0);
				if (randcolor[i] == 3) randcolor[i] = RGB(0, 0, 255);
				if (randcolor[i] == 4) randcolor[i] = RGB(0, 255, 255);
			}
		}
		InvalidateRect(hWnd, nullptr, false);
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		mDC = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, rt.right, rt.bottom);
		SelectObject(mDC, (HBITMAP)hBitmap);
		FillRect(mDC, &rt, (HBRUSH)GetStockObject(WHITE_BRUSH));

		HBRUSH oldbrush = (HBRUSH)SelectObject(mDC, boardbrush);
		for (int i = 0; i < TOTAL; ++i)
			Rectangle(mDC, g_rect[i][0], g_rect[i][1], g_rect[i][2], g_rect[i][3]);
		SelectObject(mDC, oldbrush);

		oldbrush = (HBRUSH)SelectObject(mDC, playerbrush);
		Ellipse(mDC,
			g_player[PLAYER_IDX][0] + px, g_player[PLAYER_IDX][1] + py,
			g_player[PLAYER_IDX][2] + px, g_player[PLAYER_IDX][3] + py);
		SelectObject(mDC, oldbrush);

		for (int i = 0; i < 60; ++i)
		{
			obbrush = (HBRUSH)CreateSolidBrush(randcolor[i]);
			oldbrush = (HBRUSH)SelectObject(mDC, obbrush);
			Rectangle(mDC,
				g_ob[obposition[i]][0] + obx[i], g_ob[obposition[i]][1] + oby[i],
				g_ob[obposition[i]][2] + obx[i], g_ob[obposition[i]][3] + oby[i]);
			SelectObject(mDC, oldbrush);
			DeleteObject(obbrush);
		}
		for (int i = 60; i < obcount; ++i)
		{
			obbrush = (HBRUSH)CreateSolidBrush(randcolor[i]);
			oldbrush = (HBRUSH)SelectObject(mDC, obbrush);
			Rectangle(mDC, mx[i], my[i], mx[i] + CELL, my[i] + CELL);
			SelectObject(mDC, oldbrush);
			DeleteObject(obbrush);
		}

		BitBlt(hdc, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
		DeleteDC(mDC);
		DeleteObject(hBitmap);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_DESTROY:
		DeleteObject(boardbrush);
		DeleteObject(playerbrush);
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}