#include <windows.h>
#include <windowsx.h>
#include "resource1.h"

HINSTANCE g_hInst;

#define IDC_BTN_LEFT    200
#define IDC_BTN_RIGHT   201
#define IDC_BTN_SELECT  100
#define IDC_BTN_DONE    101
#define IDC_BTN_MOVE    102
#define IDC_BTN_STOP    103
#define IDC_LIST        104
#define IDC_EDIT        105
#define TIMER_ID        1
#define WM_NAV_LEFT     (WM_USER + 10)
#define WM_NAV_RIGHT    (WM_USER + 11)

// Connected image sequence (max 10)
HBITMAP g_images[10] = {};
int g_imageCount = 0;
int g_currentIndex = 0;

HWND g_hChild = NULL;
HWND g_hEdit  = NULL;
HWND g_hList  = NULL;

LRESULT CALLBACK ChildProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void UpdateDisplay()
{
    if (g_hChild) {
        HBITMAP bmp = (g_imageCount > 0) ? g_images[g_currentIndex] : NULL;
        SendMessage(g_hChild, WM_USER + 1, 0, (LPARAM)bmp);
    }
    if (g_hEdit) {
        WCHAR buf[64] = {};
        if (g_imageCount > 0)
            wsprintf(buf, L"%d / %d", g_currentIndex + 1, g_imageCount);
        SetWindowText(g_hEdit, buf);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    g_hInst = hInstance;

    WNDCLASSEX wc = {};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"ParentClass";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    RegisterClassEx(&wc);

    wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wc.lpszClassName = L"ChildClass";
    wc.lpfnWndProc   = ChildProc;
    RegisterClassEx(&wc);

    HWND hWnd = CreateWindow(
        L"ParentClass", L"실습 7-1",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 680,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CREATE:
    {
        // Left side: child window for image display
        g_hChild = CreateWindow(L"ChildClass", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            20, 20, 620, 520,
            hWnd, NULL, g_hInst, NULL);

        // Right side: list box
        g_hList = CreateWindow(L"listbox", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY |
            LBS_OWNERDRAWFIXED | LBS_HASSTRINGS,
            660, 20, 290, 260,
            hWnd, (HMENU)IDC_LIST, g_hInst, NULL);

        // Edit box: shows current image order
        g_hEdit = CreateWindow(L"edit", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_CENTER,
            660, 300, 290, 28,
            hWnd, (HMENU)IDC_EDIT, g_hInst, NULL);

        // Control buttons
        CreateWindow(L"button", L"Select",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            660, 345, 290, 35, hWnd, (HMENU)IDC_BTN_SELECT, g_hInst, NULL);
        CreateWindow(L"button", L"Done",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            660, 390, 290, 35, hWnd, (HMENU)IDC_BTN_DONE, g_hInst, NULL);
        CreateWindow(L"button", L"Move",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            660, 435, 290, 35, hWnd, (HMENU)IDC_BTN_MOVE, g_hInst, NULL);
        CreateWindow(L"button", L"Stop",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            660, 480, 290, 35, hWnd, (HMENU)IDC_BTN_STOP, g_hInst, NULL);

        // Load bitmaps into list
        HBITMAP hBmp;
        int idx;

        hBmp = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
        idx  = (int)SendMessage(g_hList, LB_ADDSTRING, 0, (LPARAM)L"Image 01");
        SendMessage(g_hList, LB_SETITEMDATA, idx, (LPARAM)hBmp);

        hBmp = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
        idx  = (int)SendMessage(g_hList, LB_ADDSTRING, 0, (LPARAM)L"Image 02");
        SendMessage(g_hList, LB_SETITEMDATA, idx, (LPARAM)hBmp);

        hBmp = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
        idx  = (int)SendMessage(g_hList, LB_ADDSTRING, 0, (LPARAM)L"Image 03");
        SendMessage(g_hList, LB_SETITEMDATA, idx, (LPARAM)hBmp);

        return 0;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_BTN_SELECT:
        {
            int sel = (int)SendMessage(g_hList, LB_GETCURSEL, 0, 0);
            if (sel == LB_ERR) break;
            if (g_imageCount >= 10) {
                MessageBox(hWnd, L"최대 10개까지만 선택 가능합니다.", L"알림", MB_OK);
                break;
            }
            HBITMAP hBmp = (HBITMAP)SendMessage(g_hList, LB_GETITEMDATA, sel, 0);
            g_images[g_imageCount++] = hBmp;
            g_currentIndex = g_imageCount - 1;
            UpdateDisplay();
            break;
        }
        case IDC_BTN_DONE:
        {
            KillTimer(hWnd, TIMER_ID);
            WCHAR msg[64];
            wsprintf(msg, L"총 %d개의 이미지 연결이 완성되었습니다.", g_imageCount);
            MessageBox(hWnd, msg, L"완성", MB_OK);
            break;
        }
        case IDC_BTN_MOVE:
            if (g_imageCount > 1)
                SetTimer(hWnd, TIMER_ID, 1000, NULL);
            break;
        case IDC_BTN_STOP:
            KillTimer(hWnd, TIMER_ID);
            break;
        }
        return 0;

    // Left arrow from child window
    case WM_NAV_LEFT:
        if (g_imageCount > 0) {
            g_currentIndex = (g_currentIndex - 1 + g_imageCount) % g_imageCount;
            UpdateDisplay();
        }
        return 0;

    // Right arrow from child window
    case WM_NAV_RIGHT:
        if (g_imageCount > 0) {
            g_currentIndex = (g_currentIndex + 1) % g_imageCount;
            UpdateDisplay();
        }
        return 0;

    case WM_TIMER:
        if (wParam == TIMER_ID && g_imageCount > 0) {
            // Move left: advance to next image
            g_currentIndex = (g_currentIndex + 1) % g_imageCount;
            UpdateDisplay();
        }
        return 0;

    case WM_MEASUREITEM: {
        MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)lParam;
        mis->itemHeight = 40;
        return TRUE;
    }

    case WM_DRAWITEM: {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
        if (dis->CtlID != IDC_LIST) break;

        FillRect(dis->hDC, &dis->rcItem,
            (dis->itemState & ODS_SELECTED) ?
            (HBRUSH)GetStockObject(LTGRAY_BRUSH) :
            (HBRUSH)GetStockObject(WHITE_BRUSH));

        HBITMAP hBmp = (HBITMAP)SendMessage(g_hList, LB_GETITEMDATA, dis->itemID, 0);
        if (hBmp) {
            HDC memDC = CreateCompatibleDC(dis->hDC);
            HBITMAP old = (HBITMAP)SelectObject(memDC, hBmp);
            BitBlt(dis->hDC, dis->rcItem.left + 2, dis->rcItem.top + 2,
                   36, 36, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, old);
            DeleteDC(memDC);
        }

        TCHAR buf[64];
        SendMessage(g_hList, LB_GETTEXT, dis->itemID, (LPARAM)buf);
        TextOut(dis->hDC, dis->rcItem.left + 45, dis->rcItem.top + 12,
                buf, lstrlen(buf));
        return TRUE;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hWnd, TIMER_ID);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK ChildProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    static HBITMAP s_hBmp = NULL;
    static HWND hBtnLeft  = NULL;
    static HWND hBtnRight = NULL;

    switch (message) {
    case WM_CREATE:
    {
        // Arrow buttons placed on left/right edges; repositioned in WM_SIZE
        hBtnLeft = CreateWindow(L"button", L"\x2190",   // ←
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            5, 5, 55, 55,
            hWnd, (HMENU)IDC_BTN_LEFT, g_hInst, NULL);
        hBtnRight = CreateWindow(L"button", L"\x2192",  // →
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            5, 5, 55, 55,
            hWnd, (HMENU)IDC_BTN_RIGHT, g_hInst, NULL);
        return 0;
    }

    case WM_SIZE:
    {
        int w = LOWORD(lParam);
        int h = HIWORD(lParam);
        if (hBtnLeft)
            SetWindowPos(hBtnLeft,  NULL, 5,      h/2 - 28, 55, 55, SWP_NOZORDER);
        if (hBtnRight)
            SetWindowPos(hBtnRight, NULL, w - 60, h/2 - 28, 55, 55, SWP_NOZORDER);
        return 0;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BTN_LEFT)
            SendMessage(GetParent(hWnd), WM_NAV_LEFT, 0, 0);
        else if (LOWORD(wParam) == IDC_BTN_RIGHT)
            SendMessage(GetParent(hWnd), WM_NAV_RIGHT, 0, 0);
        return 0;

    case WM_USER + 1:
        s_hBmp = (HBITMAP)lParam;
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        if (s_hBmp) {
            RECT rc;
            GetClientRect(hWnd, &rc);
            BITMAP bm;
            GetObject(s_hBmp, sizeof(BITMAP), &bm);
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP old = (HBITMAP)SelectObject(memDC, s_hBmp);
            StretchBlt(hdc, 0, 0, rc.right, rc.bottom,
                       memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
            SelectObject(memDC, old);
            DeleteDC(memDC);
        }
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}
