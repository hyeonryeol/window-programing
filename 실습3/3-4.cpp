#include <windows.h>
#include <cstdlib>
#include <ctime>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

int mx[100] = {};
int my[100] = {};
int obcount = 60;
int crash = -1;
int g_rect[900][4];
int g_player[900][4];
int g_ob[900][4];
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
int enterpress = 1;

void makeboard()
{
	int w = 20, h = 20;
	for (int i = 0; i < 30; ++i)
		for (int j = 0; j < 30; ++j)
		{
			int c = i * 30 + j;
			g_rect[c][0] = i * w + 20;
			g_rect[c][1] = j * h + 20;
			g_rect[c][2] = g_rect[c][0] + w;
			g_rect[c][3] = g_rect[c][1] + h;
		}
}

void player()
{
	int w = 20, h = 20;
	for (int i = 0; i < 30; ++i)
		for (int j = 0; j < 30; ++j)
		{
			int c = i * 30 + j;
			g_player[c][0] = i * w + 20;
			g_player[c][1] = j * h + 20;
			g_player[c][2] = g_player[c][0] + w;
			g_player[c][3] = g_player[c][1] + h;
		}
}

void ob()
{
	int w = 20, h = 20;
	for (int i = 0; i < 28; ++i)
		for (int j = 0; j < 25; ++j)
		{
			int c = i * 30 + j;
			g_ob[c][0] = i * w + 60;
			g_ob[c][1] = j * h + 60;
			g_ob[c][2] = g_ob[c][0] + w;
			g_ob[c][3] = g_ob[c][1] + h;
		}
}

void CALLBACK PlayerTimerProc(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime)
{
	if (crash == -1)
	{
		for (int i = 0; i < 60; ++i)
		{
			if (g_ob[obposition[i]][3] + oby[i] == g_player[450][1] + py &&
				g_player[450][0] + px == g_ob[obposition[i]][0] + obx[i])
			{
				crash = i;
				oby[crash] += 20;
			}
		}
	}
	else
	{
		oby[crash] += 20;
	}

	py += 20;
	if (g_player[450][3] + py > 620)
	{
		py = 0;
		crash = -1;
	}

	
	erasecount = 0;
	int lastrow[100] = {};

	// 기존 장애물 (0~59)
	for (int i = 0; i < 60; ++i)
	{
		if (g_ob[obposition[i]][3] + oby[i] == 620)
		{
			lastrow[erasecount] = i;
			erasecount++;
		}
	}
	// 클릭 장애물 +20
	for (int i = 60; i < obcount; ++i)
	{
		if (my[i] + 20 == 620)
		{
			lastrow[erasecount] = i;
			erasecount++;
		}
	}

	if (erasecount == 30)
	{
		bool sameColor = true;
		for (int i = 0; i < erasecount - 1; ++i)
		{
			if (randcolor[lastrow[i]] != randcolor[lastrow[i + 1]])
			{
				sameColor = false;
				break;
			}
		}
		if (sameColor)
		{
			for (int i = 0; i < erasecount; ++i)
			{
				int idx = lastrow[i];
				if (idx < 60) oby[idx] = -9999;  // 기존 장애물
				else          my[idx] = -9999;  // 클릭 장애물
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
		CW_USEDEFAULT, CW_USEDEFAULT, 1000, 1000,
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
	HDC hdc, mDC;
	HBITMAP hBitmap;
	RECT rt ;
	
	GetClientRect(hWnd, &rt);
	static BOOL Selection;
	switch (msg)
	{
	case WM_CREATE:
	{
		srand((unsigned)time(NULL));
		for (int i = 0; i < 60; ++i)
		{
			randcolor[i] = rand() % 4 + 1;
			obposition[i] = rand() % 900;
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
		mx[obcount] = (LOWORD(lParam) / 20) * 20;
		my[obcount] = (HIWORD(lParam) / 20) * 20;
		if (mx[obcount] >= 20 && mx[obcount] < 620 &&
			my[obcount] >= 20 && my[obcount] < 620)
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
			{
				oby[i] = -9999;
			}
		}
		for (int i = 60; i < obcount; ++i)
		{
			if (clickX >= mx[i] && clickX <= mx[i] + 20 &&
				clickY >= my[i] && clickY <= my[i] + 20)
			{
				my[i] = -9999;
			}
		}
		InvalidateRect(hWnd, nullptr, false);
		return 0;
	}
	case WM_KEYDOWN:
	{
		if (wParam == VK_LEFT)
		{
			px -= 20;
			for (int i = 0; i < 60; ++i)
			{
				if (g_player[450][2] + px == g_ob[obposition[i]][0] + obx[i] &&
					g_ob[obposition[i]][3] + oby[i] == g_player[450][1] + py)
				{
					obx[i] -= 20;
				}
			}
		}
		if (wParam == VK_RIGHT)
		{
			px += 20;
			for (int i = 0; i < 60; ++i)
			{
				if (g_player[450][0] + px == g_ob[obposition[i]][2] + obx[i] &&
					g_ob[obposition[i]][3] + oby[i] == g_player[450][1] + py)
				{
					obx[i] += 20;
				}
			}
		}
		if (wParam == VK_RETURN)
		{
			if (enterpress == 1) { obx[crash] += 20; oby[crash] += 20; enterpress++; }
			else if (enterpress == 2) { obx[crash] -= 20; oby[crash] += 20; enterpress++; }
			else if (enterpress == 3) { obx[crash] -= 20; oby[crash] -= 20; enterpress++; }
			else if (enterpress == 4) { obx[crash] += 20; oby[crash] -= 20; enterpress = 1; }
		}
		if (wParam == 'Q') PostQuitMessage(0);
		if (wParam == 'R')
		{
			crash = -1;
			obcount = 60;
			px = 0; py = 0;
			for (int i = 0; i < 60; ++i)
			{
				oby[i] = 0; obx[i] = 0;
			}
			for (int i = 0; i < 60; ++i)
			{
				randcolor[i] = rand() % 4 + 1;
				obposition[i] = rand() % 900;
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

		mDC = CreateCompatibleDC(hdc); //--- 메모리 DC 만들기
		hBitmap = CreateCompatibleBitmap(hdc, rt.right, rt.bottom); //--- 메모리 DC와 연결할 비트맵 만들기
		SelectObject(mDC, (HBITMAP)hBitmap); //--- 메모리 DC와 비트맵 연결하기
		FillRect(mDC, &rt, (HBRUSH)GetStockObject(WHITE_BRUSH)); // 배경 흰색

		HBRUSH oldbrush = (HBRUSH)SelectObject(mDC, boardbrush);
		for (int i = 0; i < 900; ++i)
			Rectangle(mDC, g_rect[i][0], g_rect[i][1], g_rect[i][2], g_rect[i][3]);
		SelectObject(mDC, oldbrush);

		oldbrush = (HBRUSH)SelectObject(mDC, playerbrush);
		Ellipse(mDC,
			g_player[450][0] + px, g_player[450][1] + py,
			g_player[450][2] + px, g_player[450][3] + py);
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
			Rectangle(mDC, mx[i], my[i], mx[i] + 20, my[i] + 20);
			SelectObject(mDC, oldbrush);
			DeleteObject(obbrush);
		}
		BitBlt(hdc, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
		DeleteDC(mDC); //--- 생성한 메모리 DC 삭제
		DeleteObject(hBitmap); //--- 생성한 비트맵 삭제
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