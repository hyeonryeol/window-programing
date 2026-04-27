#include <windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

HPEN   linepen, dotpen, signalboxpen, dotlinepen;
HBRUSH signalred, signalyellow, signalblue, carbrush, peoplebrush;
bool cross = false;
int speed1 = 250;
int  carx[10] = {};
int  cary[10] = {};
static int mx, my;

bool redlight = false;
bool yellowlight = false;
bool bluelight = true;

bool redlight2 = true;
bool yellowlight2 = false;
bool bluelight2 = false;

bool stopped[10] = {};  // 1, 2
int  stoppedY[10] = {};  // 각 차가 멈춘 y 위치
int stoppedX[10] = {}; //x위치

// ── 차 그리기 (랩어라운드 처리) ──────────────────────────────
void DrawCarV(HDC hdc, int left, int right, int top, int screenH)
{
	int bottom = top + 100;

	if (top <= 0)
	{
		// 화면 아래쪽에 남은 조각
		Rectangle(hdc, left, screenH + top, right, screenH);
		// 화면 위쪽에 걸친 조각
		if (bottom >= 0)
			Rectangle(hdc, left, 0, right, bottom);

	}
	else if (bottom >= screenH)
	{
		Rectangle(hdc, left, top, right, screenH);
		Rectangle(hdc, left, 0, right, bottom - screenH);
	}
	else
	{
		Rectangle(hdc, left, top, right, bottom);
	}
}
void DrawCarV1(HDC hdc, int screenW, int right, int top, int bottom)
{
	int left = right - 100;

	if (right <= 0)
	{
		return; // 차 전체가 화면 왼쪽 밖 → 그리지 않음
	}
	else if (left <= 0)
	{
		// 왼쪽 경계에 걸쳐 있을 때 → 두 조각으로 분할
		Rectangle(hdc, 0, top, right, bottom);               // 왼쪽 조각
		Rectangle(hdc, screenW + left, top, screenW, bottom); // 오른쪽에서 나오는 조각
	}
	else
	{
		Rectangle(hdc, left, top, right, bottom); // 화면 안 정상 그리기
	}
}
void CALLBACK TimerProc2(HWND hWnd, UINT, UINT, DWORD)
{

	if (redlight == true)
	{


			redlight = false;
			yellowlight = false;
			bluelight = true;
	
	}
	else if (bluelight == true)
	{
		

			bluelight = false;
			yellowlight = false;
			redlight = true;
	
	}
	KillTimer(hWnd, 2);
}
void CALLBACK TimerProc3(HWND hWnd, UINT, UINT, DWORD)
{
	yellowlight = true;
	SetTimer(hWnd, 2, 1000, (TIMERPROC)TimerProc2);

}
// ── 타이머 ────────────────────────────────────────────────────
void CALLBACK TimerProc1(HWND hWnd, UINT, UINT, DWORD)
{
	RECT rc;
	GetClientRect(hWnd, &rc);
	int H = rc.bottom;
	int L = rc.right;
	// 1) 이동
	if (bluelight)
	{
		cary[1] -= 10;
		cary[2] -= 10;
		cary[3] += 5;
		cary[4] += 10;
	}
	if (bluelight2)
	{
		carx[6] -= 10;
		carx[7] -= 10;
		carx[8] += 10;
		carx[9] += 10;

	}
	if (redlight)
	{
		if (2 * 266 + 40 + cary[5] > 266 && stopped[1] && stopped[2] && stopped[3] && stopped[4] && cross == true)
		{
			cary[5] -= 10;

		}
		if (2 * 266 + cary[5] < 533 && stopped[1] && stopped[2] && stopped[3] && stopped[4] && cross == false)
		{
			cary[5] += 10;

		}
		// ── 차1 처리 ──
		if (!stopped[1])
		{
			// 차2가 400에 서 있으면 차1은 510에 서야 함
			int myStop1 = (stopped[2] && stoppedY[2] == 510) ? 620 : 510;
			int top1 = 50 + cary[1];

			if (top1 > myStop1 && top1 - 10 <= myStop1)
			{
				// 정지선에 맞춰 세우기
				cary[1] = myStop1 - 50;
				stopped[1] = true;
				stoppedY[1] = myStop1;
			}
			else
			{
				cary[1] -= 10; // 접근 중이거나 이미 지남(랩 대기)
			}
		}

		// ── 차2 처리 (차1이 방금 멈췄을 수 있으니 stopped[1] 재확인) ──
		if (!stopped[2])
		{
			int myStop2 = (stopped[1] && stoppedY[1] == 510) ? 620 : 510;
			int top2 = 250 + cary[2];

			if (top2 > myStop2 && top2 - 10 <= myStop2)
			{
				cary[2] = myStop2 - 250;
				stopped[2] = true;
				stoppedY[2] = myStop2;
			}
			else
			{
				cary[2] -= 10;
			}
		}
		////////////////////////////////////////////////////////////////
		if (!stopped[3])
		{

			int myStop1 = (stopped[4] && stoppedY[4] == 250) ? 130 : 250;
			int bottom1 = 450 + cary[3];

			if (bottom1 <= myStop1 && bottom1 + 10 >= myStop1)
			{
				// 정지선에 맞춰 세우기
				cary[3] = myStop1 - 450;
				stopped[3] = true;
				stoppedY[3] = myStop1;
			}
			else
			{
				cary[3] += 10; // 접근 중이거나 이미 지남(랩 대기)
			}
		}


		if (!stopped[4])
		{
			int myStop2 = (stopped[3] && stoppedY[3] == 250) ? 130 : 250;
			int bottom2 = 650 + cary[4];

			if (bottom2 <= myStop2 && bottom2 + 10 >= myStop2)
			{
				cary[4] = myStop2 - 650;
				stopped[4] = true;
				stoppedY[4] = myStop2;
			}
			else
			{
				cary[4] += 10;
			}
		}
		
	}
	///////////////////////////////////////////////////////////////
	if (redlight2)
	{
		if (!stopped[6])
		{
			int myStop = (stopped[7] && stoppedX[2] == 630) ? 740 : 630;
			int right6 = 150 + carx[6];

			if (right6 <= myStop && right6 + 10 >= myStop)
			{
				carx[6] = myStop - 150; 
				stopped[6] = true;        
				stoppedX[1] = myStop;
			}
			else
			{
				carx[6] -= 10;
			}
		}
		if (!stopped[7])
		{
			int myStop = (stopped[6] && stoppedX[1] == 630) ? 740 : 630;
			int right7 = 650 + carx[7];

			if (right7 <= myStop && right7 + 10 >= myStop)
			{
				carx[7] = myStop - 650;
				stopped[7] = true;
				stoppedX[2] = myStop;
			}
			else
			{
				carx[7] -= 10;
			}
		}
		////////////////////////////////////////////////////////////////////////
		if (!stopped[8])
		{
			int myStop = (stopped[9] && stoppedX[4] == 630) ? 740 : 630;
			int right6 = 150 + carx[6];

			if (right6 <= myStop && right6 + 10 >= myStop)
			{
				carx[6] = myStop - 150;
				stopped[6] = true;
				stoppedX[1] = myStop;
			}
			else
			{
				carx[8] -= 10;
			}
		}
		if (!stopped[9])
		{
			int myStop = (stopped[8] && stoppedX[3] == 630) ? 740 : 630;
			int right7 = 650 + carx[7];

			if (right7 <= myStop && right7 + 10 >= myStop)
			{
				carx[7] = myStop - 650;
				stopped[7] = true;
				stoppedX[2] = myStop;
			}
			else
			{
				carx[9] -= 10;
			}
		}
	}
	// 2) 랩어라운드: 이동 후에 체크
	if (150 + cary[1] < 0) { cary[1] += H;  stopped[1] = false; stoppedY[1] = 0; }
	if (350 + cary[2] < 0) { cary[2] += H; stopped[2] = false; stoppedY[2] = 0; }
	if (350 + cary[3] > H) { cary[3] -= H;  stopped[3] = false; stoppedY[3] = 0; }
	if (550 + cary[4] > H) { cary[4] -= H; stopped[4] = false; stoppedY[4] = 0; }
	if (150 + carx[6] < 0) { carx[6] += L; stopped[6] = false; stoppedX[1] = 0; }
	if (650 + carx[7] < 0) { carx[7] += L; stopped[7] = false; stoppedX[2] = 0; }
		
	InvalidateRect(hWnd, NULL, TRUE);
}

// ── WinMain ───────────────────────────────────────────────────
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

	HWND hWnd = CreateWindowExW(0, CLASS_NAME, WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
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

// ── 윈도우 프로시저 ───────────────────────────────────────────
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static BOOL Selection;
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	int x = clientRect.right / 3;
	int y = clientRect.bottom / 3;
	int x1 = clientRect.right / 2;
	int y1 = clientRect.bottom / 2;

	switch (msg)
	{
	case WM_CREATE:
	{
		linepen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		dotpen = CreatePen(PS_DASH, 1, RGB(0, 0, 0));
		signalboxpen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		dotlinepen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
		signalred = CreateSolidBrush(RGB(255, 0, 0));
		signalyellow = CreateSolidBrush(RGB(255, 255, 0));
		signalblue = CreateSolidBrush(RGB(0, 0, 255));
		carbrush = CreateSolidBrush(RGB(0, 0, 255));
		peoplebrush = CreateSolidBrush(RGB(255, 255, 50));
		SetTimer(hWnd, 1, speed1, (TIMERPROC)TimerProc1);

		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		Selection = TRUE;

		// 빨간불
		if (55 < mx && mx < 95 && 5 < my && my < 45)
		{
			redlight = true; yellowlight = false; bluelight = false;
			if (cross == false)
			{
				cross = true;
			}
			else if (cross == true)
			{
				cross = false;
			}
		}
		//노란불
		if (105 < mx && mx < 145 && 5 < my && my < 45)
		{
			yellowlight = true;
			
			SetTimer(hWnd, 2, 1000, (TIMERPROC)TimerProc2);

		}
		// 파란불
		if (155 < mx && mx < 195 && 5 < my && my < 45)
		{
			redlight = false; yellowlight = false; bluelight = true;cross = true;
			stopped[1] = stopped[2] = stopped[3] = stopped[4] = stopped[6] =stopped[7] = false;
			stoppedY[1] = stoppedY[2] = stoppedY[3] = stopped[4] = stopped[6] = stopped[7] = 0;
		}

		return 0;
	}

	case WM_KEYDOWN:
	{
		if (wParam == VK_OEM_PLUS)
		{
			if (speed1 > 200)
			{

				KillTimer(hWnd, 1);
				speed1 -= 200;
				SetTimer(hWnd, 1, speed1, (TIMERPROC)TimerProc1);
			}
		}
		if (wParam == VK_OEM_MINUS)
		{
			KillTimer(hWnd, 1);
			speed1 += 200;
			SetTimer(hWnd, 1, speed1, (TIMERPROC)TimerProc1);
		}
		if (wParam == 'A')
		{
			SetTimer(hWnd, 3, 10000, (TIMERPROC)TimerProc3);
			
		}
		if (wParam == 'Q')
		{
			PostQuitMessage(0);
		}
	}
	return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// 차선
		HPEN oldpen = (HPEN)SelectObject(hdc, linepen);
		MoveToEx(hdc, x, 0, NULL); LineTo(hdc, x, y);
		MoveToEx(hdc, x, 2 * y, NULL); LineTo(hdc, x, 3 * y);
		MoveToEx(hdc, 2 * x, 0, NULL); LineTo(hdc, 2 * x, y);
		MoveToEx(hdc, 2 * x, 2 * y, NULL); LineTo(hdc, 2 * x, 3 * y);
		MoveToEx(hdc, 0, y, NULL); LineTo(hdc, x, y);
		MoveToEx(hdc, 2 * x, y, NULL); LineTo(hdc, 3 * x, y);
		MoveToEx(hdc, 0, 2 * y, NULL); LineTo(hdc, x, 2 * y);
		MoveToEx(hdc, 2 * x, 2 * y, NULL); LineTo(hdc, 3 * x, 2 * y);
		SelectObject(hdc, oldpen);

		oldpen = (HPEN)SelectObject(hdc, dotlinepen);
		MoveToEx(hdc, x1, 0, NULL); LineTo(hdc, x1, 3 * y);
		MoveToEx(hdc, 0, y1, NULL); LineTo(hdc, 2 * x1, y1);
		SelectObject(hdc, oldpen);

		oldpen = (HPEN)SelectObject(hdc, dotpen);
		MoveToEx(hdc, x, y, NULL); LineTo(hdc, x, 2 * y);
		MoveToEx(hdc, 2 * x, y, NULL); LineTo(hdc, 2 * x, 2 * y);
		MoveToEx(hdc, x, y, NULL); LineTo(hdc, 2 * x, y);
		MoveToEx(hdc, x, 2 * y, NULL); LineTo(hdc, 2 * x, 2 * y);
		SelectObject(hdc, oldpen);

		// 신호등 박스
		oldpen = (HPEN)SelectObject(hdc, signalboxpen);
		Rectangle(hdc, 50, 0, 200, 50);
		Rectangle(hdc, 550, 550, 700, 600);
		SelectObject(hdc, oldpen);

		// 신호등
		HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, signalred);
		if (redlight == true)
		{
			Ellipse(hdc, 55, 5, 95, 45);
		}
		if (redlight2 == true)
		{
			Ellipse(hdc, 555, 555, 595, 595);
			SelectObject(hdc, oldbrush);
		}
		if (yellowlight == true)
		{
			oldbrush = (HBRUSH)SelectObject(hdc, signalyellow);
			Ellipse(hdc, 105, 5, 145, 45);
		}
		if (yellowlight2 == true)
		{

			Ellipse(hdc, 605, 555, 645, 595);
			SelectObject(hdc, oldbrush);
		}
		if (bluelight == true)
		{
			oldbrush = (HBRUSH)SelectObject(hdc, signalblue);
			Ellipse(hdc, 155, 5, 195, 45);
		}
		if (bluelight2 == true)
		{
			Ellipse(hdc, 655, 555, 695, 595);
			SelectObject(hdc, oldbrush);
		}

		// 자동차 (랩어라운드)
		{
			int H = clientRect.bottom;
			int R = clientRect.right;
			oldbrush = (HBRUSH)SelectObject(hdc, carbrush);
			DrawCarV(hdc, 405 + carx[1], 460 + carx[1], 50 + cary[1], H);
			DrawCarV(hdc, 405 + carx[2], 460 + carx[2], 250 + cary[2], H);
			DrawCarV(hdc, 320 + carx[3], 375 + carx[3], 350 + cary[3], H);
			DrawCarV(hdc, 320 + carx[4], 375 + carx[4], 550 + cary[4], H);
			DrawCarV1(hdc, R, 150 + carx[6], 290 + cary[6], 350 + cary[6]);
			DrawCarV1(hdc, R, 650 + carx[7], 290 + cary[7], 350 + cary[7]);
			DrawCarV1(hdc, R, 150 + carx[8], 290 + cary[8], 350 + cary[8]);
			DrawCarV1(hdc, R, 650 + carx[9], 290 + cary[9], 350 + cary[9]);
			SelectObject(hdc, oldbrush);
		}
		oldbrush = (HBRUSH)SelectObject(hdc, peoplebrush);
		Ellipse(hdc, x-40, 2*y+cary[5], x, 2 * y + 40+cary[5]);
		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		KillTimer(hWnd, 3);
		DeleteObject(linepen);
		DeleteObject(dotpen);
		DeleteObject(signalboxpen);
		DeleteObject(dotlinepen);
		DeleteObject(signalred);
		DeleteObject(signalyellow);
		DeleteObject(signalblue);
		DeleteObject(carbrush);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}