#pragma comment(lib, "msimg32.lib")
#include <windows.h>
#include <atlbase.h>
#include <atlImage.h>
#include <windowsx.h>
#include <cstdlib>
#include <ctime>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

PAINTSTRUCT ps;

static CImage img[5];
static CImage imgBg;
static int xPos = 0, yPos = 200;
static int chaCount = 0;
static RECT rect;
int nomouserand = 0;
static CImage imgMouse[3];
static int mouseX = 400, mouseY = 100;  // ← 마우스 위치
static int mouseCount = 0;
bool mouse = false;
bool gorani = false;
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

	HWND hWnd = CreateWindowExW(0, CLASS_NAME, WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) return 0;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int speed = 100;
	switch (msg)
	{
	case WM_CREATE:
	{
		srand((unsigned int)time(NULL));
		imgBg.Load(L"gorani.bmp");
	

		for (int i = 0; i < 5; i++) {
			wchar_t path[32];
			swprintf(path, 32, L"goranirun%d.png", i + 1);
			img[i].Load(path);
			
		}
		for (int i = 0; i < 3; i++) {
			wchar_t paths[32];
			swprintf(paths, 32, L"mouse%d.png", i + 1);
			imgMouse[i].Load(paths);
		}
		nomouserand = rand() % 4 + 1;
		GetClientRect(hWnd, &rect);
		SetTimer(hWnd, 1, speed, nullptr);
		return 0;
	}

	case WM_SIZE:
	{

		GetClientRect(hWnd, &rect);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		mouse = true;
		mouseX = GET_X_LPARAM(lParam);
		mouseY = GET_Y_LPARAM(lParam);
		if (mouseX >= xPos && mouseY >= yPos && mouseX <= xPos + 300 && mouseY <= 300)
		{
			gorani = true;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		if (wParam & MK_LBUTTON) {  // 왼쪽 버튼 누른 채 이동
			mouseX = GET_X_LPARAM(lParam);
			mouseY = GET_Y_LPARAM(lParam);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		return 0;
	}
	case WM_LBUTTONUP:
	{
		mouse = false;
		gorani = false;
		nomouserand = rand() % 4 + 1;
		return 0;
	}
	case VK_OEM_PLUS:
	{
		if (speed < 50)
		{
			KillTimer(hWnd, 1);
			speed -= 50;
			SetTimer(hWnd, 1, speed, nullptr);
			InvalidateRect(hWnd, NULL, FALSE);
		}
	}
	case VK_OEM_MINUS:
	{
		KillTimer(hWnd, 1);
		speed += 50;
		SetTimer(hWnd, 1, speed, nullptr);
		InvalidateRect(hWnd, NULL, FALSE);
	}
	
	case WM_PAINT:
	{
		HDC hdc = BeginPaint(hWnd, &ps);

		HDC     memDC = CreateCompatibleDC(hdc);
		HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, hBitmap);

		FillRect(memDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

		// 배경
		if (!imgBg.IsNull())
			imgBg.Draw(memDC, 0, 0, rect.right, rect.bottom,
				0, 0, imgBg.GetWidth(), imgBg.GetHeight());

		// 스프라이트
		if (!img[chaCount].IsNull()) {
			HDC imgDC = img[chaCount].GetDC();
			TransparentBlt(memDC, xPos, yPos, 300, 300,
				imgDC, 0, 0,
				img[chaCount].GetWidth(),
				img[chaCount].GetHeight(),
				RGB(255, 255, 255));
			img[chaCount].ReleaseDC();
		}
		if (mouse == true)
		{

			if (!imgMouse[mouseCount].IsNull()) {
				imgMouse[mouseCount].Draw(memDC, mouseX - 50, mouseY - 50, 100, 100,
					0, 0,
					imgMouse[mouseCount].GetWidth(),
					imgMouse[mouseCount].GetHeight());
			}
		}
		if (gorani == true)
		{
			imgBg.Draw(memDC, xPos, yPos, 300, 300,
				0, 0, imgBg.GetWidth(), imgBg.GetHeight());
		}
		BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);

		SelectObject(memDC, oldBmp);
		DeleteObject(hBitmap);
		DeleteDC(memDC);
		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_TIMER:
		if (mouse == false)
		{
			
			if (nomouserand == 1)
			{
			xPos += 5;
			yPos -= 5;
			if (xPos > rect.right) xPos = 0;
			if (yPos < 0) yPos = 200;
			}
			else if (nomouserand == 2)
			{
				xPos += 5;
				yPos += 5;
				if (xPos > rect.right) xPos = 0;
				if (yPos > rect.bottom) yPos = 200;
			}
			else if (nomouserand == 3)
			{
				xPos -= 5;
				yPos += 5;
				if (xPos < rect.left) xPos = 0;
				if (yPos > rect.bottom) yPos = 200;
			}
			else if (nomouserand == 4)
			{
				xPos -= 5;
				yPos -= 5;
				if (xPos < rect.left) xPos = 0;
				if (yPos < 0) yPos = 200;
			}
		}
		if (mouse == true)
		{
			if (mouseX > xPos && mouseY > yPos )
			{
				xPos += 5;
				yPos += 5;
			
			}
			if (mouseX < xPos && mouseY < yPos)
			{
				xPos -= 5;
				yPos -= 5;
				
			}
			if (mouseX > xPos && mouseY < yPos)
			{
				xPos += 5;
				yPos -= 5;

			}
			if (mouseX < xPos && mouseY > yPos)
			{
				xPos -= 5;
				yPos += 5;

			}
		}
		chaCount = (chaCount + 1) % 5;
		InvalidateRect(hWnd, NULL, FALSE);
		return 0;

	case WM_KEYDOWN:
		if (wParam == 'Q') PostQuitMessage(0);
		if (wParam == 'R')
		{
			xPos = 0;
			yPos = 0;
			speed = 100;
		}
		return 0;

	case WM_DESTROY:
		KillTimer(hWnd, 1);
		for (int i = 0; i < 5; i++) img[i].Destroy();
		imgBg.Destroy();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}