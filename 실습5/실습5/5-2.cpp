#include <windows.h>

#include "resource.h"

// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 클래스 이름 / 창 제목
const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";
HINSTANCE g_hInstance;

bool show1 = false;
bool show2 = false;
bool divide = false;
// 프로그램 시작점
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	g_hInstance = hInstance;
	// 1) 윈도우 클래스 등록
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
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
	static BITMAP bmp;
	static BITMAP bmp2;  
	static RECT rect;

	GetClientRect(hWnd, &rect); //--- 윈도우 크기 읽어오기
	static HBITMAP hBitmap;
	static HBITMAP hBitmap2;
	HDC hDC, hMemDC;
	switch (msg)
	{
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
			case IDR_MENU1:
			{

				break;
			}
			case ID_PICTURE_1:
			{
				show1 = true;
				show2 = false;
				break;
			}
			case ID_PICTURE_2:
			{
				show2 = true;
				show1 = false;
				break;
			}
			case ID_PICTUREDIVIDE_3:
			{
				divide = true;
			}
		}
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}
	case WM_CREATE:
	{
		hBitmap = (HBITMAP)LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP1)); //--- 리소스로 비트맵 읽어오기
		hBitmap2 = (HBITMAP)LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
		
		GetObject(hBitmap, sizeof(BITMAP), &bmp); //--- 비트맵 정보 읽어오기 (크기 포함)
		GetObject(hBitmap2, sizeof(BITMAP), &bmp2);
	}
	case WM_KEYDOWN:
	{
		return 0;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		hMemDC = CreateCompatibleDC(hdc); //--- 메모리 DC 만들기
		if (show1 == true)
		{

		(HBITMAP)SelectObject(hMemDC, hBitmap); //--- 비트맵과 메모리 DC 연결하기
		// TODO: 여기에 그리기 코드 작성
		StretchBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY); //--- 윈도우 크기에 맞기 비트
		if (divide == true)
		{

			BitBlt(hdc, 0, 0, rect.right / 3, rect.bottom / 3, hMemDC, 0, 0, SRCCOPY);
		}
		DeleteDC(hMemDC);
		}
		if (show2 == true)
		{

		(HBITMAP)SelectObject(hMemDC, hBitmap2); //--- 비트맵과 메모리 DC 연결하기
		StretchBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, bmp2.bmWidth, bmp2.bmHeight, SRCCOPY); //--- 윈도우 크기에 맞기 비트
		}
		
		DeleteDC(hMemDC);
		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}