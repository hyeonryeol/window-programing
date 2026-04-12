#include <windows.h>


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

int g_rect[900][3];

void rectangle()
{
	int w = 30;
	int h = 30;
	for (int i = 0; i < 30; ++i)
	{
		for (int j = 30; j < 30; ++j)
		{
			int count = 0;

			int left =j * 30;
			int top = i * 30;
			int right = w + left;
			int bottom = h + top;
			count++;
			g_rect[count][0] = left;
			g_rect[count][1] = top;
			g_rect[count][2] = right;
			g_rect[count][3] = bottom;
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
		CW_USEDEFAULT, CW_USEDEFAULT, 1000, 1000,
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
		if (wParam == 'q' || wParam == 'Q')
		{
			PostQuitMessage(0);
		}
		return 0;
	}
	case WM_KEYDOWN:
	{

		return 0;
	}
	case WM_CREATE:
	{
		

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
	
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}