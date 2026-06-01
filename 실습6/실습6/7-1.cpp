#include <windows.h>
#include <windowsx.h>
#include "resource1.h"

HINSTANCE g_hInst;


LRESULT CALLBACK ChildProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    g_hInst = hInstance;

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"ParentClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassEx(&wc);

    wc.hCursor = LoadCursor(NULL, IDC_HELP);
    wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wc.lpszClassName = L"ChildClass";
    wc.lpfnWndProc = ChildProc;
    RegisterClassEx(&wc);

    HWND hWnd = CreateWindow(
        L"ParentClass", L"제목",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 700,
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
#define IDC_BUTTON1 100
#define IDC_BUTTON2 101
#define IDC_BUTTON3 102
#define IDC_BUTTON4 103
#define IDC_LIST 104
LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    static HWND hButton;
    static HWND hList;
    static HWND hChild;
    switch (message) {
    case WM_CREATE:
    {
       

        
        hList = CreateWindow(L"listbox", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS,
            700, 50, 200, 300,
            hWnd, (HMENU)IDC_LIST, g_hInst, NULL);
        hChild = CreateWindow(L"ChildClass", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            50, 50, 600, 550,
            hWnd, NULL, g_hInst, NULL);
      


        hButton = CreateWindow(L"button", L"Select",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
            700, 400, 100, 25, hWnd, (HMENU)IDC_BUTTON1, g_hInst, NULL);
        hButton = CreateWindow(L"button", L"Done",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            700, 440, 100, 25, hWnd, (HMENU)IDC_BUTTON2, g_hInst, NULL);
        hButton = CreateWindow(L"button", L"Move",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            700, 480, 100, 25, hWnd, (HMENU)IDC_BUTTON3, g_hInst, NULL);
        hButton = CreateWindow(L"button", L"Stop",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            700, 520, 100, 25, hWnd, (HMENU)IDC_BUTTON4, g_hInst, NULL);
        // WM_CREATE 안에서
        HBITMAP hBmp1 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
        int idx = SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)L"항목1");
        SendMessage(hList, LB_SETITEMDATA, idx, (LPARAM)hBmp1);
        HBITMAP hBmp2 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
        idx = SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)L"항목2");
        SendMessage(hList, LB_SETITEMDATA, idx, (LPARAM)hBmp2);
        HBITMAP hBmp3 = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
        idx = SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)L"항목3");
        SendMessage(hList, LB_SETITEMDATA, idx, (LPARAM)hBmp3);                                                               
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BUTTON1:
            hdc = GetDC(hWnd);
            TextOut(hdc, 0, 100, L"Hello World", 11);
            ReleaseDC(hWnd, hdc);
            break;
        case IDC_BUTTON2:
            hdc = GetDC(hWnd);
            TextOut(hdc, 0, 100, L"Hello Worl", 11);
            ReleaseDC(hWnd, hdc);
            break;
        case IDC_BUTTON3:
            hdc = GetDC(hWnd);
            TextOut(hdc, 0, 100, L"Hello Word", 11);
            ReleaseDC(hWnd, hdc);
            break;
        case IDC_BUTTON4:
            hdc = GetDC(hWnd);
            TextOut(hdc, 0, 100, L"Hello Wrld", 11);
            ReleaseDC(hWnd, hdc);
            break;
   
        case IDC_LIST:
            if (HIWORD(wParam) == LBN_SELCHANGE) {
                int idx = SendMessage(hList, LB_GETCURSEL, 0, 0);
                // 선택된 항목의 비트맵을 차일드로 전달
                HBITMAP hBmp = (HBITMAP)SendMessage(hList, LB_GETITEMDATA, idx, 0);
                SendMessage(hChild, WM_USER + 1, 0, (LPARAM)hBmp);
            }
            break;
        }
        return 0;
    case WM_MEASUREITEM: {
        MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
        mis->itemHeight = 40;  // 항목 높이
        return TRUE;
    }
    case WM_DRAWITEM: {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
        if (dis->CtlID != IDC_LIST) break;

        // 배경
        FillRect(dis->hDC, &dis->rcItem,
            (dis->itemState & ODS_SELECTED) ?
            (HBRUSH)GetStockObject(LTGRAY_BRUSH) :
            (HBRUSH)GetStockObject(WHITE_BRUSH));

        // 비트맵 그리기
        HBITMAP hBmp = (HBITMAP)SendMessage(hList, LB_GETITEMDATA, dis->itemID, 0);
        if (hBmp) {
            HDC memDC = CreateCompatibleDC(dis->hDC);
            HBITMAP old = (HBITMAP)SelectObject(memDC, hBmp);
            BitBlt(dis->hDC, dis->rcItem.left, dis->rcItem.top, 40, 40, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, old);
            DeleteDC(memDC);
        }

        // 텍스트
        TCHAR buf[64];
        SendMessage(hList, LB_GETTEXT, dis->itemID, (LPARAM)buf);
        TextOut(dis->hDC, dis->rcItem.left + 45, dis->rcItem.top + 10, buf, lstrlen(buf));
        return TRUE;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_KEYDOWN:
        return 0;

    case WM_LBUTTONDOWN: {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK ChildProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    static HBITMAP g_hBmp = NULL;
    switch (message) {
        

    case WM_USER + 1:
    {

        g_hBmp = (HBITMAP)lParam;
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        if (g_hBmp) {
            RECT rc;
            GetClientRect(hWnd, &rc);  // 차일드 윈도우 크기 가져오기

            BITMAP bm;
            GetObject(g_hBmp, sizeof(BITMAP), &bm);  // 비트맵 원본 크기

            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP old = (HBITMAP)SelectObject(memDC, g_hBmp);
            StretchBlt(hdc, 0, 0, rc.right, rc.bottom,   // 목적지: 윈도우 전체
                memDC, 0, 0, bm.bmWidth, bm.bmHeight,  // 원본 크기
                SRCCOPY);
            SelectObject(memDC, old);
            DeleteDC(memDC);
        }
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}