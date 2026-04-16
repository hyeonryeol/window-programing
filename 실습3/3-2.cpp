#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <math.h>
// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 클래스 이름 / 창 제목
const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";
HBRUSH wonbrush;
HPEN wonpen;
HPEN selectpen;
HBRUSH gongjeon1;
HBRUSH gongjeon2;
HBRUSH gongjeon3;
HBRUSH gongjeon4;
static int mx, my;

int rbx1 = 0;
int rby1 = 0;
int rbx2 = 0;
int rby2 = 0;
int rbx3 = 0;
int rby3 = 0;
int rbx4 = 0;
int rby4 = 0;

bool won = true;
bool rect = false;
bool tri = false;

bool select1 = false;
bool select2 = false;
bool select3 = false;
bool select4 = false;

bool leftclick1 = false;
bool leftclick2 = false;
bool leftclick3 = false;
bool leftclick4 = false;

bool rbpress1 = false;
bool rbpress2 = false;
bool rbpress3 = false;
bool rbpress4 = false;

int cpress1 = 1;
int cpress2 = 1;
int cpress3 = 1;
int cpress4 = 1;

double angle1 = 0.0; // 현재 각도 (라디안)
double angle2 = 0.0; // 현재 각도 (라디안)
double angle3 = 0.0; // 현재 각도 (라디안)
double angle4 = 0.0; // 현재 각도 (라디안)
int orbitR = 100;   // 궤도 반지름

int rectmovex1 = -100;//배경도형 사각형일 때 공전하는 원위치
int rectmovey1 = -100;
int rectmovex2 = -100;
int rectmovey2 = -100;
int rectmovex3 = -100;
int rectmovey3 = -100;
int rectmovex4 = -100;
int rectmovey4 = -100;

int trimovex1 = 5;
int trimovey1 = -100;
int trimovex2 = 5;
int trimovey2 = -100;
int trimovex3 = 5;
int trimovey3 = -100;
int trimovex4 = 5;
int trimovey4 =-100;

int timerspeed1 = 500;
int timerspeed2 = 500;
int timerspeed3 = 500;
int timerspeed4 = 500;
// 프로그램 시작점
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	// 1) 윈도우 클래스 등록
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

	// 2) 윈도우 생성
	HWND hWnd = CreateWindowExW(
		0,
		CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		nullptr, nullptr, hInstance, nullptr
	);

	if (!hWnd) return 0;

	// 3) 창 표시
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 4) 메시지 루프
	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
void CALLBACK TimerProc1(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime) //--- 1번 아이디 타이머 함수
{
	HDC hdc;
	if (leftclick1 == true)
	{
		timerspeed1 += 50;
		KillTimer(hWnd, 1);                              // 기존 타이머 삭제
		SetTimer(hWnd, 1, timerspeed1, (TIMERPROC)TimerProc1);
		
		if (timerspeed1 > 500)
		{
			leftclick1 = false;
		}
	}
	if (won == true)
	{
		if (cpress1 == 1)
		{
			angle1 += 0.1; // 각도 증가 (값 키우면 더 빠름)
			if (angle1 > 6.28) angle1 = 0.0; // 2π 넘으면 초기화
		}
		if (cpress1 == 0)
		{
			angle1 -= 0.1; // 각도 증가 (값 키우면 더 빠름)
			if (angle1 < -6.28) angle1 = 0.0; // 2π 넘으면 초기화
		}
	}
	if (rect == true)
	{
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		int x = clientRect.right / 4;
		int y = clientRect.bottom / 4;
		if (cpress1 == 1)
		{
			if (-100 <= rectmovex1 && rectmovex1 <= 100 && rectmovey1 == -100) //위
			{
				rectmovex1 += 10;
			}
			if (-100 <= rectmovex1 && rectmovex1 <= 100 && rectmovey1 == 100) // 아래
			{
				rectmovex1 -= 10;
			}
			if (rectmovex1 == -100 && -100 <= rectmovey1 && rectmovey1 <= 100)
			{
				rectmovey1 -= 10;
			}
			if (rectmovex1 == 100 && -100 <= rectmovey1 && rectmovey1 <= 100) // 오른쪽
			{
				rectmovey1 += 10;
			}

		}
		if (cpress1 == 0)
		{
			if (-100 <= rectmovex1 && rectmovex1 <= 100 && rectmovey1 == -100) //위
			{
				rectmovex1 -= 10;
			}
			if (-100 <= rectmovex1 && rectmovex1 <= 100 && rectmovey1 == 100) // 아래
			{
				rectmovex1 += 10;
			}
			if (rectmovex1 == -100 && -100 <= rectmovey1 && rectmovey1 <= 100)
			{
				rectmovey1 += 10;
			}
			if (rectmovex1 == 100 && -100 <= rectmovey1 && rectmovey1 <= 100) // 오른쪽
			{
				rectmovey1 -= 10;
			}
		}
	}
	InvalidateRect(hWnd, NULL, TRUE);
}
void CALLBACK TimerProc2(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime) //--- 2번 아이디 타이머 함수
{
	HDC hdc;
	if (leftclick2 == true)
	{
		timerspeed2 += 50;
		KillTimer(hWnd, 2);                              // 기존 타이머 삭제
		SetTimer(hWnd, 2, timerspeed2, (TIMERPROC)TimerProc2);

		if (timerspeed2 > 500)
		{
			leftclick2 = false;
		}
	}
	if (won == true)
	{
		if (cpress2 == 1)
		{
			angle2 += 0.2; // 각도 증가 (값 키우면 더 빠름)
			if (angle2 > 6.28) angle2 = 0.0; // 2π 넘으면 초기화
		}
		if (cpress2 == 0)
		{
			angle2 -= 0.2; // 각도 증가 (값 키우면 더 빠름)
			if (angle2 < -6.28) angle2 = 0.0; // 2π 넘으면 초기화
		}
	}
	if (rect == true)
	{
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		int x = clientRect.right / 4;
		int y = clientRect.bottom / 4;
		if (cpress2 == 1)
		{
			if (-100 <= rectmovex2 && rectmovex2 <= 100 && rectmovey2 == -100) //위
			{
				rectmovex2 += 10;
			}
			if (-100 <= rectmovex2 && rectmovex2 <= 100 && rectmovey2 == 100) // 아래
			{
				rectmovex2 -= 10;
			}
			if (rectmovex2 == -100 && -100 <= rectmovey2 && rectmovey2 <= 100)
			{
				rectmovey2 -= 10;
			}
			if (rectmovex2 == 100 && -100 <= rectmovey2 && rectmovey2 <= 100) // 오른쪽
			{
				rectmovey2 += 10;
			}

		}
		if (cpress2 == 0)
		{
			if (-100 <= rectmovex2 && rectmovex2 <= 100 && rectmovey2 == -100) //위
			{
				rectmovex2 -= 10;
			}
			if (-100 <= rectmovex2 && rectmovex2 <= 100 && rectmovey2 == 100) // 아래
			{
				rectmovex2 += 10;
			}
			if (rectmovex2 == -100 && -100 <= rectmovey2 && rectmovey2 <= 100)
			{
				rectmovey2 += 10;
			}
			if (rectmovex2 == 100 && -100 <= rectmovey2 && rectmovey2 <= 100) // 오른쪽
			{
				rectmovey2 -= 10;
			}
		}
	}
	InvalidateRect(hWnd, NULL, TRUE);
}
void CALLBACK TimerProc3(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime) //--- 3번 아이디 타이머 함수
{
	HDC hdc;
	if (leftclick3 == true)
	{
		timerspeed3 += 50;
		KillTimer(hWnd, 3);                              // 기존 타이머 삭제
		SetTimer(hWnd, 3, timerspeed3, (TIMERPROC)TimerProc3);

		if (timerspeed3 > 500)
		{
			leftclick3 = false;
		}
	}
	if (won == true)
	{
		if (cpress3 == 1)
		{
			angle3 += 0.3; // 각도 증가 (값 키우면 더 빠름)
			if (angle3 > 6.28) angle3 = 0.0; // 2π 넘으면 초기화
		}
		if (cpress3 == 0)
		{
			angle3 -= 0.3; // 각도 증가 (값 키우면 더 빠름)
			if (angle3 < -6.28) angle3 = 0.0; // 2π 넘으면 초기화
		}
	}
	if (rect == true)
	{
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		int x = clientRect.right / 4;
		int y = clientRect.bottom / 4;
		if (cpress3 == 1)
		{
			if (-100 <= rectmovex3 && rectmovex3 <= 100 && rectmovey3 == -100) //위
			{
				rectmovex3 += 20;
			}
			if (-100 <= rectmovex3 && rectmovex3 <= 100 && rectmovey3 == 100) // 아래
			{
				rectmovex3 -= 20;
			}
			if (rectmovex3 == -100 && -100 <= rectmovey3 && rectmovey3 <= 100)
			{
				rectmovey3 -= 20;
			}
			if (rectmovex3 == 100 && -100 <= rectmovey3 && rectmovey3 <= 100) // 오른쪽
			{
				rectmovey3 += 20;
			}

		}
		if (cpress3 == 0)
		{
			if (-100 <= rectmovex3 && rectmovex3 <= 100 && rectmovey3 == -100) //위
			{
				rectmovex3 -= 20;
			}
			if (-100 <= rectmovex3 && rectmovex3 <= 100 && rectmovey3 == 100) // 아래
			{
				rectmovex3 += 20;
			}
			if (rectmovex3 == -100 && -100 <= rectmovey3 && rectmovey3 <= 100)
			{
				rectmovey3 += 20;
			}
			if (rectmovex3 == 100 && -100 <= rectmovey3 && rectmovey3 <= 100) // 오른쪽
			{
				rectmovey3 -= 20;
			}
		}
	}
	InvalidateRect(hWnd, NULL, TRUE);
}
void CALLBACK TimerProc4(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime) //--- 4번 아이디 타이머 함수
{
	HDC hdc;
	if (leftclick4 == true)
	{
		timerspeed4 += 50;
		KillTimer(hWnd, 4);                              // 기존 타이머 삭제
		SetTimer(hWnd, 4, timerspeed4, (TIMERPROC)TimerProc4);

		if (timerspeed4 > 500)
		{
			leftclick4 = false;
		}
	}
	if (won == true)
	{
		if (cpress4 == 1)
		{
			angle4 += 0.4; // 각도 증가 (값 키우면 더 빠름)
			if (angle4 > 6.28) angle4 = 0.0; // 2π 넘으면 초기화
		}
		if (cpress4 == 0)
		{
			angle4 -= 0.4; // 각도 증가 (값 키우면 더 빠름)
			if (angle4 < -6.28) angle4 = 0.0; // 2π 넘으면 초기화
		}
	}
	if (rect == true)
	{
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		int x = clientRect.right / 4;
		int y = clientRect.bottom / 4;
		if (cpress4 == 1)
		{
			if (-100 <= rectmovex4 && rectmovex4 <= 100 && rectmovey4 == -100) //위
			{
				rectmovex4 += 25;
			}
			if (-100 <= rectmovex4 && rectmovex4 <= 100 && rectmovey4 == 100) // 아래
			{
				rectmovex4 -= 25;
			}
			if (rectmovex4 == -100 && -100 <= rectmovey4 && rectmovey4 <= 100)
			{
				rectmovey4 -= 25;
			}
			if (rectmovex4 == 100 && -100 <= rectmovey4 && rectmovey4 <= 100) // 오른쪽
			{
				rectmovey4 += 25;
			}

		}
		if (cpress4 == 0)
		{
			if (-100 <= rectmovex4 && rectmovex4 <= 100 && rectmovey4 == -100) //위
			{
				rectmovex4 -= 25;
			}
			if (-100 <= rectmovex4 && rectmovex4 <= 100 && rectmovey4 == 100) // 아래
			{
				rectmovex4 += 25;
			}
			if (rectmovex4 == -100 && -100 <= rectmovey4 && rectmovey4 <= 100)
			{
				rectmovey4 += 25;
			}
			if (rectmovex4 == 100 && -100 <= rectmovey4 && rectmovey4 <= 100) // 오른쪽
			{
				rectmovey4 -= 25;
			}
		}
	}
	InvalidateRect(hWnd, NULL, TRUE);
}
// 윈도우 메시지 처리 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static BOOL Selection;
	srand((unsigned)time(NULL));
	int r = rand() % 255;
	int g = rand() % 255;
	int b = rand() % 255;
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	int x = clientRect.right / 4;
	int y = clientRect.bottom / 4;
	switch (msg)
	{
	case WM_RBUTTONDOWN:
	{
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		//--- 마우스 좌표값 저장: (mx, my)
		Selection = TRUE;
		
		if (select1 == true)
		{
			if (rbpress1 == false)
			{
			rbx1 = mx - 200;
			rby1 = my - 150;
			rbpress1 = true;
			}
			else if (rbpress1 == true)
			{
				rbx1 = 0;
				rby1 = 0;
				rbpress1 = false;
			}
		}
		if (select2 == true)
		{
			if (rbpress2 == false)
			{
				rbx2 = mx - 200;
				rby2 = my - 400;
				rbpress2 = true;
			}
			else if (rbpress2 == true)
			{
				rbx2 = 0;
				rby2 = 0;
				rbpress2 = false;
			}
		}
		if (select3 == true)
		{
			if (rbpress3 == false)
			{
				rbx3 = mx - 600;
				rby3 = my - 150;
				rbpress3 = true;
			}
			else if (rbpress3 == true)
			{
				rbx3 = 0;
				rby3 = 0;
				rbpress3 = false;
			}
		}
		if (select4 == true)
		{
			if (rbpress4 == false)
			{
				rbx4 = mx - 600;
				rby4 = my - 400;
				rbpress4 = true;
			}
			else if (rbpress4 == true)
			{
				rbx4 = 0;
				rby4 = 0;
				rbpress4 = false;
			}
		}
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{

		if (select1 == true)
		{
			
			if (timerspeed1 == 500)
			{	
				leftclick1 = true;
				timerspeed1 -= 400;
				KillTimer(hWnd, 1);                              // 기존 타이머 삭제
				SetTimer(hWnd, 1, timerspeed1, (TIMERPROC)TimerProc1); // 새 속도로 재생성
			}
			
		}
		if (select2 == true)
		{

			if (timerspeed2 == 500)
			{
				leftclick2 = true;
				timerspeed2 -= 400;
				KillTimer(hWnd, 2);                              // 기존 타이머 삭제
				SetTimer(hWnd, 2, timerspeed2, (TIMERPROC)TimerProc2); // 새 속도로 재생성
			}

		}
		if (select3 == true)
		{

			if (timerspeed3 == 500)
			{
				leftclick3 = true;
				timerspeed3 -= 400;
				KillTimer(hWnd, 3);                              // 기존 타이머 삭제
				SetTimer(hWnd, 3, timerspeed3, (TIMERPROC)TimerProc3); // 새 속도로 재생성
			}

		}
		if (select4 == true)
		{

			if (timerspeed4 == 500)
			{
				leftclick4 = true;
				timerspeed4 -= 400;
				KillTimer(hWnd, 4);                              // 기존 타이머 삭제
				SetTimer(hWnd, 4, timerspeed4, (TIMERPROC)TimerProc4); // 새 속도로 재생성
			}

		}
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}
	case WM_KEYDOWN:
	{
		if (wParam == 'R')
		{
			rect = true;
			won = false;
			tri = false;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == 'T')
		{
			tri = true;
			rect = false;
			won = false;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == 'Q')
		{
			PostQuitMessage(0);
		}
		if (wParam == '1')
		{

			select1 = true;
			select2 = false;
			select3 = false;
			select4 = false;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == '2')
		{
			select1 = false;
			select2 = true;
			select3 = false;
			select4 = false;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == '3')
		{
			select1 = false;
			select2 = false;

			select3 = true;
			select4 = false;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == '4')
		{
			select1 = false;
			select2 = false;
			select3 = false;
			select4 = true;

			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == 'C')
		{
			if (select1 == true)
			{
				if (cpress1 == 0)
				{
					cpress1 = 1;
				}
				else if (cpress1 == 1)
				{
					cpress1 = 0;
				}
			}
			if (select2 == true)
			{
				if (cpress2 == 0)
				{
					cpress2 = 1;
				}
				else if (cpress2 == 1)
				{
					cpress2 = 0;
				}
			}
			if (select3 == true)
			{
				if (cpress3 == 0)
				{
					cpress3 = 1;
				}
				else if (cpress3 == 1)
				{
					cpress3 = 0;
				}
			}
			if (select4 == true)
			{
				if (cpress4 == 0)
				{
					cpress4 = 1;
				}
				else if (cpress4 == 1)
				{
					cpress4 = 0;
				}
			}
		}
		return 0;
	}
	case WM_CREATE:
	{
		wonpen = CreatePen(PS_SOLID, 3, RGB(0, 200, 255));
		wonbrush = CreateSolidBrush(RGB(r, g, b));
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

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, wonbrush);

		int r = 5;
		Ellipse(hdc, x - r +rbx1, y - r+rby1, x + r+rbx1, y + r+rby1);
		Ellipse(hdc, x - r + rbx2, 3 * y - r+rby2, x + r+rbx2, 3 * y + r+rby2);
		Ellipse(hdc, 3 * x - r+rbx3, y - r+rby3, 3 * x + r+rbx3, y + r+rby3);
		Ellipse(hdc, 3 * x - r+rbx4, 3 * y - r+rby4, 3 * x + r+rbx4, 3 * y + r+rby4);
		SelectObject(hdc, oldbrush);

		int gr = 10;
		if (won == true) // 배경도형 원일때 공전원
		{

			int ox1 = (int)(orbitR * cos(angle1)); // x 오프셋
			int oy1 = (int)(orbitR * sin(angle1)); // y 오프셋

			int ox2 = (int)(orbitR * cos(angle2)); // x 오프셋
			int oy2 = (int)(orbitR * sin(angle2)); // y 오프셋

			int ox3 = (int)(orbitR * cos(angle3)); // x 오프셋
			int oy3 = (int)(orbitR * sin(angle3)); // y 오프셋

			int ox4 = (int)(orbitR * cos(angle4)); // x 오프셋
			int oy4 = (int)(orbitR * sin(angle4)); // y 오프셋
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon1);
			Ellipse(hdc, x - gr + ox1 + rbx1, y - gr + oy1+ rby1, x + gr + ox1+rbx1, y + gr + oy1+rby1);
			SelectObject(hdc, oldbrush);
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon2);
			Ellipse(hdc, x - gr + ox2+rbx2, 3 * y - gr + oy2+ rby2, x + gr + ox2+rbx2, 3 * y + gr + oy2+rby2);
			SelectObject(hdc, oldbrush);
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon3);
			Ellipse(hdc, 3 * x - gr + ox3 + rbx3, y - gr + oy3+rby3, 3 * x + gr + ox3+rbx3, y + gr + oy3+rby3);
			SelectObject(hdc, oldbrush);
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon4);
			Ellipse(hdc, 3 * x - gr + ox4 + rbx4, 3 * y - gr + oy4+rby4, 3 * x + gr + ox4+rbx4, 3 * y + gr + oy4+rby4);
			SelectObject(hdc, oldbrush);
		}
		if (rect == true) //배경도형 사각형일때 공전원
		{
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon1);
			Ellipse(hdc, x - gr + trimovex1 + rbx1, y - gr + rectmovey1+rby1, x + gr + trimovex1+rbx1, y + gr + rectmovey1+rby1);
			SelectObject(hdc, oldbrush);
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon2);
			Ellipse(hdc, x - gr + rectmovex2+rbx2, 3 * y - gr + rectmovey2+rby2, x + gr + rectmovex2+rbx2, 3 * y + gr + rectmovey2+rby2);
			SelectObject(hdc, oldbrush);
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon3);
			Ellipse(hdc, 3 * x - gr + rectmovex3+rbx3, y - gr + rectmovey3+rby3, 3 * x + gr + rectmovex3+rbx3, y + gr + rectmovey3+rby3);
			SelectObject(hdc, oldbrush);
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon4);
			Ellipse(hdc, 3 * x - gr + rectmovex4+rbx4, 3 * y - gr + rectmovey4+rby4, 3 * x + gr + rectmovex4+rbx4, 3 * y + gr + rectmovey4+rby4);
			SelectObject(hdc, oldbrush);
		}
		if (tri == true) //배경도형 삼각형일 때 공전하는 원
		{
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon1);
			Ellipse(hdc, x - gr + trimovex1 + rbx1, y - gr + trimovey1 + rby1, x + gr + trimovex1 + rbx1, y + gr + trimovey1 + rby1);
			SelectObject(hdc, oldbrush);
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon2);
			Ellipse(hdc, x - gr + trimovex2 + rbx2, 3 * y - gr + trimovey2 + rby2, x + gr + trimovex2 + rbx2, 3 * y + gr + trimovey2 + rby2);
			SelectObject(hdc, oldbrush);
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon3);
			Ellipse(hdc, 3 * x - gr + trimovex3 + rbx3, y - gr + trimovey3 + rby3, 3 * x + gr + trimovex3 + rbx3, y + gr + trimovey3 + rby3);
			SelectObject(hdc, oldbrush);
			oldbrush = (HBRUSH)SelectObject(hdc, gongjeon4);
			Ellipse(hdc, 3 * x - gr + trimovex4 + rbx4, 3 * y - gr + trimovey4 + rby4, 3 * x + gr + trimovex4 + rbx4, 3 * y + gr + trimovey4 + rby4);
			SelectObject(hdc, oldbrush);
		}
		if (won == true) //배경도형
		{

			HBRUSH nullbrush = (HBRUSH)GetStockObject(NULL_BRUSH); //채우기 없는 브러쉬
			SelectObject(hdc, nullbrush);
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			int r1 = 100;
			Ellipse(hdc, x - r1+rbx1, y - r1+rby1, x + r1+rbx1, y + r1+rby1);
			Ellipse(hdc, x - r1+rbx2, 3 * y - r1+rby2, x + r1+rbx2, 3 * y + r1+rby2);
			Ellipse(hdc, 3 * x - r1+rbx3, y - r1+rby3, 3 * x + r1+rbx3, y + r1+rby3);
			Ellipse(hdc, 3 * x - r1+rbx4, 3 * y - r1+rby4, 3 * x + r1+rbx4, 3 * y + r1+rby4);
			SelectObject(hdc, oldpen);
		}
		if (rect == true) //배경도형
		{
			HBRUSH nullbrush = (HBRUSH)GetStockObject(NULL_BRUSH); //채우기 없는 브러쉬
			SelectObject(hdc, nullbrush);
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			int r1 = 100;
			Rectangle(hdc, x - r1+rbx1, y - r1+rby1, x + r1+rbx1, y + r1+rby1);
			Rectangle(hdc, x - r1+rbx2, 3 * y - r1+rby2, x + r1+rbx2, 3 * y + r1+rby2);
			Rectangle(hdc, 3 * x - r1+rbx3, y - r1+rby3, 3 * x + r1+rbx3, y + r1+rby3);
			Rectangle(hdc, 3 * x - r1+rbx4, 3 * y - r1+rby4, 3 * x + r1+rbx4, 3 * y + r1+rby4);
			SelectObject(hdc, oldpen);
		}
		if (tri == true) //배경도형
		{
			HBRUSH nullbrush = (HBRUSH)GetStockObject(NULL_BRUSH); //채우기 없는 브러쉬
			SelectObject(hdc, nullbrush);
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			int r1 = 100;
			POINT p1[3] = { { r1 + r1, y - r1 }, {x - r1 , y + r1}, {x + r1,  y + r1} };
			Polygon(hdc, p1, 3);
			POINT p2[3] = { { r1 + r1, 3 * y - r1 }, {x - r1 , 3 * y + r1}, {x + r1, 3 * y + r1} };
			Polygon(hdc, p2, 3);
			POINT p3[3] = { { 3 * (r1 + r1) - 10, y - r1 }, {3 * x - r1 , y + r1}, {3 * x + r1, y + r1} };
			Polygon(hdc, p3, 3);
			POINT p4[4] = { { 3 * (r1 + r1) - 10,3 * y - r1 }, {3 * x - r1 , 3 * y + r1}, {3 * x + r1, 3 * y + r1} };
			Polygon(hdc, p4, 3);

			SelectObject(hdc, oldpen);
		}
		HBRUSH nullbrush = (HBRUSH)GetStockObject(NULL_BRUSH); //채우기 없는 브러쉬
		SelectObject(hdc, nullbrush);
		HPEN oldpen = (HPEN)SelectObject(hdc, selectpen);
		int r1 = 120;
		if (select1 == true) //선택표시
		{
			Rectangle(hdc, x - r1 + rbx1, y - r1+rby1, x + r1+rbx1, y + r1+rby1);
			SelectObject(hdc, oldpen);
		}
		if (select2 == true)
		{
			Rectangle(hdc, x - r1 + rbx2, 3 * y - r1+rby2, x + r1+rbx2, 3 * y + r1+rby2);
			SelectObject(hdc, oldpen);
		}
		if (select3 == true)
		{
			Rectangle(hdc, 3 * x - r1+rbx3, y - r1+rby3, 3 * x + r1+rbx3, y + r1+rby3);
			SelectObject(hdc, oldpen);
		}
		if (select4 == true)
		{
			Rectangle(hdc, 3 * x - r1+rbx4, 3 * y - r1+rby4, 3 * x + r1+rbx4, 3 * y + r1+rby4);
			SelectObject(hdc, oldpen);
		}

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		DeleteObject(wonbrush);
		DeleteObject(wonpen);
		DeleteObject(selectpen);
		DeleteObject(gongjeon1);
		DeleteObject(gongjeon2);
		DeleteObject(gongjeon3);
		DeleteObject(gongjeon4);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}