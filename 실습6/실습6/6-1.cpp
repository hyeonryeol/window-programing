#include <windows.h>
#include <windowsx.h> 
#include <TCHAR.h>

#include "resource.h"
HWND hDlg;
int randspacex = 0;
int randspacey = 0;
int g_rect[3][4] = {};
bool rect = false;
void smallrect()
{
	int w = 20;
	int h = 20;
	g_rect[1][0] = randspacex;
	g_rect[1][1] = randspacey;
	g_rect[1][2] = randspacex + w;
	g_rect[1][3] = randspacey + h;
}
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK Dlalog_Proc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCzmdLine, int nCmdShow)
{
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = TEXT("MyWindow");
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	g_hInst = hInstance;
	RegisterClass(&wc);

	HWND hWnd = CreateWindow(
		TEXT("MyWindow"), TEXT("제목"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		NULL, NULL, hInstance, NULL
	);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

BOOL CALLBACK Dlalog_Proc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hDC;

	switch (iMsg) {
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: //--- 버튼
			MessageBox(hDlg, L"test", L"test, ", MB_OK);
			break;
		case IDC_RADIO1: //--- rectangle 버튼
			hDC = GetDC(hDlg);
			rect = true;
			ReleaseDC(hDlg, hDC);
			break;

		case IDCANCEL: //--- 버튼
			EndDialog(hDlg, 0);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		break;
	}
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CREATE:
		// 초기화
		smallrect();
		return 0;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hDlg, &ps);
		if (rect == true)
		{

		Rectangle(hdc, g_rect[1][0], g_rect[1][1], g_rect[1][2], g_rect[1][3]);
		}
		// 그리기 코드
			
		EndPaint(hDlg, &ps);
		return 0;
	}

	case WM_KEYDOWN:

	{
		// 키 입력 처리
		return 0;
	}

	case WM_LBUTTONDOWN: //--- 마우스 클릭하면 대화상자 띄우기
	{

		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, (DLGPROC)Dlalog_Proc);
		break;
	}

	case WM_DESTROY:
	{

		PostQuitMessage(0);
		return 0;
	}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
