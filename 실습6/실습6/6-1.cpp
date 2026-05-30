#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <cstdlib>
#include "resource.h"

enum ShapeType { SHAPE_RECT, SHAPE_CIRCLE };
enum SizeType  { SIZE_SMALL, SIZE_MEDIUM, SIZE_LARGE };

static ShapeType g_shape = SHAPE_RECT;
static SizeType  g_size  = SIZE_SMALL;
static bool g_gridOn     = false;
static bool g_colorRed   = false;
static bool g_colorGreen = false;
static bool g_colorBlue  = false;

static int  g_cx = 0, g_cy = 0;
static bool g_placed   = false;
static bool g_drawMode = false;
static bool g_dragging = false;

static std::vector<POINT> g_path;
static bool g_moving      = false;
static int  g_pathIndex   = 0;
static int  g_timerInterval = 30;

static int g_drawAreaBottom = 0;

static int GetShapeRadius()
{
    switch (g_size) {
    case SIZE_SMALL:  return 15;
    case SIZE_MEDIUM: return 30;
    case SIZE_LARGE:  return 50;
    }
    return 15;
}

static COLORREF GetFillColor()
{
    if (!g_colorRed && !g_colorGreen && !g_colorBlue)
        return RGB(255, 255, 255);
    return RGB(g_colorRed ? 255 : 0,
               g_colorGreen ? 255 : 0,
               g_colorBlue  ? 255 : 0);
}

static void DrawScene(HDC hdc, RECT rc)
{
    // background
    FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

    // grid
    if (g_gridOn) {
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
        HPEN old = (HPEN)SelectObject(hdc, pen);
        for (int x = 0; x < rc.right; x += 30) {
            MoveToEx(hdc, x, 0, NULL); LineTo(hdc, x, rc.bottom);
        }
        for (int y = 0; y < rc.bottom; y += 30) {
            MoveToEx(hdc, 0, y, NULL); LineTo(hdc, rc.right, y);
        }
        SelectObject(hdc, old);
        DeleteObject(pen);
    }

    // path
    if (g_path.size() >= 2) {
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HPEN old = (HPEN)SelectObject(hdc, pen);
        MoveToEx(hdc, g_path[0].x, g_path[0].y, NULL);
        for (size_t i = 1; i < g_path.size(); i++)
            LineTo(hdc, g_path[i].x, g_path[i].y);
        SelectObject(hdc, old);
        DeleteObject(pen);
    }

    // shape
    if (g_placed) {
        int r = GetShapeRadius();
        HBRUSH brush = CreateSolidBrush(GetFillColor());
        HPEN   pen   = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
        HBRUSH oldB  = (HBRUSH)SelectObject(hdc, brush);
        HPEN   oldP  = (HPEN)SelectObject(hdc, pen);

        if (g_shape == SHAPE_RECT)
            Rectangle(hdc, g_cx - r, g_cy - r, g_cx + r, g_cy + r);
        else
            Ellipse(hdc, g_cx - r, g_cy - r, g_cx + r, g_cy + r);

        SelectObject(hdc, oldB);
        SelectObject(hdc, oldP);
        DeleteObject(brush);
        DeleteObject(pen);
    }
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
    case WM_INITDIALOG:
    {
        CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
        CheckRadioButton(hDlg, IDC_RADIO3, IDC_RADIO5, IDC_RADIO3);
        CheckRadioButton(hDlg, IDC_RADIO6, IDC_RADIO7, IDC_RADIO7);

        // drawing area = client area above the radio buttons
        RECT rcCtrl;
        HWND hCtrl = GetDlgItem(hDlg, IDC_RADIO1);
        GetWindowRect(hCtrl, &rcCtrl);
        POINT pt = { rcCtrl.left, rcCtrl.top };
        ScreenToClient(hDlg, &pt);
        g_drawAreaBottom = pt.y - 12;
        return TRUE;
    }

    case WM_ERASEBKGND:
        return TRUE; // 깜빡임 방지: 기본 배경 지우기 차단

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hDlg, &ps);
        RECT rc;
        GetClientRect(hDlg, &rc);
        rc.bottom = g_drawAreaBottom;

        // 더블 버퍼링: 메모리 DC에 그린 뒤 한 번에 복사
        HDC     memDC  = CreateCompatibleDC(hdc);
        HBITMAP memBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

        DrawScene(memDC, rc);
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBmp);
        DeleteObject(memBmp);
        DeleteDC(memDC);
        EndPaint(hDlg, &ps);
        return TRUE;
    }

    case WM_LBUTTONDOWN:
    {
        if (!g_drawMode || !g_placed) break;
        int mx = GET_X_LPARAM(lParam);
        int my = GET_Y_LPARAM(lParam);
        int r  = GetShapeRadius();
        // must click near shape center
        if (abs(mx - g_cx) <= r + 5 && abs(my - g_cy) <= r + 5) {
            g_path.clear();
            g_path.push_back({ g_cx, g_cy });
            g_dragging = true;
            SetCapture(hDlg);
        }
        break;
    }

    case WM_MOUSEMOVE:
    {
        if (!g_dragging) break;
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (pt.y >= g_drawAreaBottom) break;
        g_path.push_back(pt);
        // draw incrementally
        HDC hdc = GetDC(hDlg);
        if (g_path.size() >= 2) {
            size_t n = g_path.size();
            MoveToEx(hdc, g_path[n-2].x, g_path[n-2].y, NULL);
            LineTo(hdc, pt.x, pt.y);
        }
        ReleaseDC(hDlg, hdc);
        break;
    }

    case WM_LBUTTONUP:
        g_dragging = false;
        ReleaseCapture();
        break;

    case WM_TIMER:
        if (wParam == 1 && g_moving) {
            if (g_pathIndex < (int)g_path.size()) {
                g_cx = g_path[g_pathIndex].x;
                g_cy = g_path[g_pathIndex].y;
                g_pathIndex++;
                RECT rc;
                GetClientRect(hDlg, &rc);
                rc.bottom = g_drawAreaBottom;
                InvalidateRect(hDlg, &rc, FALSE);
            } else {
                KillTimer(hDlg, 1);
                g_moving    = false;
                g_pathIndex = 0;
            }
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_RADIO1: g_shape = SHAPE_RECT;   break;
        case IDC_RADIO2: g_shape = SHAPE_CIRCLE; break;
        case IDC_RADIO3: g_size  = SIZE_SMALL;   break;
        case IDC_RADIO4: g_size  = SIZE_MEDIUM;  break;
        case IDC_RADIO5: g_size  = SIZE_LARGE;   break;
        case IDC_RADIO6:
            g_gridOn = true;
            InvalidateRect(hDlg, NULL, TRUE);
            break;
        case IDC_RADIO7:
            g_gridOn = false;
            InvalidateRect(hDlg, NULL, TRUE);
            break;
        case IDC_RADIO8:
            g_colorRed   = (IsDlgButtonChecked(hDlg, IDC_RADIO8)  == BST_CHECKED);
            break;
        case IDC_RADIO9:
            g_colorGreen = (IsDlgButtonChecked(hDlg, IDC_RADIO9)  == BST_CHECKED);
            break;
        case IDC_RADIO10:
            g_colorBlue  = (IsDlgButtonChecked(hDlg, IDC_RADIO10) == BST_CHECKED);
            break;

        case IDC_BUTTON2: // Draw
        {
            if (g_moving) { KillTimer(hDlg, 1); g_moving = false; }
            RECT rc;
            GetClientRect(hDlg, &rc);
            rc.bottom = g_drawAreaBottom;
            int r = GetShapeRadius();
            int maxX = rc.right  - r - 5;
            int maxY = rc.bottom - r - 5;
            int minX = r + 5, minY = r + 5;
            if (maxX <= minX) maxX = minX + 1;
            if (maxY <= minY) maxY = minY + 1;
            g_cx = minX + rand() % (maxX - minX);
            g_cy = minY + rand() % (maxY - minY);
            g_placed   = true;
            g_drawMode = true;
            g_path.clear();
            InvalidateRect(hDlg, &rc, TRUE);
            break;
        }

        case IDC_BUTTON3: // Move
            if (g_placed && g_path.size() >= 2 && !g_moving) {
                g_moving    = true;
                g_pathIndex = 0;
                g_cx = g_path[0].x;
                g_cy = g_path[0].y;
                SetTimer(hDlg, 1, g_timerInterval, NULL);
            }
            break;

        case IDC_BUTTON4: // Speed+
            if (g_timerInterval > 10) g_timerInterval -= 10;
            if (g_moving) { KillTimer(hDlg, 1); SetTimer(hDlg, 1, g_timerInterval, NULL); }
            break;

        case IDC_BUTTON5: // Speed-
            g_timerInterval += 10;
            if (g_moving) { KillTimer(hDlg, 1); SetTimer(hDlg, 1, g_timerInterval, NULL); }
            break;

        case IDC_BUTTON6: // Quit
            EndDialog(hDlg, 0);
            break;
        }
        return TRUE;

    case WM_CLOSE:
        EndDialog(hDlg, 0);
        return TRUE;
    }
    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    srand((unsigned)GetTickCount());
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
    return 0;
}
