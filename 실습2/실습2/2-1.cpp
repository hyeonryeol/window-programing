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
		hDC = BeginPaint(hWnd, &ps);
		rect.left = 325;
		rect.top = 260;
		rect.right = 475;
		rect.bottom = 340;

		wsprintf(lpOut, L" %d:(400,300)", 0);
		DrawText(hDC, lpOut, lstrlen(lpOut), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		RECT rects[15]; //저장할 곳

		for (int i = 0; i < 15; ++i)
		{
			RECT newRect; //새거 저장할 곳
			BOOL overlap; //겹치는지 여부판단
			do //반복
			{
				overlap = false; 
				int l = rand() % 600;      // 랜덤값 생성

				int t = rand() % 500;

				newRect.left = l;
				newRect.top = t;
				newRect.right = l + 150;
				newRect.bottom = t + 80;


				for (int j = 0; j < i; ++j) //비교
				{
					RECT temp;

					if (IntersectRect(&temp, &newRect, &rects[j])) //IntersectRect가 사각형이 겹치는지 검사하는 함수. 겹치면 다시 생성
					{
						overlap = TRUE;
						break;
					}
				}
			} while (overlap);

			rects[i] = newRect; //겹치지 않는 친구 저장

			TCHAR buf[100];
			wsprintf(buf, L" %d:(%d,%d)", i+1, rects[i].left,rects[i].top);
			DrawText(hDC, buf, lstrlen(buf), &rects[i], DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		}
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}
	return (DefWindowProc(hWnd, iMessage, wParam, IParam));

}
