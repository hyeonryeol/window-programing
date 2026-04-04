#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"WindowClassName";
LPCTSTR lpszWindowName = L"랜덤사각형그리기";


int    rectX, rectY;       // 사각형 위치
int    rectW, rectH;       // 사각형 크기
TCHAR  garoStr[100];       // 가로 문자열
TCHAR  seroStr[100];       // 세로 문자열
int    garoLen;            // 가로 문자열 길이
int    seroLen;            // 세로 문자열 길이
COLORREF garoColor;        // 가로 색상
COLORREF seroColor;        // 세로 색상

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

//랜덤 알파벳 문자열 생성 
void makeRandStr(TCHAR* str, int len)
{
	for (int i = 0; i < len; i++)
		str[i] = L'A' + rand() % 26;
	str[len] = L'\0';
}

//랜덤 색상
COLORREF randColor()
{
	return RGB(rand() % 256, rand() % 256, rand() % 256);
}

// 값 초기화 
void initValues(HWND hWnd)
{
	RECT client;
	GetClientRect(hWnd, &client);
	int gw = client.right;   // 창 너비
	int sh = client.bottom;  // 창 높이

	if (gw < 220 || sh < 220)
	{
		rectW = rectH = 0; 
		return;
	}
	// 사각형 크기
	rectW = 100 + rand() % (gw / 2 - 100);
	
	rectH = 100 + rand() % (sh / 2 - 100);
	
	// 사각형 위치
	rectX = 1 + rand() % (gw / 2);
	rectY = 1 + rand() % (sh / 2);



	// 가로 문자열: 사각형 너비에 맞게 글자 수 결정
	// (글자 하나 약 8픽셀 기준)
	garoLen = rectW / 8;
	if (garoLen < 1) garoLen = 1;
	if (garoLen > 99) garoLen = 99;
	makeRandStr(garoStr, garoLen);

	// 세로 문자열: 사각형 높이에 맞게 글자 수 결정
	// (글자 하나 약 16픽셀 기준)
	seroLen = rectH / 16;
	if (seroLen < 1) seroLen = 1;
	if (seroLen > 99) seroLen = 99;
	makeRandStr(seroStr, seroLen);

	garoColor = randColor();
	seroColor = randColor();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	srand(time(NULL));

	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW,
		100, 100, 800, 600, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;

	switch (iMessage)
	{
	case WM_CREATE: // 창 처음 만들어질 때
		initValues(hWnd);
		break;

	case WM_SIZE: // 창 크기 바뀔 때 → 새로 랜덤
		initValues(hWnd);
		InvalidateRect(hWnd, NULL, TRUE);
		break;

	case WM_PAINT:
	{
		if (rectW <= 0 || rectH <= 0)
		{
			EndPaint(hWnd, &ps);
			return 0;
		}
		if (rectW > 101 && rectH > 101)
		{


			hDC = BeginPaint(hWnd, &ps);
			SetBkMode(hDC, TRANSPARENT);

			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			int charH = tm.tmHeight; // 글자 높이



			// 가로 문자열 실제 너비 측정
			SIZE sz; //너비
			GetTextExtentPoint32(hDC, garoStr, garoLen, &sz);
			int strW = sz.cx;  // 문자열 전체 너비
			int charW = strW / garoLen;  // 글자 하나 너비

			// 위쪽 가로선
			SetTextColor(hDC, garoColor);
			TextOut(hDC, rectX, rectY, garoStr, garoLen);

			//아래쪽 가로선 
			TextOut(hDC, rectX, rectY + rectH, garoStr, garoLen);

			//왼쪽 세로선
			SetTextColor(hDC, seroColor);
			for (int i = 0; i < seroLen; i++)
			{
				TCHAR ch[2] = { seroStr[i], L'\0' };
				TextOut(hDC, rectX, rectY + i * charH, ch, 1);
			}

			// 오른쪽 세로선
			int rightX = rectX + strW - charW; // 실제 너비 기준 마지막 글자 위치
			for (int i = 0; i < seroLen; i++)
			{
				TCHAR ch[2] = { seroStr[i], L'\0' };
				TextOut(hDC, rightX, rectY + i * charH, ch, 1);
			}

			EndPaint(hWnd, &ps);
			return 0;
		}


	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}
