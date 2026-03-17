#include <windows.h> //윈도우 좌표값, 창크기, 색, 이름
#include <tchar.h>
#include<time.h>
#include<stdlib.h>

HINSTANCE g_hlnst;
LPCTSTR IpszClass = L"Window Class Name";
LPCTSTR IpszWindowName = L"windows program 3";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessgae, WPARAM wParam, LPARAM IParam);

int WINAPI WinMain(HINSTANCE hlnstance, HINSTANCE hPrevlnstance, LPSTR IpszCmdParam, int nCmdShow)
{
	srand(time(NULL)); // 랜덤 시드 초기화
	HWND hWND;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hlnst = hlnstance;

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hlnstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = IpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	hWND = CreateWindow(IpszClass, IpszWindowName, WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_HSCROLL | WS_VSCROLL | WS_MAXIMIZEBOX | WS_MINIMIZEBOX, 100, 100, 800, 600, NULL, (HMENU)NULL, hlnstance, NULL);

	ShowWindow(hWND, nCmdShow);
	UpdateWindow(hWND);

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);

	}
	return Message.wParam;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
	PAINTSTRUCT ps;
	HDC hDC;

	RECT rect;
	int x = 0, y = 0;
	TCHAR lpOut[100];



	switch (iMessage) {
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		rect.left = 0;
		rect.top = 0;
		rect.right = 100;
		rect.bottom = 50;

		for (int k = 1; k < 10; ++k)
		{

			for (int i = 1; i < 10; ++i)
			{

				RECT r = rect;
				r.top += i * 30;
				r.bottom = r.top + 30;

				r.left += (k - 2) * 95;
				r.right = r.left + 80;

				wsprintf(lpOut, L" %d X %d =%d", k, i, i * k);
				DrawText(hDC, lpOut, lstrlen(lpOut), &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
		}
		int rowbottom=300;
		for (int k = 9; k >= 1; --k)
		{

			for (int i = 1; i < 10; ++i)
			{

				RECT r = rect;
				r.top += rowbottom + i * 30;
				r.bottom = r.top + 30;

				r.left += (9 - k) * 95;
				r.right = r.left + 80;

				wsprintf(lpOut, L" %d X %d =%d", k, i, k * i);
				DrawText(hDC, lpOut, lstrlen(lpOut), &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
		}




	}
	EndPaint(hWnd, &ps);
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}
	return (DefWindowProc(hWnd, iMessage, wParam, IParam));

}
