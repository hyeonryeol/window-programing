#include <windows.h>


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

HBRUSH boardBrush;

int rectangle = 900;
int g_rect[900][4];       // [i][0..3] = left,top,right,bottom

void makerectangle()
{
	int w = 30, h = 30;

	for (int j = 0; j < 30; ++j)
	{

		for (int i = 0; i < 30; ++i)
		{
			int idx = j * 30 + i;

			int left = i * 30;
			int top = j * h;
			int right = left + w;
			int bottom = top + h;

			g_rect[idx][0] = { left };
			g_rect[idx][1] = { top };
			g_rect[idx][2] = { right };
			g_rect[idx][3] = { bottom };

		}

	}



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
		0,
		CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 950, 950,
		nullptr, nullptr, hInstance, nullptr
	);

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
	switch (msg)
	{
	case WM_CHAR:
	{
		if (wParam == 'q' || 'Q')
		{
			PostQuitMessage(0);
		}
	}
	case WM_KEYDOWN:
	{

		return 0;
	}
	case WM_CREATE:
	{
		boardBrush = CreateSolidBrush(RGB(255, 255, 255));
		makerectangle();

		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);




		for (int i = 0; i < 900; ++i)
		{
			Rectangle(hdc, g_rect[i][0], g_rect[i][1], g_rect[i][2], g_rect[i][3]);
		}




		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		DeleteObject(boardBrush);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}