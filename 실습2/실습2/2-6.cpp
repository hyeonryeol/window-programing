#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"windows program 3";

//전역 변수
int startX, startY, r, g, b, R, G, B, n, count;         // 시작 위치
COLORREF textColor;         // 텍스트 색상
TCHAR lines[5][21];         // 최대 5줄, 각 20자
COLORREF BkColor;           //배경 색
int CNT = 0;
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    srand(time(NULL));

    // 랜덤 위치/색상 초기화
    startX = rand() % 600;       // 0~600
    startY = rand() % 500;       // 0~500
    int r = rand() % 256;
    int g = rand() % 256;
    int b = rand() % 256;
    int R = rand() % 256;
    int G = rand() % 256;
    int B = rand() % 256;
    int n = rand() % 200;
    int count = rand() % 16 + 5;
    textColor = RGB(r, g, b);
    BkColor = RGB(R, G, B);
    
   

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

    while (GetMessage(&Message, 0, 0, 0)) {
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
    case WM_KEYDOWN:
    {
        if (wParam == VK_RETURN)
        {
            if (CNT < 10)
            {
                startX = rand() % 600;       // 0~600
                startY = rand() % 500;       // 0~500
                int r = rand() % 256;
                int g = rand() % 256;
                int b = rand() % 256;
                int R = rand() % 256;
                int G = rand() % 256;
                int B = rand() % 256;
                int n = rand() % 200;
                int count = rand() % 16 + 5;
                CNT++;
                InvalidateRect(hWnd, NULL, TRUE);
            }

        }

    }
    case WM_CHAR:
    {
        TCHAR ch = (TCHAR)wParam;

        if (ch == 'q')
        {
            PostQuitMessage(0);
        }
        else if (ch == 'r')
        {

        }
        else if (ch == 'a')
        {
            
        }

        return 0;
    }

    case WM_PAINT:
    {
        hDC = BeginPaint(hWnd, &ps);
        SetTextColor(hDC, textColor);
        SetBkMode(hDC, TRANSPARENT);
        TextOut(hDC, startX, startY, L"출력할 문자열", );



        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, iMessage, wParam, lParam);
}