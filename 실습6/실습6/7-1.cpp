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

#define TIMER_AUTO      1    // 이미지 자동 전환 간격 (1초)
#define TIMER_ANIM      2    // 애니메이션 프레임 (16ms ≈ 60fps)
#define ANIM_SPEED      22   // 한 프레임당 이동 픽셀

#define WM_NAV_LEFT     (WM_USER + 10)
#define WM_NAV_RIGHT    (WM_USER + 11)

// ── 연결된 이미지 시퀀스 ─────────────────────────────────────────────
HBITMAP g_images[10] = {};
int g_imageCount  = 0;
int g_currentIdx  = 0;

// ── 슬라이드 애니메이션 상태 ──────────────────────────────────────────
bool g_animating  = false;
int  g_animOffset = 0;     // 현재까지 이동한 픽셀
int  g_animTarget = 0;     // 애니메이션 완료 후 표시할 인덱스
int  g_animDir    = 1;     // +1: 좌로 밀기(다음), -1: 우로 밀기(이전)
bool g_autoMove   = false; // Move 버튼으로 자동 진행 중인지

HWND g_hChild = NULL;
HWND g_hEdit  = NULL;
HWND g_hList  = NULL;

LRESULT CALLBACK ChildProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

// ── 에디트 박스 갱신 ─────────────────────────────────────────────────
void UpdateEditBox() {
    if (!g_hEdit) return;
    WCHAR buf[32] = {};
    if (g_imageCount > 0)
        wsprintf(buf, L"%d / %d", g_currentIdx + 1, g_imageCount);
    SetWindowText(g_hEdit, buf);
}

// ── 슬라이드 시작 (애니메이션 없이 이미 진행 중이면 무시) ──────────────
void StartSlide(HWND hWnd, int targetIdx, int dir) {
    if (g_animating)      return;
    if (g_imageCount < 2) return;
    g_animTarget  = targetIdx;
    g_animDir     = dir;
    g_animOffset  = 0;
    g_animating   = true;
    SetTimer(hWnd, TIMER_ANIM, 16, NULL);
    InvalidateRect(g_hChild, NULL, FALSE);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    g_hInst = hInstance;

    WNDCLASSEX wc = { sizeof(wc) };
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

    HWND hWnd = CreateWindow(L"ParentClass", L"실습 7-1",
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

// ── 메인 윈도우 프로시저 ─────────────────────────────────────────────
LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        g_hChild = CreateWindow(L"ChildClass", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            20, 20, 620, 520,
            hWnd, NULL, g_hInst, NULL);

        g_hList = CreateWindow(L"listbox", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY |
            LBS_OWNERDRAWFIXED | LBS_HASSTRINGS,
            660, 20, 290, 260,
            hWnd, (HMENU)IDC_LIST, g_hInst, NULL);

        g_hEdit = CreateWindow(L"edit", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_CENTER,
            660, 300, 290, 28,
            hWnd, (HMENU)IDC_EDIT, g_hInst, NULL);

        CreateWindow(L"button", L"Select",
            WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 660,345,290,35,
            hWnd,(HMENU)IDC_BTN_SELECT,g_hInst,NULL);
        CreateWindow(L"button", L"Done",
            WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 660,390,290,35,
            hWnd,(HMENU)IDC_BTN_DONE,g_hInst,NULL);
        CreateWindow(L"button", L"Move",
            WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 660,435,290,35,
            hWnd,(HMENU)IDC_BTN_MOVE,g_hInst,NULL);
        CreateWindow(L"button", L"Stop",
            WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 660,480,290,35,
            hWnd,(HMENU)IDC_BTN_STOP,g_hInst,NULL);

        // 리스트에 비트맵 로드
        HBITMAP hBmp; int idx;
        hBmp = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
        idx = (int)SendMessage(g_hList,LB_ADDSTRING,0,(LPARAM)L"Image 01");
        SendMessage(g_hList,LB_SETITEMDATA,idx,(LPARAM)hBmp);
        hBmp = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
        idx = (int)SendMessage(g_hList,LB_ADDSTRING,0,(LPARAM)L"Image 02");
        SendMessage(g_hList,LB_SETITEMDATA,idx,(LPARAM)hBmp);
        hBmp = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
        idx = (int)SendMessage(g_hList,LB_ADDSTRING,0,(LPARAM)L"Image 03");
        SendMessage(g_hList,LB_SETITEMDATA,idx,(LPARAM)hBmp);
        return 0;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BTN_SELECT:
        {
            int sel = (int)SendMessage(g_hList,LB_GETCURSEL,0,0);
            if (sel == LB_ERR) break;
            if (g_imageCount >= 10) {
                MessageBox(hWnd,L"최대 10개까지 선택 가능합니다.",L"알림",MB_OK);
                break;
            }
            HBITMAP hBmp = (HBITMAP)SendMessage(g_hList,LB_GETITEMDATA,sel,0);
            g_images[g_imageCount++] = hBmp;
            g_currentIdx = g_imageCount - 1;
            UpdateEditBox();
            InvalidateRect(g_hChild, NULL, FALSE);
            break;
        }
        case IDC_BTN_DONE:
        {
            KillTimer(hWnd,TIMER_AUTO);
            KillTimer(hWnd,TIMER_ANIM);
            g_autoMove  = false;
            g_animating = false;
            WCHAR msg[64];
            wsprintf(msg, L"총 %d개의 이미지 연결이 완성되었습니다.", g_imageCount);
            MessageBox(hWnd, msg, L"완성", MB_OK);
            break;
        }
        case IDC_BTN_MOVE:
            if (g_imageCount > 1) {
                g_autoMove = true;
                // 즉시 첫 슬라이드 시작, 이후 완료 때마다 TIMER_AUTO가 재발동
                StartSlide(hWnd,
                    (g_currentIdx + 1) % g_imageCount, +1);
            }
            break;
        case IDC_BTN_STOP:
            g_autoMove = false;
            KillTimer(hWnd, TIMER_AUTO);
            break;
        }
        return 0;

    // 차일드 화살표 버튼 → 수동 슬라이드
    case WM_NAV_LEFT:
        if (g_imageCount > 0) {
            int prev = (g_currentIdx - 1 + g_imageCount) % g_imageCount;
            StartSlide(hWnd, prev, -1);
        }
        return 0;
    case WM_NAV_RIGHT:
        if (g_imageCount > 0) {
            int next = (g_currentIdx + 1) % g_imageCount;
            StartSlide(hWnd, next, +1);
        }
        return 0;

    case WM_TIMER:
    {
        if (wParam == TIMER_AUTO) {
            // 자동 전환: 애니메이션이 끝난 후 1초 대기 후 다음 슬라이드
            KillTimer(hWnd, TIMER_AUTO);
            StartSlide(hWnd,
                (g_currentIdx + 1) % g_imageCount, +1);
        }
        else if (wParam == TIMER_ANIM) {
            // 애니메이션 프레임 진행
            RECT rc; GetClientRect(g_hChild, &rc);
            int childW = rc.right;

            g_animOffset += ANIM_SPEED;

            if (g_animOffset >= childW) {
                // 애니메이션 완료
                g_animOffset  = 0;
                g_animating   = false;
                g_currentIdx  = g_animTarget;
                KillTimer(hWnd, TIMER_ANIM);
                UpdateEditBox();
                InvalidateRect(g_hChild, NULL, FALSE);

                // 자동 모드면 1초 후 다음 슬라이드 예약
                if (g_autoMove && g_imageCount > 1)
                    SetTimer(hWnd, TIMER_AUTO, 1000, NULL);
            } else {
                InvalidateRect(g_hChild, NULL, FALSE);
            }
        }
        return 0;
    }

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

        HBITMAP hBmp = (HBITMAP)SendMessage(g_hList,LB_GETITEMDATA,dis->itemID,0);
        if (hBmp) {
            HDC memDC = CreateCompatibleDC(dis->hDC);
            HBITMAP old = (HBITMAP)SelectObject(memDC,hBmp);
            BitBlt(dis->hDC, dis->rcItem.left+2, dis->rcItem.top+2,
                   36, 36, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC,old); DeleteDC(memDC);
        }
        TCHAR buf[64];
        SendMessage(g_hList,LB_GETTEXT,dis->itemID,(LPARAM)buf);
        TextOut(dis->hDC, dis->rcItem.left+45, dis->rcItem.top+12,
                buf, lstrlen(buf));
        return TRUE;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hWnd,&ps);
        EndPaint(hWnd,&ps);
        return 0;
    }
    case WM_DESTROY:
        KillTimer(hWnd, TIMER_AUTO);
        KillTimer(hWnd, TIMER_ANIM);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// ── 비트맵 하나를 HDC에 StretchBlt로 그리는 헬퍼 ──────────────────────
static void DrawBitmapAt(HDC hdc, HBITMAP hBmp, int x, int y, int w, int h)
{
    if (!hBmp) return;
    BITMAP bm;
    GetObject(hBmp, sizeof(BITMAP), &bm);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP old = (HBITMAP)SelectObject(memDC, hBmp);
    StretchBlt(hdc, x, y, w, h, memDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    SelectObject(memDC, old);
    DeleteDC(memDC);
}

// ── 차일드 윈도우 프로시저 ────────────────────────────────────────────
LRESULT CALLBACK ChildProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    static HWND hBtnLeft  = NULL;
    static HWND hBtnRight = NULL;

    switch (message)
    {
    case WM_CREATE:
        hBtnLeft  = CreateWindow(L"button", L"\x2190",
            WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
            5, 5, 55, 55,
            hWnd, (HMENU)IDC_BTN_LEFT, g_hInst, NULL);
        hBtnRight = CreateWindow(L"button", L"\x2192",
            WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
            5, 5, 55, 55,
            hWnd, (HMENU)IDC_BTN_RIGHT, g_hInst, NULL);
        return 0;

    case WM_SIZE: {
        int w = LOWORD(lParam), h = HIWORD(lParam);
        if (hBtnLeft)
            SetWindowPos(hBtnLeft,  NULL, 5,      h/2-28, 55,55, SWP_NOZORDER);
        if (hBtnRight)
            SetWindowPos(hBtnRight, NULL, w - 60, h/2-28, 55,55, SWP_NOZORDER);
        return 0;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BTN_LEFT)
            SendMessage(GetParent(hWnd), WM_NAV_LEFT,  0, 0);
        else if (LOWORD(wParam) == IDC_BTN_RIGHT)
            SendMessage(GetParent(hWnd), WM_NAV_RIGHT, 0, 0);
        return 0;

    // 배경 지우기 억제 → 깜빡임 방지
    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc; GetClientRect(hWnd, &rc);
        int W = rc.right, H = rc.bottom;

        if (g_animating && g_imageCount >= 1) {
            HBITMAP curBmp = g_images[g_currentIdx];
            HBITMAP nxtBmp = g_images[g_animTarget];

            if (g_animDir == +1) {
                // 좌로 밀기: 현재 이미지가 왼쪽으로 빠지고, 다음이 오른쪽에서 진입
                DrawBitmapAt(hdc, curBmp, -g_animOffset,      0, W, H);
                DrawBitmapAt(hdc, nxtBmp,  W - g_animOffset,  0, W, H);
            } else {
                // 우로 밀기: 현재 이미지가 오른쪽으로 빠지고, 이전이 왼쪽에서 진입
                DrawBitmapAt(hdc, curBmp,  g_animOffset,      0, W, H);
                DrawBitmapAt(hdc, nxtBmp, -W + g_animOffset,  0, W, H);
            }
        } else {
            // 정지 상태 – 현재 이미지 표시
            if (g_imageCount > 0)
                DrawBitmapAt(hdc, g_images[g_currentIdx], 0, 0, W, H);
            else {
                // 아무것도 없으면 회색
                FillRect(hdc, &rc, (HBRUSH)GetStockObject(GRAY_BRUSH));
            }
        }

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}
