#include <windows.h>
#include <cstdlib> // srand, rand
#include <ctime>   // time

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";



HBRUSH triBrush;
HBRUSH rectBrush;
HBRUSH wonBrush;


int triangle = 0;
POINT g_tri[5][3];
void maketriangle()
{
	triangle = rand() % 3 + 1;
	for (int i = 0; i < triangle; ++i)
	{
		int triW = 30;
		int triH = 40;

		int left = 0 + rand() % 100 - triW + 40;
		int top = 0 + rand() % 400 - triH + 50;

		g_tri[i][0] = { left,            top + triH };
		g_tri[i][1] = { left + triW / 2, top };
		g_tri[i][2] = { left + triW,     top + triH };

		
	}
}

int rectangle = 0;
int g_rect[5][4];
void makerectangle()
{
	rectangle = rand() % 3 + 1;
	for (int i = 0; i < rectangle; ++i)
	{
		int w = 40, h = 30;

		int left = 251 + rand() % (500 - 251 - w + 1); // 251~460
		int top = 0 + rand() % (550 - h + 1);       // 0~520

		int right = left + w;
		int bottom = top + h;


		g_rect[i][0] = { left};
		g_rect[i][1] = { top};
		g_rect[i][2] = { right};
		g_rect[i][3] = { bottom };

		
	}
}
int won = 0;
int g_won[5][4];
void makewon()
{
	won = rand() % 3 + 1;
	for (int i = 0; i < won; ++i)
	{
		int w = 40, h = 30;

		int left = 500 + rand() % (700 - 501 - w + 1); // 251~460
		int top = 0 + rand() % (550 - h + 1);       // 0~520

		int right = left + w;
		int bottom = top + h;

		g_won[i][0] = { left };
		g_won[i][1] = { top };
		g_won[i][2] = { right };
		g_won[i][3] = { bottom };
		
	}
}
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

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 3) 메시지 루프
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
	switch (msg)
	{
	case WM_CREATE:
		// 초기화 코드
		srand((unsigned)time(NULL));
		triBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
		rectBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
		wonBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));

		maketriangle();
		makerectangle();
		makewon();




		return 0;

	case WM_KEYDOWN:
		// 키 입력 처리
		// 예: if (wParam == VK_ESCAPE) DestroyWindow(hWnd);
		if (wParam == VK_RETURN)
		{
			// 색도 바꾸고 싶으면 기존 브러시 삭제 후 재생성
			DeleteObject(triBrush);
			DeleteObject(rectBrush);
			DeleteObject(wonBrush);

			triBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
			rectBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
			wonBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));

			maketriangle();
			makerectangle();
			makewon();
			InvalidateRect(hWnd, NULL, TRUE); // WM_PAINT 다시 호출

		}
		if (wParam == '1')
		{
			DeleteObject(triBrush);
			triBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
			maketriangle();

			InvalidateRect(hWnd, NULL, TRUE); // WM_PAINT 다시 호출
		}
		if (wParam == '2')
		{
			DeleteObject(rectBrush);
			rectBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
			makerectangle();

			InvalidateRect(hWnd, NULL, TRUE); // WM_PAINT 다시 호출
		}
		if (wParam == '3')
		{
			DeleteObject(wonBrush);
			wonBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
			makewon();

			InvalidateRect(hWnd, NULL, TRUE); // WM_PAINT 다시 호출
		}
		return 0;

	case WM_CHAR:
		// 문자 입력 처리
		return 0;

	case WM_PAINT:
	{
		//srand((unsigned)time(NULL));
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		Rectangle(hdc, 0, 0, 250, 550);
		Rectangle(hdc, 251, 0, 500, 550);
		Rectangle(hdc, 501, 0, 760, 550);


	
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, triBrush);
		for (int i = 0; i < triangle; ++i)
			Polygon(hdc, g_tri[i], 3);   // 저장된 좌표만 그림
		SelectObject(hdc, oldBrush);

		

		oldBrush = (HBRUSH)SelectObject(hdc, rectBrush);
		for (int i = 0; i < rectangle; ++i)
		{
			Rectangle(hdc, g_rect[i][0], g_rect[i][1], g_rect[i][2], g_rect[i][3]);
		}
		SelectObject(hdc, oldBrush);
		

		oldBrush = (HBRUSH)SelectObject(hdc, wonBrush);
		for (int i = 0; i < won; ++i)
		{
			Ellipse(hdc, g_won[i][0], g_won[i][1], g_won[i][2], g_won[i][3]);
		}
		SelectObject(hdc, oldBrush);


		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		DeleteObject(triBrush);
		DeleteObject(rectBrush);
		DeleteObject(wonBrush);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}