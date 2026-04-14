#include <windows.h>
#include <cstdlib>
#include <ctime>

// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 클래스 이름 / 창 제목
const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";
HBRUSH wonbrush;
HPEN wonpen;
HPEN selectpen;
bool won = true;
bool rect = false;
bool tri = false;
bool select1 = false;
bool select2 = false;
bool select3 = false;
bool select4 = false;

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

// 윈도우 메시지 처리 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
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
			InvalidateRect(hWnd, NULL, TRUE);
		}
		return 0;
	}
	case WM_CREATE:
	{
		wonpen = CreatePen(PS_SOLID, 3, RGB(0, 200, 255));
		wonbrush = CreateSolidBrush(RGB(r, g, b));
		selectpen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
		return 0;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, wonbrush);

		int r = 5;
		Ellipse(hdc, x - r, y - r, x + r, y + r);
		Ellipse(hdc, x - r, 3 * y - r, x + r, 3 * y + r);
		Ellipse(hdc, 3 * x - r, y - r, 3 * x + r, y + r);
		Ellipse(hdc, 3 * x - r, 3 * y - r, 3 * x + r, 3 * y + r);
		SelectObject(hdc, oldbrush);
		if (won == true)
		{

			HBRUSH nullbrush = (HBRUSH)GetStockObject(NULL_BRUSH); //채우기 없는 브러쉬
			SelectObject(hdc, nullbrush);
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			int r1 = 100;
			Ellipse(hdc, x - r1, y - r1, x + r1, y + r1);
			Ellipse(hdc, x - r1, 3 * y - r1, x + r1, 3 * y + r1);
			Ellipse(hdc, 3 * x - r1, y - r1, 3 * x + r1, y + r1);
			Ellipse(hdc, 3 * x - r1, 3 * y - r1, 3 * x + r1, 3 * y + r1);
			SelectObject(hdc, oldpen);
		}
		if (rect == true)
		{
			HBRUSH nullbrush = (HBRUSH)GetStockObject(NULL_BRUSH); //채우기 없는 브러쉬
			SelectObject(hdc, nullbrush);
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			int r1 = 100;
			Rectangle(hdc, x - r1, y - r1, x + r1, y + r1);
			Rectangle(hdc, x - r1, 3 * y - r1, x + r1, 3 * y + r1);
			Rectangle(hdc, 3 * x - r1, y - r1, 3 * x + r1, y + r1);
			Rectangle(hdc, 3 * x - r1, 3 * y - r1, 3 * x + r1, 3 * y + r1);
			SelectObject(hdc, oldpen);
		}
		if (tri == true)
		{
			HBRUSH nullbrush = (HBRUSH)GetStockObject(NULL_BRUSH); //채우기 없는 브러쉬
			SelectObject(hdc, nullbrush);
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			int r1 = 100;
			POINT p1[3] = { { r1+r1, y - r1 }, {x - r1 , y + r1}, {x + r1,  y + r1} };
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
		if (select1 == true)
		{
		Rectangle(hdc, x - r1, y - r1, x + r1, y + r1);
		SelectObject(hdc, oldpen);
		}
		
		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		DeleteObject(wonbrush);
		DeleteObject(wonpen);
		DeleteObject(selectpen);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}