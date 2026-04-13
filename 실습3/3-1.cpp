#include <windows.h>


// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 클래스 이름 / 창 제목
const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";
bool rect = true;
bool won = false;
bool tri = false;
int speed = 500;
bool hpress = false;
int rightmove = -1;
bool vpress = false;
int downmove = -1;
bool spress = false;
int cross = -1;
HBRUSH rectbrush;
HBRUSH wonbrush;
HBRUSH tribrush;
int x = 0;
int y = 0; //타이머로 늘릴친구
static int mx, my;


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
void CALLBACK TimerProc(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime) //--- 3번 아이디 타이머 함수
{
	HDC hdc;
	
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	if (hpress == true)
	{

		if (rightmove == 1)
		{

			x += 40;


			if (mx - 25 + x > clientRect.right - 40)
			{
				rightmove = 0;
				y += 50;
			}

		}
		else if (rightmove == 0)
		{

			x -= 40;

			if (mx - 25 + x < clientRect.left)
			{
				rightmove = 1;
				y += 50;
			}
		}
		if (my + 25 + y > clientRect.bottom)
		{
			x = -(mx - 25);
			y = -(my - 25);
			rightmove = 1;
		}
	}
	if (vpress == true)
	{
		if (downmove == 1)
		{
			y -= 40;

			if (my - 25 + y < clientRect.top)
			{
				downmove = 0;
				x += 50;
			}
		}
		else if (downmove == 0)
		{
			y += 40;

			if (my + 25 + y > clientRect.bottom)
			{
				downmove = 1;
				x += 50;
			}
		}
		if (mx + 25 + x > clientRect.right)
		{
			x = -(mx - 25);
			y = -(my - 25);
			downmove = 0;
		}
	}
	if (spress == true)
	{
		if (cross == 1)
		{
			x += 40; //오른쪽 위로
			y -= 40;
		}
		if (cross == 2)
		{
			x += 40; //오른쪽 아래
			y += 40;
		}
		if (cross == 3)
		{
			x -= 40; //왼쪽 아래
			y += 40;
		}
		if (cross == 4)
		{
			x -= 40; //왼쪽 위로
			y -= 40;
		}
		if (my - 25 + y < clientRect.top)
		{
			cross = 2;
		}
		if (mx + 25 + x > clientRect.right)
		{
			cross = 3;
		}
		if (my + 25 + y > clientRect.bottom)
		{
			cross = 4;
		}
		if (mx - 25 + x < clientRect.left)
		{
			cross = 1;
		}
	}
	
	InvalidateRect(hWnd, NULL, TRUE);
}
// 윈도우 메시지 처리 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	static BOOL Selection;


	switch (msg)
	{
	case WM_LBUTTONDOWN:
	{
		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		//--- 마우스 좌표값 저장: (mx, my)
		Selection = TRUE;
		x = 0;
		y = 0;
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_CREATE:
	{
		rectbrush = CreateSolidBrush(RGB(255, 0, 0));
		wonbrush = CreateSolidBrush(RGB(255, 255, 0));
		tribrush = CreateSolidBrush(RGB(0, 255, 255));
		SetTimer(hWnd, 3, speed, (TIMERPROC)TimerProc);
		return 0;
	}
	case WM_KEYDOWN:
	{
		if (wParam == 'R')
		{
			rect = true;
			won = false;
			tri = false;

		}
		if (wParam == 'E')
		{
			rect = false;
			won = true;
			tri = false;

		}
		if (wParam == 'T')
		{
			rect = false;
			won = false;
			tri = true;

		}
		if (wParam == 'H')
		{
			hpress = true;
			rightmove = 1;
		}
		if (wParam == 'V')
		{
			vpress = true;
			downmove = 0;
		}
		if (wParam == 'S')
		{
			spress = true;
			cross = 1;
		}
		if (wParam == 'P')
		{
			hpress = false;
			vpress = false;
			spress = false;
		}
		if (wParam == VK_OEM_PLUS)
		{
			if (speed > 100)
			{
			speed -= 100;
			KillTimer(hWnd, 3);                              // 기존 타이머 삭제
			SetTimer(hWnd, 3, speed, (TIMERPROC)TimerProc); // 새 속도로 재생성

			}
		}
		if (wParam == 'Q')
		{
			PostQuitMessage(0);
		}
		if (wParam == VK_OEM_MINUS)
		{
			if (speed < 5000)
			{
				speed += 100;
				KillTimer(hWnd, 3);                              // 기존 타이머 삭제
				SetTimer(hWnd, 3, speed, (TIMERPROC)TimerProc); // 새 속도로 재생성

			}
		}
		return 0;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, rectbrush);
		if (Selection && rect == true)
		{
			Rectangle(hdc, mx - 25 + x, my - 25 + y, mx + 25 + x, my + 25 + y);

			SelectObject(hdc, oldbrush);
		}
		if (Selection && won == true)
		{
			oldbrush = (HBRUSH)SelectObject(hdc, wonbrush);
			Ellipse(hdc, mx - 25 + x, my - 25 + y, mx + 25 + x, my + 25 + y);

			SelectObject(hdc, oldbrush);
		}
		if (Selection && tri == true)
		{
			oldbrush = (HBRUSH)SelectObject(hdc, tribrush);
			POINT p[3] = { { mx - 25 + x, my + y }, {mx + x , my - 50 + y}, {mx + 25 + x, my + y} };

			Polygon(hdc, p, 3);

			SelectObject(hdc, oldbrush);
		}
		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		DeleteObject(rectbrush);
		DeleteObject(wonbrush);
		DeleteObject(tribrush);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
