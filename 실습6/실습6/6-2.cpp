#include <windows.h>
#include <windowsx.h>
#include <vector>
#include <cmath>
#include "resource.h"

// 실습 6-2 : 컨트롤 사용하기
//   - 메인 윈도우 : 좌표축 + 곡선 그리기 + 애니메이션 (더블 버퍼링)
//   - 모델리스 대화상자(IDD_DIALOG2) : 라디오/체크/버튼 컨트롤

const double PI = 3.14159265358979;

enum CurveType { CURVE_SIN, CURVE_HALF, CURVE_SPRING, CURVE_STAIR };


// 변경
enum AnimMode { ANIM_NONE = 0, ANIM_MOVEX = 1, ANIM_MOVEY = 2, ANIM_CIRCLE = 4 };

HINSTANCE g_hInst;
HWND      g_hMain;
HWND      g_hDlg;

// --- 상태 ---
CurveType g_curve = CURVE_SIN;
bool g_cyan = false, g_magenta = false, g_yellow = false, g_invert = false;

AnimMode g_anim = ANIM_NONE;
int    g_offsetX = 0;       // Move_x 스크롤 오프셋
double g_phaseY  = 0.0;     // Move_y 위상
int    g_offsetY = 0;
int    g_circleIdx = 0;     // 원이동 경로 인덱스
int    g_loopLenX = 314;    // X 스크롤 반복 길이(곡선 주기)

int g_mouseX = 0, g_mouseY = 0;

std::vector<POINT> g_path;  // 원점(0,0) 기준 곡선 좌표

const int HALF_W = 1200;    // 곡선 생성 가로 반폭

// 선택된 체크박스로 곡선 색 합성 (CMY 감산 혼합 + 반전)
COLORREF GetCurveColor()
{
    int r = 255, g = 255, b = 255;
    if (!g_cyan && !g_magenta && !g_yellow) {
        r = g = b = 0;             // 아무것도 선택 안 하면 검정
    } else {
        if (g_cyan)    r = 0;      // cyan  : 빨강 성분 제거
        if (g_magenta) g = 0;      // magenta: 초록 성분 제거
        if (g_yellow)  b = 0;      // yellow : 파랑 성분 제거
    }
    if (g_invert) { r = 255 - r; g = 255 - g; b = 255 - b; }
    return RGB(r, g, b);
}

// 라디오 선택에 따라 곡선 좌표(원점 기준) 생성
void BuildCurve()
{
    g_path.clear();

    switch (g_curve)
    {
    case CURVE_SIN:
    {
        double A = 120, k = 0.02;          // 진폭, 각진동수
        for (int x = -HALF_W; x <= HALF_W; x++)
            g_path.push_back({ x, (int)(-A * sin(k * x)) });
        g_loopLenX = (int)(2 * PI / k);    // ≈ 314
        break;
    }
    case CURVE_HALF:
    {
        double R = 80;
        // 각 원을 위쪽 반원 → 아래쪽 반원 순서로 하나씩 이어서 경로 생성
        for (int cx = -HALF_W + (int)R; cx <= HALF_W; cx += (int)(2 * R))
        {
            // 위쪽 반원: PI → 2PI (sin < 0, 화면 위쪽)
            for (double t = PI; t <= 2 * PI + 0.05; t += 0.05)
                g_path.push_back({ cx + (int)(R * cos(t)), (int)(R * sin(t)) });
            // 아래쪽 반원: 2PI → 3PI (sin > 0, 화면 아래쪽)
            
        }
        for (int cx = -HALF_W + (int)R; cx <= HALF_W; cx += (int)(2 * R))
            for (double t = 2 * PI; t <= 3 * PI + 0.05; t += 0.05)
                g_path.push_back({ cx + (int)(R * cos(t)), (int)(R * sin(t)) });
        int sz = (int)g_path.size();
        for (int i = sz - 1; i >= 0; i--)
            g_path.push_back(g_path[i]);

        g_loopLenX = (int)(2 * R);
        break;
    }
    case CURVE_SPRING:
    {
        double R = 80, c = 25;             // 반지름, 라디안당 전진량 (R>c → 고리 겹침)
        for (double t = -HALF_W / c; t <= HALF_W / c; t += 0.05)
            g_path.push_back({ (int)(c * t + R * cos(t)), (int)(R * sin(t)) });
        g_loopLenX = (int)(2 * PI * c);    // ≈ 157
        break;
    }
    case CURVE_STAIR:
    {
        int w = 200, h = 50;
        int x = -HALF_W + 600, y = 0;
        // 가로 구간을 1픽셀씩 채움
        while (x < HALF_W) {
            for (int i = 0; i <= w; i++)
                g_path.push_back({ x + i, y });
            x += w;
            // 세로 구간을 1픽셀씩 채움
            for (int i = 1; i <= h; i++)
                g_path.push_back({ x, y - i });
            y -= h;
        }
        g_loopLenX = w;
        break;
    }
    }
    g_circleIdx = 0;
}

// 장면 그리기 (메모리 DC에 그림  더블 버퍼링)
void DrawScene(HDC hdc, RECT rc)
{
    int cx = rc.right / 2;
    int cy = rc.bottom / 2;

    FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

    // 좌표축
    HPEN axisPen = CreatePen(PS_SOLID, 1, RGB(120, 120, 120));
    HPEN oldPen  = (HPEN)SelectObject(hdc, axisPen);
    MoveToEx(hdc, 0, cy, NULL);        LineTo(hdc, rc.right, cy);   // X축
    MoveToEx(hdc, cx, 0, NULL);        LineTo(hdc, cx, rc.bottom);  // Y축
    SelectObject(hdc, oldPen);
    DeleteObject(axisPen);

    // 마우스 좌표 / 오프셋 표시
    SetBkMode(hdc, TRANSPARENT);
    TCHAR buf[128];
    wsprintf(buf, TEXT("mX : %d, mY : %d | %d"), g_mouseX, g_mouseY, g_offsetX);
    TextOut(hdc, 10, 10, buf, lstrlen(buf));

    if (g_path.empty()) return;

    int ox = cx + g_offsetX;
    int oy = cy + g_offsetY;

    // 곡선 그리기 (원점 기준 좌표를 화면 좌표로 평행이동)
    std::vector<POINT> pts(g_path.size());
    for (size_t i = 0; i < g_path.size(); i++) {
        pts[i].x = g_path[i].x + ox;
        pts[i].y = g_path[i].y + oy;
    }
    HPEN curvePen = CreatePen(PS_SOLID, 2, GetCurveColor());
    oldPen = (HPEN)SelectObject(hdc, curvePen);
    Polyline(hdc, pts.data(), (int)pts.size());
    SelectObject(hdc, oldPen);
    DeleteObject(curvePen);

    // 원이동 : 경로를 따라가는 원 + "It's moving" 문자
    if (g_anim & ANIM_CIRCLE) {
        int idx = g_circleIdx % (int)g_path.size();
        int px = g_path[idx].x + ox;
        int py = g_path[idx].y + oy;
        int r  = 14;

        HBRUSH br  = CreateSolidBrush(GetCurveColor());
        HPEN   pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
        HBRUSH ob  = (HBRUSH)SelectObject(hdc, br);
        HPEN   op  = (HPEN)SelectObject(hdc, pen);
        Ellipse(hdc, px - r, py - r, px + r, py + r);
        SelectObject(hdc, ob);
        SelectObject(hdc, op);
        DeleteObject(br);
        DeleteObject(pen);

        const TCHAR* msg = TEXT("It's moving");
        SetTextColor(hdc, RGB(0, 0, 0));
        TextOut(hdc, px - 30, py - r - 18, msg, lstrlen(msg));
    }
}

// 애니메이션 시작/정지
void StartAnim(AnimMode mode)
{
    g_anim = (AnimMode)(g_anim | mode);  // 기존 모드에 OR로 추가
    if (mode == ANIM_CIRCLE) g_circleIdx = 0;
    SetTimer(g_hMain, 1, 20, NULL);
}
void StopAnim()
{
    g_anim = ANIM_NONE;
    KillTimer(g_hMain, 1);
}
void ResetAll()
{
    StopAnim();
    g_offsetX = 0; g_offsetY = 0; g_phaseY = 0; g_circleIdx = 0;
    g_cyan = g_magenta = g_yellow = g_invert = false;
    g_curve = CURVE_SIN;
    if (g_hDlg) {
        CheckRadioButton(g_hDlg, IDC_RADIO_SIN, IDC_RADIO_STAIR, IDC_RADIO_SIN);
        CheckDlgButton(g_hDlg, IDC_CHK_CYAN,    BST_UNCHECKED);
        CheckDlgButton(g_hDlg, IDC_CHK_MAGENTA, BST_UNCHECKED);
        CheckDlgButton(g_hDlg, IDC_CHK_YELLOW,  BST_UNCHECKED);
        CheckDlgButton(g_hDlg, IDC_CHK_INVERT,  BST_UNCHECKED);
    }
    BuildCurve();
    InvalidateRect(g_hMain, NULL, FALSE);
}

// 대화상자 프로시저 (모델리스)
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
    case WM_INITDIALOG:
        CheckRadioButton(hDlg, IDC_RADIO_SIN, IDC_RADIO_STAIR, IDC_RADIO_SIN);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_RADIO_SIN:    g_curve = CURVE_SIN;    BuildCurve(); break;
        case IDC_RADIO_HALF:   g_curve = CURVE_HALF;   BuildCurve(); break;
        case IDC_RADIO_SPRING: g_curve = CURVE_SPRING; BuildCurve(); break;
        case IDC_RADIO_STAIR:  g_curve = CURVE_STAIR;  BuildCurve(); break;

        case IDC_CHK_CYAN:    g_cyan    = (IsDlgButtonChecked(hDlg, IDC_CHK_CYAN)    == BST_CHECKED); break;
        case IDC_CHK_MAGENTA: g_magenta = (IsDlgButtonChecked(hDlg, IDC_CHK_MAGENTA) == BST_CHECKED); break;
        case IDC_CHK_YELLOW:  g_yellow  = (IsDlgButtonChecked(hDlg, IDC_CHK_YELLOW)  == BST_CHECKED); break;
        case IDC_CHK_INVERT:  g_invert  = (IsDlgButtonChecked(hDlg, IDC_CHK_INVERT)  == BST_CHECKED); break;

        case IDC_BTN_MOVEX:  StartAnim(ANIM_MOVEX);  break;   // 좌우 이동
        case IDC_BTN_MOVEY:  StartAnim(ANIM_MOVEY);  break;   // 위아래 이동
        case IDC_BTN_STOP:   StopAnim();             break;   // 정지
        case IDC_BTN_RESET:                                   // 오프셋 리셋
            StopAnim();
            g_offsetX = g_offsetY = 0; g_phaseY = 0; g_circleIdx = 0;
            InvalidateRect(g_hMain, NULL, FALSE);
            break;
        case IDC_BTN_CIRCLE: StartAnim(ANIM_CIRCLE); break;   // 원이동

        case IDOK:           ResetAll();             return TRUE;   // 모든 값 리셋
        case IDCANCEL:       DestroyWindow(g_hMain); return TRUE;   // 종료
        }
        InvalidateRect(g_hMain, NULL, FALSE);
        return TRUE;
    }
    return FALSE;
}

// 메인 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        BuildCurve();
        return 0;

    case WM_MOUSEMOVE:
        g_mouseX = GET_X_LPARAM(lParam);
        g_mouseY = GET_Y_LPARAM(lParam);
        InvalidateRect(hWnd, NULL, FALSE);
        return 0;

    case WM_TIMER:
        if (wParam == 1) {
            if (g_anim & ANIM_MOVEX) {
                g_offsetX += 4;
                if (g_loopLenX > 0 && g_offsetX >= g_loopLenX) g_offsetX -= g_loopLenX;
            }
            if (g_anim & ANIM_MOVEY) {
                g_phaseY += 0.06;
                g_offsetY = (int)(100 * sin(g_phaseY));
            }
            if (g_anim & ANIM_CIRCLE) {
                g_circleIdx += 4;
                if (g_circleIdx >= (int)g_path.size()) g_circleIdx = 0;
            }
            InvalidateRect(hWnd, NULL, FALSE);
        }
        return 0;

    case WM_ERASEBKGND:
        return 1;   // 깜빡임 방지

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc; GetClientRect(hWnd, &rc);

        HDC     memDC  = CreateCompatibleDC(hdc);
        HBITMAP memBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

        DrawScene(memDC, rc);
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBmp);
        DeleteObject(memBmp);
        DeleteDC(memDC);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    g_hInst = hInstance;

    WNDCLASS wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = TEXT("Curve62");
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&wc);

    g_hMain = CreateWindow(TEXT("Curve62"), TEXT("Window Title Name"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 650,
        NULL, NULL, hInstance, NULL);
    ShowWindow(g_hMain, nCmdShow);
    UpdateWindow(g_hMain);

    // 모델리스 대화상자
    g_hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG2), g_hMain, DlgProc);
    ShowWindow(g_hDlg, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!IsDialogMessage(g_hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}
