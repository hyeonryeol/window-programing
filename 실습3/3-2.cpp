#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <math.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

// GDI 오브젝트
HBRUSH wonbrush1, wonbrush2, wonbrush3, wonbrush4;
HPEN   wonpen1, wonpen2, wonpen3, wonpen4;
HPEN   selectpen;
HBRUSH gongjeon1, gongjeon2, gongjeon3, gongjeon4;
static int mx, my;

// 이동 오프셋
int rbx1 = 0, rby1 = 0, rbx2 = 0, rby2 = 0, rbx3 = 0, rby3 = 0, rbx4 = 0, rby4 = 0;

// [FIX] 오브젝트별 배경도형 상태 (전역 won/rect/tri 제거 → 개별화)
bool won1 = true, won2 = true, won3 = true, won4 = true;
bool rect1 = false, rect2 = false, rect3 = false, rect4 = false;
bool tri1 = false, tri2 = false, tri3 = false, tri4 = false;

// [FIX] 오브젝트별 공전도형 상태 (전역 gongjeonwon 제거 → 개별화)
bool gongjeonwon1 = true, gongjeonwon2 = true, gongjeonwon3 = true, gongjeonwon4 = true;

// 선택
bool select1 = false, select2 = false, select3 = false, select4 = false;

// 속도 부스트 플래그
bool leftclick1 = false, leftclick2 = false, leftclick3 = false, leftclick4 = false;

// 우클릭 드래그 플래그
bool rbpress1 = false, rbpress2 = false, rbpress3 = false, rbpress4 = false;

// 회전 방향
int cpress1 = 1, cpress2 = 1, cpress3 = 1, cpress4 = 1;

// 원 배경일 때 공전 각도
double angle1 = 0.0, angle2 = 0.0, angle3 = 0.0, angle4 = 0.0;
int orbitR = 100;

// 사각형 배경일 때 공전 위치
int rectmovex1 = -100, rectmovey1 = -100;
int rectmovex2 = -100, rectmovey2 = -100;
int rectmovex3 = -100, rectmovey3 = -100;
int rectmovex4 = -100, rectmovey4 = -100;

// 삼각형 배경일 때 공전 위치
int trimovex1 = 0, trimovey1 = -100;
int trimovex2 = 0, trimovey2 = -100;
int trimovex3 = 0, trimovey3 = -100;
int trimovex4 = 0, trimovey4 = -100;

// 타이머 속도
int timerspeed1 = 500, timerspeed2 = 500, timerspeed3 = 500, timerspeed4 = 500;

// ─────────────────────────────────────
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = CLASS_NAME;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	RegisterClassExW(&wc);

	HWND hWnd = CreateWindowExW(0, CLASS_NAME, WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
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

// ─────────────────────────────────────
void CALLBACK TimerProc1(HWND hWnd, UINT, UINT, DWORD)
{
	if (leftclick1)
	{
		timerspeed1 += 50;
		KillTimer(hWnd, 1);
		SetTimer(hWnd, 1, timerspeed1, (TIMERPROC)TimerProc1);
		if (timerspeed1 >= 500)          // [FIX] >= 500으로 정확히 리셋
		{
			timerspeed1 = 500;
			leftclick1 = false;
		}
	}
	if (won1)
	{
		if (cpress1 == 1) { angle1 += 0.1; if (angle1 > 6.28) angle1 = 0.0; }
		else { angle1 -= 0.1; if (angle1 < -6.28) angle1 = 0.0; }
	}
	if (rect1)
	{
		if (cpress1 == 1)
		{
			if (-100 <= rectmovex1 && rectmovex1 <= 100 && rectmovey1 == -100) rectmovex1 += 10;
			if (-100 <= rectmovex1 && rectmovex1 <= 100 && rectmovey1 == 100)  rectmovex1 -= 10;
			if (rectmovex1 == -100 && -100 <= rectmovey1 && rectmovey1 <= 100) rectmovey1 -= 10;
			if (rectmovex1 == 100 && -100 <= rectmovey1 && rectmovey1 <= 100) rectmovey1 += 10;
		}
		else
		{
			if (-100 <= rectmovex1 && rectmovex1 <= 100 && rectmovey1 == -100) rectmovex1 -= 10;
			if (-100 <= rectmovex1 && rectmovex1 <= 100 && rectmovey1 == 100)  rectmovex1 += 10;
			if (rectmovex1 == -100 && -100 <= rectmovey1 && rectmovey1 <= 100) rectmovey1 += 10;
			if (rectmovex1 == 100 && -100 <= rectmovey1 && rectmovey1 <= 100) rectmovey1 -= 10;
		}
	}
	if (tri1)
	{
		if (cpress1 == 1)
		{
			if (0 <= trimovex1 && trimovex1 < 100 && trimovey1 < 100)
			{
				trimovex1 += 10; trimovey1 += 20;
			}
			else if (trimovey1 >= 100 && trimovex1 > -100)
			{
				trimovey1 = 100; trimovex1 -= 10;
			}
			else if (trimovex1 >= -100 && trimovex1<0 && trimovey1>-100)
			{
				trimovex1 += 10; trimovey1 -= 20;
			}
		}
		else
		{
			if (-100 < trimovex1 && trimovex1 <= 0 && trimovey1 < 100)
			{
				trimovex1 -= 10; trimovey1 += 20;
			}
			else if (trimovey1 >= 100 && trimovex1 < 100)
			{
				trimovey1 = 100; trimovex1 += 10;
			}
			else if (0 < trimovex1 && trimovex1 <= 100 && trimovey1 > -100)
			{
				trimovex1 -= 10; trimovey1 -= 20;
			}
		}
	}
	InvalidateRect(hWnd, NULL, TRUE);
}

void CALLBACK TimerProc2(HWND hWnd, UINT, UINT, DWORD)
{
	if (leftclick2)
	{
		timerspeed2 += 50;
		KillTimer(hWnd, 2);
		SetTimer(hWnd, 2, timerspeed2, (TIMERPROC)TimerProc2);
		if (timerspeed2 >= 500)
		{
			timerspeed2 = 500;
			leftclick2 = false;
		}
	}
	if (won2)
	{
		if (cpress2 == 1) { angle2 += 0.2; if (angle2 > 6.28) angle2 = 0.0; }
		else { angle2 -= 0.2; if (angle2 < -6.28) angle2 = 0.0; }
	}
	if (rect2)
	{
		if (cpress2 == 1)
		{
			if (-100 <= rectmovex2 && rectmovex2 <= 100 && rectmovey2 == -100) rectmovex2 += 10;
			if (-100 <= rectmovex2 && rectmovex2 <= 100 && rectmovey2 == 100)  rectmovex2 -= 10;
			if (rectmovex2 == -100 && -100 <= rectmovey2 && rectmovey2 <= 100) rectmovey2 -= 10;
			if (rectmovex2 == 100 && -100 <= rectmovey2 && rectmovey2 <= 100) rectmovey2 += 10;
		}
		else
		{
			if (-100 <= rectmovex2 && rectmovex2 <= 100 && rectmovey2 == -100) rectmovex2 -= 10;
			if (-100 <= rectmovex2 && rectmovex2 <= 100 && rectmovey2 == 100)  rectmovex2 += 10;
			if (rectmovex2 == -100 && -100 <= rectmovey2 && rectmovey2 <= 100) rectmovey2 += 10;
			if (rectmovex2 == 100 && -100 <= rectmovey2 && rectmovey2 <= 100) rectmovey2 -= 10;
		}
	}
	if (tri2)
	{
		if (cpress2 == 1)
		{
			if (0 <= trimovex2 && trimovex2 < 100 && trimovey2 < 100)
			{
				trimovex2 += 5; trimovey2 += 10;
			}
			else if (trimovey2 >= 100 && trimovex2 > -100)
			{
				trimovey2 = 100; trimovex2 -= 10;
			}
			else if (trimovex2 >= -100 && trimovex2<0 && trimovey2>-100)
			{
				trimovex2 += 5; trimovey2 -= 10;
			}
		}
		else
		{
			if (-100 < trimovex2 && trimovex2 <= 0 && trimovey2 < 100)
			{
				trimovex2 -= 5; trimovey2 += 10;
			}
			else if (trimovey2 >= 100 && trimovex2 < 100)
			{
				trimovey2 = 100; trimovex2 += 5;
			}
			else if (0 < trimovex2 && trimovex2 <= 100 && trimovey2 > -100)
			{
				trimovex2 -= 5; trimovey2 -= 10;
			}
		}
	}
	InvalidateRect(hWnd, NULL, TRUE);
}

void CALLBACK TimerProc3(HWND hWnd, UINT, UINT, DWORD)
{
	if (leftclick3)
	{
		timerspeed3 += 50;
		KillTimer(hWnd, 3);
		SetTimer(hWnd, 3, timerspeed3, (TIMERPROC)TimerProc3);
		if (timerspeed3 >= 500)
		{
			timerspeed3 = 500;
			leftclick3 = false;
		}
	}
	if (won3)
	{
		if (cpress3 == 1) { angle3 += 0.3; if (angle3 > 6.28) angle3 = 0.0; }
		else { angle3 -= 0.3; if (angle3 < -6.28) angle3 = 0.0; }
	}
	if (rect3)
	{
		if (cpress3 == 1)
		{
			if (-100 <= rectmovex3 && rectmovex3 <= 100 && rectmovey3 == -100) rectmovex3 += 20;
			if (-100 <= rectmovex3 && rectmovex3 <= 100 && rectmovey3 == 100)  rectmovex3 -= 20;
			if (rectmovex3 == -100 && -100 <= rectmovey3 && rectmovey3 <= 100) rectmovey3 -= 20;
			if (rectmovex3 == 100 && -100 <= rectmovey3 && rectmovey3 <= 100) rectmovey3 += 20;
		}
		else
		{
			if (-100 <= rectmovex3 && rectmovex3 <= 100 && rectmovey3 == -100) rectmovex3 -= 20;
			if (-100 <= rectmovex3 && rectmovex3 <= 100 && rectmovey3 == 100)  rectmovex3 += 20;
			if (rectmovex3 == -100 && -100 <= rectmovey3 && rectmovey3 <= 100) rectmovey3 += 20;
			if (rectmovex3 == 100 && -100 <= rectmovey3 && rectmovey3 <= 100) rectmovey3 -= 20;
		}
	}
	if (tri3)
	{
		if (cpress3 == 1)
		{
			if (0 <= trimovex3 && trimovex3 < 100 && trimovey3 < 100)
			{
				trimovex3 += 10; trimovey3 += 20;
			}
			else if (trimovey3 >= 100 && trimovex3 > -100)
			{
				trimovey3 = 100; trimovex3 -= 10;
			}
			else if (trimovex3 >= -100 && trimovex3<0 && trimovey3>-100)
			{
				trimovex3 += 10; trimovey3 -= 20;
			}
		}
		else
		{
			if (-100 < trimovex3 && trimovex3 <= 0 && trimovey3 < 100)
			{
				trimovex3 -= 10; trimovey3 += 20;
			}
			else if (trimovey3 >= 100 && trimovex3 < 100)
			{
				trimovey3 = 100; trimovex3 += 10;
			}
			else if (trimovex3 > 0 && trimovex3 <= 100 && trimovey3 > -100)
			{
				trimovex3 -= 10; trimovey3 -= 20;
			}
		}
	}
	InvalidateRect(hWnd, NULL, TRUE);
}

void CALLBACK TimerProc4(HWND hWnd, UINT, UINT, DWORD)
{
	if (leftclick4)
	{
		timerspeed4 += 50;
		KillTimer(hWnd, 4);
		SetTimer(hWnd, 4, timerspeed4, (TIMERPROC)TimerProc4);
		if (timerspeed4 >= 500)
		{
			timerspeed4 = 500;
			leftclick4 = false;
		}
	}
	if (won4)
	{
		if (cpress4 == 1) { angle4 += 0.4; if (angle4 > 6.28) angle4 = 0.0; }
		else { angle4 -= 0.4; if (angle4 < -6.28) angle4 = 0.0; }
	}
	if (rect4)
	{
		if (cpress4 == 1)
		{
			if (-100 <= rectmovex4 && rectmovex4 <= 100 && rectmovey4 == -100) rectmovex4 += 25;
			if (-100 <= rectmovex4 && rectmovex4 <= 100 && rectmovey4 == 100)  rectmovex4 -= 25;
			if (rectmovex4 == -100 && -100 <= rectmovey4 && rectmovey4 <= 100) rectmovey4 -= 25;
			if (rectmovex4 == 100 && -100 <= rectmovey4 && rectmovey4 <= 100) rectmovey4 += 25;
		}
		else
		{
			if (-100 <= rectmovex4 && rectmovex4 <= 100 && rectmovey4 == -100) rectmovex4 -= 25;
			if (-100 <= rectmovex4 && rectmovex4 <= 100 && rectmovey4 == 100)  rectmovex4 += 25;
			if (rectmovex4 == -100 && -100 <= rectmovey4 && rectmovey4 <= 100) rectmovey4 += 25;
			if (rectmovex4 == 100 && -100 <= rectmovey4 && rectmovey4 <= 100) rectmovey4 -= 25;
		}
	}
	if (tri4)
	{
		if (cpress4 == 1)
		{
			if (0 <= trimovex4 && trimovex4 < 100 && trimovey4 < 100)
			{
				trimovex4 += 10; trimovey4 += 20;
			}
			else if (trimovey4 >= 100 && trimovex4 > -100)
			{
				trimovey4 = 100; trimovex4 -= 10;
			}
			else if (trimovex4 >= -100 && trimovex4<0 && trimovey4>-100)
			{
				trimovex4 += 10; trimovey4 -= 20;
			}
		}
		else
		{
			if (-100 < trimovex4 && trimovex4 <= 0 && trimovey4 < 100)
			{
				trimovex4 -= 10; trimovey4 += 20;
			}
			else if (trimovey4 >= 100 && trimovex4 < 100)
			{
				trimovey4 = 100; trimovex4 += 10;
			}
			else if (trimovex4 > 0 && trimovex4 <= 100 && trimovey4 > -100)
			{
				trimovex4 -= 10; trimovey4 -= 20;
			}
		}
	}
	InvalidateRect(hWnd, NULL, TRUE);
}

// ─────────────────────────────────────
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static BOOL Selection;
	srand((unsigned)time(NULL));
	int r = rand() % 255, g = rand() % 255, b = rand() % 255;
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	int x = clientRect.right / 4;
	int y = clientRect.bottom / 4;

	switch (msg)
	{
		// ── 우클릭: 오브젝트 이동 ───────────────────────
	case WM_RBUTTONDOWN:
	{
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		Selection = TRUE;
		if (select1)
		{
			if (!rbpress1) { rbx1 = mx - 200; rby1 = my - 150; rbpress1 = true; }
			else { rbx1 = 0; rby1 = 0; rbpress1 = false; }
		}
		if (select2)
		{
			if (!rbpress2) { rbx2 = mx - 200; rby2 = my - 400; rbpress2 = true; }
			else { rbx2 = 0; rby2 = 0; rbpress2 = false; }
		}
		if (select3)
		{
			if (!rbpress3) { rbx3 = mx - 600; rby3 = my - 150; rbpress3 = true; }
			else { rbx3 = 0; rby3 = 0; rbpress3 = false; }
		}
		if (select4)
		{
			if (!rbpress4) { rbx4 = mx - 600; rby4 = my - 400; rbpress4 = true; }
			else { rbx4 = 0; rby4 = 0; rbpress4 = false; }
		}
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}
	// ── 좌클릭: 속도 부스트 ────────────────────────
	case WM_LBUTTONDOWN:
	{
		// [FIX] timerspeed가 정확히 500일 때만 부스트 가능 (리셋으로 항상 재사용 가능)
		if (select1 && timerspeed1 == 500)
		{
			leftclick1 = true; timerspeed1 -= 400;
			KillTimer(hWnd, 1); SetTimer(hWnd, 1, timerspeed1, (TIMERPROC)TimerProc1);
		}
		if (select2 && timerspeed2 == 500)
		{
			leftclick2 = true; timerspeed2 -= 400;
			KillTimer(hWnd, 2); SetTimer(hWnd, 2, timerspeed2, (TIMERPROC)TimerProc2);
		}
		if (select3 && timerspeed3 == 500)
		{
			leftclick3 = true; timerspeed3 -= 400;
			KillTimer(hWnd, 3); SetTimer(hWnd, 3, timerspeed3, (TIMERPROC)TimerProc3);
		}
		if (select4 && timerspeed4 == 500)
		{
			leftclick4 = true; timerspeed4 -= 400;
			KillTimer(hWnd, 4); SetTimer(hWnd, 4, timerspeed4, (TIMERPROC)TimerProc4);
		}
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}
	// ── 우클릭 더블클릭: 색상 변경 ─────────────────
	case WM_RBUTTONDBLCLK:
	{
		if (select1)
		{
			DeleteObject(gongjeon1); gongjeon1 = CreateSolidBrush(RGB(255, 55, 0));
			DeleteObject(wonpen1);   wonpen1 = CreatePen(PS_SOLID, 3, RGB(255, 55, 0));
			DeleteObject(wonbrush1); wonbrush1 = CreateSolidBrush(RGB(255 - r, 255 - g, 255 - b));
		}
		if (select2)
		{
			DeleteObject(gongjeon2); gongjeon2 = CreateSolidBrush(RGB(255, 255, 0));
			DeleteObject(wonpen2);   wonpen2 = CreatePen(PS_SOLID, 3, RGB(255, 55, 0));
			DeleteObject(wonbrush2); wonbrush2 = CreateSolidBrush(RGB(255 - r, 255 - g, 255 - b));
		}
		if (select3)
		{
			DeleteObject(gongjeon3); gongjeon3 = CreateSolidBrush(RGB(0, 55, 0));
			DeleteObject(wonpen3);   wonpen3 = CreatePen(PS_SOLID, 3, RGB(255, 55, 0));
			DeleteObject(wonbrush3); wonbrush3 = CreateSolidBrush(RGB(255 - r, 255 - g, 255 - b));
		}
		if (select4)
		{
			DeleteObject(gongjeon4); gongjeon4 = CreateSolidBrush(RGB(255, 55, 235));
			DeleteObject(wonpen4);   wonpen4 = CreatePen(PS_SOLID, 3, RGB(255, 55, 0));
			DeleteObject(wonbrush4); wonbrush4 = CreateSolidBrush(RGB(255 - r, 255 - g, 255 - b));
		}
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}
	// ── 키 입력 ────────────────────────────────────
	case WM_KEYDOWN:
	{
		// [FIX] W/R/T/M 모두 선택된 오브젝트에만 적용
		if (wParam == 'W') // 원 배경으로 복귀
		{
			if (select1) { won1 = true;  rect1 = false; tri1 = false; }
			if (select2) { won2 = true;  rect2 = false; tri2 = false; }
			if (select3) { won3 = true;  rect3 = false; tri3 = false; }
			if (select4) { won4 = true;  rect4 = false; tri4 = false; }
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == 'R') // 사각형 배경
		{
			if (select1) { rect1 = true; won1 = false; tri1 = false; }
			if (select2) { rect2 = true; won2 = false; tri2 = false; }
			if (select3) { rect3 = true; won3 = false; tri3 = false; }
			if (select4) { rect4 = true; won4 = false; tri4 = false; }
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == 'T') // 삼각형 배경
		{
			if (select1) { tri1 = true; rect1 = false; won1 = false; }
			if (select2) { tri2 = true; rect2 = false; won2 = false; }
			if (select3) { tri3 = true; rect3 = false; won3 = false; }
			if (select4) { tri4 = true; rect4 = false; won4 = false; }
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == 'Q') PostQuitMessage(0);

		if (wParam == '1') { select1 = true;  select2 = false; select3 = false; select4 = false; InvalidateRect(hWnd, NULL, TRUE); }
		if (wParam == '2') { select1 = false; select2 = true;  select3 = false; select4 = false; InvalidateRect(hWnd, NULL, TRUE); }
		if (wParam == '3') { select1 = false; select2 = false; select3 = true;  select4 = false; InvalidateRect(hWnd, NULL, TRUE); }
		if (wParam == '4') { select1 = false; select2 = false; select3 = false; select4 = true;  InvalidateRect(hWnd, NULL, TRUE); }

		if (wParam == 'C') // 회전 방향 반전 (기존과 동일 — 이미 select 체크)
		{
			if (select1) { cpress1 = (cpress1 == 1) ? 0 : 1; }
			if (select2) { cpress2 = (cpress2 == 1) ? 0 : 1; }
			if (select3) { cpress3 = (cpress3 == 1) ? 0 : 1; }
			if (select4) { cpress4 = (cpress4 == 1) ? 0 : 1; }
		}
		if (wParam == 'M') // [FIX] 선택된 오브젝트의 공전도형만 토글
		{
			if (select1) gongjeonwon1 = !gongjeonwon1;
			if (select2) gongjeonwon2 = !gongjeonwon2;
			if (select3) gongjeonwon3 = !gongjeonwon3;
			if (select4) gongjeonwon4 = !gongjeonwon4;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		return 0;
	}
	// ── 생성 ───────────────────────────────────────
	case WM_CREATE:
	{
		wonpen1 = CreatePen(PS_SOLID, 3, RGB(0, 200, 255));
		wonpen2 = CreatePen(PS_SOLID, 3, RGB(0, 200, 255));
		wonpen3 = CreatePen(PS_SOLID, 3, RGB(0, 200, 255));
		wonpen4 = CreatePen(PS_SOLID, 3, RGB(0, 200, 255));
		wonbrush1 = CreateSolidBrush(RGB(r, g, b));
		wonbrush2 = CreateSolidBrush(RGB(r, g, b));
		wonbrush3 = CreateSolidBrush(RGB(r, g, b));
		wonbrush4 = CreateSolidBrush(RGB(r, g, b));
		selectpen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
		gongjeon1 = CreateSolidBrush(RGB(0, 200, 255));
		gongjeon2 = CreateSolidBrush(RGB(0, 0, 255));
		gongjeon3 = CreateSolidBrush(RGB(255, 200, 255));
		gongjeon4 = CreateSolidBrush(RGB(0, 200, 20));
		SetTimer(hWnd, 1, timerspeed1, (TIMERPROC)TimerProc1);
		SetTimer(hWnd, 2, timerspeed2, (TIMERPROC)TimerProc2);
		SetTimer(hWnd, 3, timerspeed3, (TIMERPROC)TimerProc3);
		SetTimer(hWnd, 4, timerspeed4, (TIMERPROC)TimerProc4);
		return 0;
	}
	// ── 그리기 ─────────────────────────────────────
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HBRUSH oldbrush;
		int r = 5, gr = 10, r1 = 100;

		// ── 중심점 4개 ──────────────────────────────
		oldbrush = (HBRUSH)SelectObject(hdc, wonbrush1);
		Ellipse(hdc, x - r + rbx1, y - r + rby1, x + r + rbx1, y + r + rby1);
		SelectObject(hdc, oldbrush);

		oldbrush = (HBRUSH)SelectObject(hdc, wonbrush2);
		Ellipse(hdc, x - r + rbx2, 3 * y - r + rby2, x + r + rbx2, 3 * y + r + rby2);
		SelectObject(hdc, oldbrush);

		oldbrush = (HBRUSH)SelectObject(hdc, wonbrush3);
		Ellipse(hdc, 3 * x - r + rbx3, y - r + rby3, 3 * x + r + rbx3, y + r + rby3);
		SelectObject(hdc, oldbrush);

		oldbrush = (HBRUSH)SelectObject(hdc, wonbrush4);
		Ellipse(hdc, 3 * x - r + rbx4, 3 * y - r + rby4, 3 * x + r + rbx4, 3 * y + r + rby4);
		SelectObject(hdc, oldbrush);

		// ── [FIX] 오브젝트별 공전도형 그리기 ──────────

		// --- Object 1 ---
		{
			int cx = x, cy = y;
			int ox = 0, oy = 0;
			if (won1) { ox = (int)(orbitR * cos(angle1)); oy = (int)(orbitR * sin(angle1)); }
			if (rect1) { ox = rectmovex1; oy = rectmovey1; }
			if (tri1) { ox = trimovex1;  oy = trimovey1; }
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon1);
			if (gongjeonwon1) Ellipse(hdc, cx - gr + ox + rbx1, cy - gr + oy + rby1, cx + gr + ox + rbx1, cy + gr + oy + rby1);
			else             Rectangle(hdc, cx - gr + ox + rbx1, cy - gr + oy + rby1, cx + gr + ox + rbx1, cy + gr + oy + rby1);
			SelectObject(hdc, oldbrush);
		}
		// --- Object 2 ---
		{
			int cx = x, cy = 3 * y;
			int ox = 0, oy = 0;
			if (won2) { ox = (int)(orbitR * cos(angle2)); oy = (int)(orbitR * sin(angle2)); }
			if (rect2) { ox = rectmovex2; oy = rectmovey2; }
			if (tri2) { ox = trimovex2;  oy = trimovey2; }
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon2);
			if (gongjeonwon2) Ellipse(hdc, cx - gr + ox + rbx2, cy - gr + oy + rby2, cx + gr + ox + rbx2, cy + gr + oy + rby2);
			else             Rectangle(hdc, cx - gr + ox + rbx2, cy - gr + oy + rby2, cx + gr + ox + rbx2, cy + gr + oy + rby2);
			SelectObject(hdc, oldbrush);
		}
		// --- Object 3 ---
		{
			int cx = 3 * x, cy = y;
			int ox = 0, oy = 0;
			if (won3) { ox = (int)(orbitR * cos(angle3)); oy = (int)(orbitR * sin(angle3)); }
			if (rect3) { ox = rectmovex3; oy = rectmovey3; }
			if (tri3) { ox = trimovex3;  oy = trimovey3; }
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon3);
			if (gongjeonwon3) Ellipse(hdc, cx - gr + ox + rbx3, cy - gr + oy + rby3, cx + gr + ox + rbx3, cy + gr + oy + rby3);
			else             Rectangle(hdc, cx - gr + ox + rbx3, cy - gr + oy + rby3, cx + gr + ox + rbx3, cy + gr + oy + rby3);
			SelectObject(hdc, oldbrush);
		}
		// --- Object 4 ---
		{
			int cx = 3 * x, cy = 3 * y;
			int ox = 0, oy = 0;
			if (won4) { ox = (int)(orbitR * cos(angle4)); oy = (int)(orbitR * sin(angle4)); }
			if (rect4) { ox = rectmovex4; oy = rectmovey4; }
			if (tri4) { ox = trimovex4;  oy = trimovey4; }
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon4);
			if (gongjeonwon4) Ellipse(hdc, cx - gr + ox + rbx4, cy - gr + oy + rby4, cx + gr + ox + rbx4, cy + gr + oy + rby4);
			else             Rectangle(hdc, cx - gr + ox + rbx4, cy - gr + oy + rby4, cx + gr + ox + rbx4, cy + gr + oy + rby4);
			SelectObject(hdc, oldbrush);
		}

		// ── [FIX] 오브젝트별 배경도형 그리기 ──────────
		HBRUSH nullbrush = (HBRUSH)GetStockObject(NULL_BRUSH);
		SelectObject(hdc, nullbrush);
		HPEN oldpen;

		// --- Object 1 배경 ---
		oldpen = (HPEN)SelectObject(hdc, wonpen1);
		if (won1)  Ellipse(hdc, x - r1 + rbx1, y - r1 + rby1, x + r1 + rbx1, y + r1 + rby1);
		if (rect1) Rectangle(hdc, x - r1 + rbx1, y - r1 + rby1, x + r1 + rbx1, y + r1 + rby1);
		if (tri1) { POINT p[3] = { {r1 + r1 + rbx1,y - r1 + rby1},{x - r1 + rbx1,y + r1 + rby1},{x + r1 + rbx1,y + r1 + rby1} }; Polygon(hdc, p, 3); }
		SelectObject(hdc, oldpen);

		// --- Object 2 배경 ---
		oldpen = (HPEN)SelectObject(hdc, wonpen2);
		if (won2)  Ellipse(hdc, x - r1 + rbx2, 3 * y - r1 + rby2, x + r1 + rbx2, 3 * y + r1 + rby2);
		if (rect2) Rectangle(hdc, x - r1 + rbx2, 3 * y - r1 + rby2, x + r1 + rbx2, 3 * y + r1 + rby2);
		if (tri2) { POINT p[3] = { {r1 + r1 + rbx2,3 * y - r1 + rby2},{x - r1 + rbx2,3 * y + r1 + rby2},{x + r1 + rbx2,3 * y + r1 + rby2} }; Polygon(hdc, p, 3); }
		SelectObject(hdc, oldpen);

		// --- Object 3 배경 ---
		oldpen = (HPEN)SelectObject(hdc, wonpen3);
		if (won3)  Ellipse(hdc, 3 * x - r1 + rbx3, y - r1 + rby3, 3 * x + r1 + rbx3, y + r1 + rby3);
		if (rect3) Rectangle(hdc, 3 * x - r1 + rbx3, y - r1 + rby3, 3 * x + r1 + rbx3, y + r1 + rby3);
		if (tri3) { POINT p[3] = { {3 * (r1 + r1) - 10 + rbx3,y - r1 + rby3},{3 * x - r1 + rbx3,y + r1 + rby3},{3 * x + r1 + rbx3,y + r1 + rby3} }; Polygon(hdc, p, 3); }
		SelectObject(hdc, oldpen);

		// --- Object 4 배경 ---
		oldpen = (HPEN)SelectObject(hdc, wonpen4);
		if (won4)  Ellipse(hdc, 3 * x - r1 + rbx4, 3 * y - r1 + rby4, 3 * x + r1 + rbx4, 3 * y + r1 + rby4);
		if (rect4) Rectangle(hdc, 3 * x - r1 + rbx4, 3 * y - r1 + rby4, 3 * x + r1 + rbx4, 3 * y + r1 + rby4);
		if (tri4) { POINT p[3] = { {3 * (r1 + r1) - 10 + rbx4,3 * y - r1 + rby4},{3 * x - r1 + rbx4,3 * y + r1 + rby4},{3 * x + r1 + rbx4,3 * y + r1 + rby4} }; Polygon(hdc, p, 3); }
		SelectObject(hdc, oldpen);

		// ── 선택 표시 ──────────────────────────────
		SelectObject(hdc, nullbrush);
		oldpen = (HPEN)SelectObject(hdc, selectpen);
		int r2 = 120;
		if (select1) Rectangle(hdc, x - r2 + rbx1, y - r2 + rby1, x + r2 + rbx1, y + r2 + rby1);
		if (select2) Rectangle(hdc, x - r2 + rbx2, 3 * y - r2 + rby2, x + r2 + rbx2, 3 * y + r2 + rby2);
		if (select3) Rectangle(hdc, 3 * x - r2 + rbx3, y - r2 + rby3, 3 * x + r2 + rbx3, y + r2 + rby3);
		if (select4) Rectangle(hdc, 3 * x - r2 + rbx4, 3 * y - r2 + rby4, 3 * x + r2 + rbx4, 3 * y + r2 + rby4);
		SelectObject(hdc, oldpen);

		EndPaint(hWnd, &ps);
		return 0;
	}
	// ── 소멸 ───────────────────────────────────────
	case WM_DESTROY:
		DeleteObject(wonbrush1); DeleteObject(wonbrush2);
		DeleteObject(wonbrush3); DeleteObject(wonbrush4);
		DeleteObject(wonpen1);   DeleteObject(wonpen2);
		DeleteObject(wonpen3);   DeleteObject(wonpen4);
		DeleteObject(selectpen);
		DeleteObject(gongjeon1); DeleteObject(gongjeon2);
		DeleteObject(gongjeon3); DeleteObject(gongjeon4);
		KillTimer(hWnd, 1); KillTimer(hWnd, 2);
		KillTimer(hWnd, 3); KillTimer(hWnd, 4);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}