#include <windows.h>

#include "resource.h"

HINSTANCE g_hInstance;
// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 클래스 이름 / 창 제목
const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";
HPEN rectbrush;
bool apress = false;
int x = 0;
int y = 0;
bool press1 = true;
bool press2 = false;
bool press4 = false;
bool press6 = false;
int selected = 0;
bool rpress = false;



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
        nullptr, nullptr, g_hInstance, nullptr
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
    static HBITMAP hBitmap;
    static RECT rect;
    HDC hDC, hMemDC;

    switch (msg)
    {
    case WM_CREATE:
    {
        hBitmap = (HBITMAP)LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP1)); //--- 리소스로 비트맵 읽어오기
        GetClientRect(hWnd, &rect); //--- 윈도우 크기 읽어오기
        GetObject(hBitmap, sizeof(BITMAP), &bmp); //--- 비트맵 정보 읽어오기 (크기 포함)
        rectbrush = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        int mousex = LOWORD(lParam);
        int mousey = HIWORD(lParam);
        if (press2 == true)
        {
            if (mousex < rect.right / 2)
            {
                selected = 1;
                
            }
            if (mousex > rect.right / 2)
            {
                selected = 2;
                
            }
        }
        if (press4 == true)
        {
            if (mousex < rect.right / 2 && mousey < rect.bottom / 2)
            {
                selected = 3;
            }
            if (mousex > rect.right / 2 && mousey < rect.bottom / 2)
            {
                selected = 4;
            }
            if (mousex < rect.right / 2 && mousey > rect.bottom / 2)
            {
                selected = 5;
            }
            if (mousex > rect.right / 2 && mousey > rect.bottom / 2)
            {
                selected = 6;
            }
        }
        if (press6 == true)
        {
            if (mousex < rect.right / 3 && mousey < rect.bottom / 2)
            {
                selected = 7;
            }
            if (mousex > rect.right / 3 &&mousex < 2*rect.right / 3 && mousey < rect.bottom / 2)
            {
                selected = 8;
            }
            if (mousex > 2*rect.right / 3 && mousex < 3 * rect.right / 3 && mousey < rect.bottom / 2)
            {
                selected = 9;
            }
            if (mousex < rect.right / 3 && mousey > rect.bottom / 2)
            {
                selected = 10;
            }
            if (mousex > rect.right / 3 && mousex < 2 * rect.right / 3 && mousey > rect.bottom / 2)
            {
                selected = 11;
            }
            if (mousex > 2 * rect.right / 3 && mousex < 3 * rect.right / 3 && mousey > rect.bottom / 2)
            {
                selected = 12;
            }
        }
        InvalidateRect(hWnd, NULL, true);
        return 0;
    }
    case WM_KEYDOWN:
    {
        if (wParam == 'A')
        {
            apress = true;
        }
        if (wParam == VK_OEM_PLUS)
        {
            x += 100;
            y += 100;
           
        }
        if (wParam == VK_OEM_MINUS)
        {
            x -= 100;
            y -= 100;
        }
        if (wParam == '1')
        {
            press1 = true;
            press2 = false;
            press4 = false;
            press6 = false;
          


        }
        if (wParam == '2')
        {
            press1 = false;
            press2 = true;
            press4 = false;
            press6 = false;
        
            
        }
        if (wParam == '4')
        {
            press1 = false;
            press2 = false;
            press4 = true;
            press6 = false;
          
        }
        if (wParam == '6')
        {
            press1 = false;
            press2 = false;
            press4 = false;
            press6 = true;
        
        }
        if (wParam == 'R')
        {
            rpress = !rpress;
            
        }
        if (wParam == 'Q')
        {
            PostQuitMessage(0);
        }
        InvalidateRect(hWnd, NULL, true);
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        hMemDC = CreateCompatibleDC(hdc); //--- 메모리 DC 만들기
        (HBITMAP)SelectObject(hMemDC, hBitmap); //--- 비트맵과 메모리 DC 연결하기
        HPEN oldpen = (HPEN)SelectObject(hdc, rectbrush);
        if (apress == false)
        {
        StretchBlt(hdc, 0, 0, rect.right-100, rect.bottom-100, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

        }

        if (apress == true && press1 == true)
        {
            StretchBlt(hdc, 0, 0, rect.right + x, rect.bottom + y, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY); //--- 윈도우 크기에 맞기 비트맵 그려주기
        }
        if (press2 == true)
        {
            if (selected == 1)
            {
                Rectangle(hdc, 0, 0, rect.right / 2, rect.bottom);
                
            }
            if (selected == 2)
            {
                Rectangle(hdc, rect.right / 2, 0, rect.right, rect.bottom);
            }
            StretchBlt(hdc, 1, 1, rect.right/2-2, rect.bottom-2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
            if (selected == 1 && rpress == true)
            {
                StretchBlt(hdc, 1, 1, rect.right / 2 - 2, rect.bottom - 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, DSTINVERT);

            }
            StretchBlt(hdc, rect.right/2+1, 1, rect.right/2-2, rect.bottom-2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
            if (selected == 2 && rpress == true)
            {
                StretchBlt(hdc, rect.right / 2 + 1, 1, rect.right / 2 - 2, rect.bottom - 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, DSTINVERT);

            }
        }
        if (press4 == true)
        {
            if (selected == 3)
            {
                Rectangle(hdc, 0, 0, rect.right / 2, rect.bottom / 2);
            }
            if (selected == 4)
            {
                Rectangle(hdc, rect.right / 2, 0, rect.right, rect.bottom / 2);
            }
            if (selected == 5)
            {
                Rectangle(hdc, 0, rect.bottom / 2, rect.right / 2, rect.bottom);
            }
            if (selected == 6)
            {
                Rectangle(hdc, rect.right / 2, rect.bottom / 2, rect.right, rect.bottom );
            }
            StretchBlt(hdc, 1, 1, rect.right / 2-2, rect.bottom/2-2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
            if (selected == 3 && rpress == true)
            {
                StretchBlt(hdc, 1, 1, rect.right / 2 - 2, rect.bottom / 2 - 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, DSTINVERT);
            }
            StretchBlt(hdc, rect.right/2+1, 1, rect.right / 2-2, rect.bottom / 2-2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
            if (selected == 4 && rpress == true)
            {
                StretchBlt(hdc, rect.right / 2 + 1, 1, rect.right / 2 - 2, rect.bottom / 2 - 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, DSTINVERT);
            }
            StretchBlt(hdc, 1, rect.bottom / 2+1, rect.right / 2-2, rect.bottom / 2-2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
            if (selected == 5 && rpress == true)
            {
                StretchBlt(hdc, 1, rect.bottom / 2 + 1, rect.right / 2 - 2, rect.bottom / 2 - 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, DSTINVERT);
            }
            StretchBlt(hdc, rect.right / 2+1, rect.bottom / 2+1, rect.right / 2-2, rect.bottom / 2-2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
            if (selected == 6 && rpress == true)
            {
                StretchBlt(hdc, rect.right / 2 + 1, rect.bottom / 2 + 1, rect.right / 2 - 2, rect.bottom / 2 - 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, DSTINVERT);
            }
        }
        if (press6 == true)
        {
            if (selected == 7)
            {
                Rectangle(hdc, 0, 0, rect.right / 3, rect.bottom / 2);
            }
            if (selected == 8)
            {
                Rectangle(hdc, rect.right / 3, 0, 2*rect.right / 3, rect.bottom / 2);
            }
            if (selected == 9)
            {
                Rectangle(hdc, 2*rect.right / 3, 0, 3*rect.right / 3, rect.bottom / 2);
            }
            if (selected == 10)
            {
                Rectangle(hdc, 0, rect.bottom / 2, rect.right / 3, rect.bottom );
            }
            if (selected == 11)
            {
                Rectangle(hdc, rect.right / 3, rect.bottom / 2, 2*rect.right / 3, rect.bottom );
            }
            if (selected == 12)
            {
                Rectangle(hdc, 2*rect.right / 3, rect.bottom / 2, 3*rect.right / 3, rect.bottom );

            }

            StretchBlt(hdc, 1, 1, rect.right / 3 -2, rect.bottom / 2-2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
            if (selected == 7 && rpress == true)
            {
                StretchBlt(hdc, 1, 1, rect.right / 3 - 2, rect.bottom / 2 - 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, DSTINVERT);
            }

            StretchBlt(hdc, rect.right / 3+1, 1, rect.right / 3-2, rect.bottom / 2-2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
            if (selected == 8 && rpress == true)
            {
                StretchBlt(hdc, rect.right / 3 + 1, 1, rect.right / 3 - 2, rect.bottom / 2 - 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, DSTINVERT);
            }
            StretchBlt(hdc, 2*rect.right / 3+1,1, rect.right / 3-2, rect.bottom / 2-2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
            if (selected == 9 && rpress == true)
            {
                StretchBlt(hdc, 2 * rect.right / 3 + 1, 1, rect.right / 3 - 2, rect.bottom / 2 - 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, DSTINVERT);
            }
            StretchBlt(hdc, 1, rect.bottom / 2+1, rect.right / 3-2, rect.bottom / 2-2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
            if (selected == 10 && rpress == true)
            {
                StretchBlt(hdc, 1, rect.bottom / 2 + 1, rect.right / 3 - 2, rect.bottom / 2 - 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, DSTINVERT);
            }
            StretchBlt(hdc, rect.right / 3+1, rect.bottom / 2+1, rect.right / 3-2, rect.bottom / 2-2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
            if (selected == 11 && rpress == true)
            {
                StretchBlt(hdc, rect.right / 3 + 1, rect.bottom / 2 + 1, rect.right / 3 - 2, rect.bottom / 2 - 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, DSTINVERT);
            }
            StretchBlt(hdc, 2 * rect.right / 3+1, rect.bottom / 2+1, rect.right / 3-2, rect.bottom / 2-2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
            if (selected == 12 && rpress == true)
            {
                StretchBlt(hdc, 2 * rect.right / 3 + 1, rect.bottom / 2 + 1, rect.right / 3 - 2, rect.bottom / 2 - 2, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, DSTINVERT);
            }
        }
        DeleteDC(hMemDC);
        SelectObject(hdc, oldpen);
        // TODO: 여기에 그리기 코드 작성

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        DeleteObject(rectbrush);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}