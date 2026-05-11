
#include <windows.h>
#include <windowsx.h>  
#include <cstdlib>
#include <ctime>
#include "resource.h"

//상수
#define MOVE_STEP   10
#define EDGE_TH     8
#define TID_BOUNCE  1
#define TID_SIZE    2
#define MAX_PP      5
#define MAX_DP      10

// 전역 변수 
HINSTANCE g_hInst;
int g_W = 1, g_H = 1;

// 비트맵 2장
HBITMAP g_hBmp[2];
BITMAP  g_bI[2];
int     g_img = 0;          // 현재 표시 이미지 (0 or 1)

// 돋보기
bool  g_magOn = false;
RECT  g_mag = {};
float g_scale = 1.0f;       // 내부 이미지 확대 배율
bool  g_inv = false;      // 색 반전 여부

// 마우스 드래그 상태
POINT g_ptDn = {};
RECT  g_magDn = {};
bool  g_lDn = false;
enum DragMode { DM_NONE, DM_CREATE, DM_MOVE, DM_RESIZE };
DragMode g_dm = DM_NONE;
int   g_re = 0;          // 리사이즈 엣지 비트 (1=R, 2=B, 4=L, 8=T)

// 복사/붙여넣기
bool g_copied = false;

struct PI {                  // Paste Item
    int   img;
    float sx, sy, sw, sh;   // 비트맵 소스 영역
    int   dx, dy, dw, dh;   // 화면 목적 영역
};
PI   g_pp[MAX_PP]; int g_pc = 0;   // p 키 붙여넣기
PI   g_dp[MAX_DP]; int g_dc = 0;   // 더블클릭 붙여넣기
bool g_fh = false, g_fv = false;   // p 붙여넣기 반전 상태

bool g_full = false;         // 전체 붙여넣기 모드

// 바운스 애니메이션
bool g_bnc = false;
int  g_bvx = 5, g_bvy = 4;

// 크기 애니메이션
bool  g_szA = false;
float g_szT = 1.0f;
float g_szSt = 0.03f;
RECT  g_magOrig = {};






// 돋보기 영역에 해당하는 비트맵 소스 rect 계산
struct SR { float x, y, w, h; };

SR CalcSR(RECT mag, float sc, int idx) {
    float bw = (float)g_bI[idx].bmWidth;
    float bh = (float)g_bI[idx].bmHeight;
    float mw = (float)(mag.right - mag.left);
    float mh = (float)(mag.bottom - mag.top);
    if (mw <= 0 || mh <= 0 || g_W <= 0 || g_H <= 0) return {};

    float cx = (mag.left + mag.right) / 2.0f;
    float cy = (mag.top + mag.bottom) / 2.0f;
    float bcx = cx * bw / g_W;
    float bcy = cy * bh / g_H;

    float sw = mw * bw / (sc * g_W);
    float sh = mh * bh / (sc * g_H);
    float sx = bcx - sw * 0.5f;
    float sy = bcy - sh * 0.5f;

    if (sx < 0) sx = 0;
    if (sy < 0) sy = 0;
    if (sw > bw) sw = bw;
    if (sh > bh) sh = bh;
    if (sx + sw > bw) sx = bw - sw;
    if (sy + sh > bh) sy = bh - sh;
    if (sx < 0) sx = 0;
    if (sy < 0) sy = 0;
    return { sx, sy, sw, sh };
}

// 비트맵 영역 → 화면 출력 (flip / invert 옵션 포함)
void Blt(HDC hdc, int idx,
    float sx, float sy, float sw, float sh,
    int dx, int dy, int dw, int dh,
    bool fh, bool fv, bool inv)
{
    if (sw <= 0 || sh <= 0 || dw <= 0 || dh <= 0) return;
    HDC mc = CreateCompatibleDC(hdc);
    HBITMAP ob = (HBITMAP)SelectObject(mc, g_hBmp[idx]);

    int ix = (int)sx, iy = (int)sy, iw = (int)sw, ih = (int)sh;
    if (fh) { ix += iw; iw = -iw; }   // 좌우 반전: 시작점을 오른쪽으로, 폭 음수
    if (fv) { iy += ih; ih = -ih; }   // 상하 반전: 시작점을 아래로, 높이 음수

    StretchBlt(hdc, dx, dy, dw, dh,
        mc, ix, iy, iw, ih,
        inv ? NOTSRCCOPY : SRCCOPY);

    SelectObject(mc, ob);
    DeleteDC(mc);
}

// 이미지/돋보기 이동 시 붙여넣기 항목들의 소스 갱신
void UpdatePI() {
    if (!g_magOn) return;
    SR sr = CalcSR(g_mag, g_scale, g_img);
    for (int i = 0; i < g_pc; i++) {
        g_pp[i].img = g_img; g_pp[i].sx = sr.x; g_pp[i].sy = sr.y;
        g_pp[i].sw = sr.w;   g_pp[i].sh = sr.h;
    }
    for (int i = 0; i < g_dc; i++) {
        g_dp[i].img = g_img; g_dp[i].sx = sr.x; g_dp[i].sy = sr.y;
        g_dp[i].sw = sr.w;   g_dp[i].sh = sr.h;
    }
}

// 테두리 사각형 그리기 도우미
void DrawBorder(HDC hdc, RECT r, COLORREF clr, int thick = 1) {
    HPEN   pen = CreatePen(PS_SOLID, thick, clr);
    HPEN   op = (HPEN)SelectObject(hdc, pen);
    HBRUSH ob = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, r.left, r.top, r.right, r.bottom);
    SelectObject(hdc, op);
    SelectObject(hdc, ob);
    DeleteObject(pen);
}


//  WinMain

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hi, HINSTANCE, LPSTR, int ns) {
    srand((unsigned)time(NULL));
    g_hInst = hi;

    WNDCLASSEX wc = { sizeof(wc) };
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hi;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = TEXT("Mag53");
    RegisterClassEx(&wc);

    HWND hw = CreateWindowEx(0, TEXT("Mag53"), TEXT("실습 5-3 돋보기"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 700,
        NULL, NULL, hi, NULL);
    ShowWindow(hw, ns);
    UpdateWindow(hw);

    MSG m;
    while (GetMessage(&m, NULL, 0, 0)) {
        TranslateMessage(&m);
        DispatchMessage(&m);
    }
    return (int)m.wParam;
}


//  WndProc

LRESULT CALLBACK WndProc(HWND hw, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {

        // 초기화
    case WM_CREATE:
        g_hBmp[0] = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
        g_hBmp[1] = (HBITMAP)LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
        GetObject(g_hBmp[0], sizeof(BITMAP), &g_bI[0]);
        GetObject(g_hBmp[1], sizeof(BITMAP), &g_bI[1]);
        return 0;

    case WM_SIZE:
        g_W = max(1, (int)LOWORD(lp));
        g_H = max(1, (int)HIWORD(lp));
        return 0;

        // 그리기
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hw, &ps);

        // [1] 배경 (전체 붙여넣기 모드면 돋보기 내용으로 대체)
        if (g_full && g_magOn) {
            SR sr = CalcSR(g_mag, g_scale, g_img);
            Blt(hdc, g_img, sr.x, sr.y, sr.w, sr.h,
                0, 0, g_W, g_H, false, false, g_inv);
        }
        else {
            HDC mc = CreateCompatibleDC(hdc);
            HBITMAP ob = (HBITMAP)SelectObject(mc, g_hBmp[g_img]);
            StretchBlt(hdc, 0, 0, g_W, g_H,
                mc, 0, 0, g_bI[g_img].bmWidth, g_bI[g_img].bmHeight, SRCCOPY);
            SelectObject(mc, ob);
            DeleteDC(mc);

            // [2] p 키 붙여넣기 (초록 테두리)
            for (int i = 0; i < g_pc; i++) {
                PI& p = g_pp[i];
                Blt(hdc, p.img, p.sx, p.sy, p.sw, p.sh,
                    p.dx, p.dy, p.dw, p.dh, g_fh, g_fv, false);
                RECT br = { p.dx, p.dy, p.dx + p.dw, p.dy + p.dh };
                DrawBorder(hdc, br, RGB(0, 180, 0));
            }

            // [3] 더블클릭 붙여넣기 (파란 테두리)
            for (int i = 0; i < g_dc; i++) {
                PI& p = g_dp[i];
                Blt(hdc, p.img, p.sx, p.sy, p.sw, p.sh,
                    p.dx, p.dy, p.dw, p.dh, false, false, false);
                RECT br = { p.dx, p.dy, p.dx + p.dw, p.dy + p.dh };
                DrawBorder(hdc, br, RGB(0, 0, 200));
            }

            // [4] 돋보기 내용 + 빨간 테두리
            if (g_magOn && g_dm != DM_CREATE) {
                int mw = g_mag.right - g_mag.left;
                int mh = g_mag.bottom - g_mag.top;
                if (mw > 0 && mh > 0) {
                    // 돋보기 영역으로 클리핑
                    HRGN rg = CreateRectRgn(g_mag.left, g_mag.top,
                        g_mag.right, g_mag.bottom);
                    SelectClipRgn(hdc, rg);
                    DeleteObject(rg);

                    SR sr = CalcSR(g_mag, g_scale, g_img);
                    Blt(hdc, g_img, sr.x, sr.y, sr.w, sr.h,
                        g_mag.left, g_mag.top, mw, mh, false, false, g_inv);

                    SelectClipRgn(hdc, NULL);
                    DrawBorder(hdc, g_mag, RGB(220, 30, 30), 2);
                }
            }

            // [5] 고무줄 사각형 (생성 중)
            if (g_dm == DM_CREATE && g_lDn && !IsRectEmpty(&g_mag))
                DrawFocusRect(hdc, &g_mag);
        }

        EndPaint(hw, &ps);
        return 0;
    }

                 // ── 키보드
    case WM_KEYDOWN: {
        bool rd = false;
        switch (wp) {

        case '1': g_img = 0; UpdatePI(); rd = true; break;
        case '2': g_img = 1; UpdatePI(); rd = true; break;

        case 'E':   // 돋보기 내부 확대 (최대 4배 → 원래로)
            if (g_magOn) {
                g_scale += 0.2f;
                if (g_scale > 4.0f) g_scale = 1.0f;
                rd = true;
            }
            break;

        case 'S':   // 돋보기 내부 축소 (최소 0.25배 → 원래로)
            if (g_magOn) {
                g_scale -= 0.2f;
                if (g_scale < 0.25f) g_scale = 1.0f;
                rd = true;
            }
            break;

        case 'B':   // 제자리 (배율 1.0)
            if (g_magOn) { g_scale = 1.0f; rd = true; }
            break;

        case 'C':   // 복사
            if (g_magOn) g_copied = true;
            break;

        case 'P':   // 붙여넣기 ½크기, 최대 5개
            if (g_magOn && g_copied && g_pc < MAX_PP) {
                SR sr = CalcSR(g_mag, g_scale, g_img);
                int mw = g_mag.right - g_mag.left;
                int mh = g_mag.bottom - g_mag.top;
                int dw = max(4, mw / 2);
                int dh = max(4, mh / 2);
                int dx = (g_W > dw) ? rand() % (g_W - dw) : 0;
                int dy = (g_H > dh) ? rand() % (g_H - dh) : 0;
                g_pp[g_pc++] = { g_img, sr.x, sr.y, sr.w, sr.h, dx, dy, dw, dh };
                rd = true;
            }
            break;

        case 'F':   // 전체 붙여넣기 토글
            if (g_magOn) { g_full = !g_full; rd = true; }
            break;

        case VK_LEFT:
            if (g_magOn) { OffsetRect(&g_mag, -MOVE_STEP, 0); UpdatePI(); rd = true; }
            break;
        case VK_RIGHT:
            if (g_magOn) { OffsetRect(&g_mag, MOVE_STEP, 0); UpdatePI(); rd = true; }
            break;
        case VK_UP:
            if (g_magOn) { OffsetRect(&g_mag, 0, -MOVE_STEP); UpdatePI(); rd = true; }
            break;
        case VK_DOWN:
            if (g_magOn) { OffsetRect(&g_mag, 0, MOVE_STEP); UpdatePI(); rd = true; }
            break;

        case 'H': g_fh = !g_fh; rd = true; break;  // p붙여넣기 좌우 반전
        case 'V': g_fv = !g_fv; rd = true; break;  // p붙여넣기 상하 반전

        case 'M':   // 바운스 애니메이션 토글
            if (g_magOn) {
                g_bnc = !g_bnc;
                if (g_bnc) SetTimer(hw, TID_BOUNCE, 20, NULL);
                else { KillTimer(hw, TID_BOUNCE); rd = true; }
            }
            break;

        case 'N':   // 크기 애니메이션 토글
            if (g_magOn) {
                if (!g_szA) {
                    g_szA = true;
                    g_szT = 1.0f; g_szSt = 0.03f;
                    g_magOrig = g_mag;
                    SetTimer(hw, TID_SIZE, 20, NULL);
                }
                else {
                    g_szA = false;
                    KillTimer(hw, TID_SIZE);
                    g_mag = g_magOrig;
                    rd = true;
                }
            }
            break;

        case 'I':   // 색 반전 토글 (래스터연산 NOTSRCCOPY)
            if (g_magOn) { g_inv = !g_inv; rd = true; }
            break;

        case 'R':   // 전체 리셋
            g_magOn = false; g_scale = 1.0f; g_inv = false;
            g_full = false; g_copied = false;
            g_pc = 0; g_dc = 0;
            g_fh = false; g_fv = false;
            g_bnc = false; KillTimer(hw, TID_BOUNCE);
            g_szA = false; KillTimer(hw, TID_SIZE);
            g_dm = DM_NONE; g_lDn = false;
            rd = true;
            break;

        case 'Q':
            DestroyWindow(hw);
            break;
        }
        if (rd) InvalidateRect(hw, NULL, TRUE);
        return 0;
    }

                   // ── 마우스
    case WM_LBUTTONDOWN: {
        POINT pt = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
        g_ptDn = pt; g_lDn = true;
        SetCapture(hw);

        if (g_magOn) {
            // 엣지 감지 (비트마스크: 1=우, 2=하, 4=좌, 8=상)
            int re = 0;
            if (abs(pt.x - g_mag.right) < EDGE_TH && pt.y > g_mag.top - EDGE_TH && pt.y < g_mag.bottom + EDGE_TH) re |= 1;
            if (abs(pt.y - g_mag.bottom) < EDGE_TH && pt.x > g_mag.left - EDGE_TH && pt.x < g_mag.right + EDGE_TH)  re |= 2;
            if (abs(pt.x - g_mag.left) < EDGE_TH && pt.y > g_mag.top - EDGE_TH && pt.y < g_mag.bottom + EDGE_TH) re |= 4;
            if (abs(pt.y - g_mag.top) < EDGE_TH && pt.x > g_mag.left - EDGE_TH && pt.x < g_mag.right + EDGE_TH)  re |= 8;

            if (re) {
                g_re = re; g_dm = DM_RESIZE; g_magDn = g_mag;
            }
            else if (PtInRect(&g_mag, pt)) {
                g_dm = DM_MOVE; g_magDn = g_mag;
            }
            else {
                // 돋보기 외부: 새로 생성
                g_magOn = false;
                g_mag = { pt.x, pt.y, pt.x, pt.y };
                g_dm = DM_CREATE;
                g_scale = 1.0f;
            }
        }
        else {
            g_mag = { pt.x, pt.y, pt.x, pt.y };
            g_dm = DM_CREATE;
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (!g_lDn) return 0;
        POINT pt = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
        int dx = pt.x - g_ptDn.x;
        int dy = pt.y - g_ptDn.y;

        if (g_dm == DM_CREATE) {
            g_mag = { min(g_ptDn.x, pt.x), min(g_ptDn.y, pt.y),
                      max(g_ptDn.x, pt.x), max(g_ptDn.y, pt.y) };

        }
        else if (g_dm == DM_MOVE) {
            g_mag = g_magDn;
            OffsetRect(&g_mag, dx, dy);
            UpdatePI();

        }
        else if (g_dm == DM_RESIZE) {
            g_mag = g_magDn;
            if (g_re & 1) g_mag.right = g_magDn.right + dx;
            if (g_re & 2) g_mag.bottom = g_magDn.bottom + dy;
            if (g_re & 4) g_mag.left = g_magDn.left + dx;
            if (g_re & 8) g_mag.top = g_magDn.top + dy;
            // 최소 크기 보장
            if (g_mag.right - g_mag.left < 20) {
                if (g_re & 1) g_mag.right = g_mag.left + 20;
                else          g_mag.left = g_mag.right - 20;
            }
            if (g_mag.bottom - g_mag.top < 20) {
                if (g_re & 2) g_mag.bottom = g_mag.top + 20;
                else          g_mag.top = g_mag.bottom - 20;
            }
        }
        InvalidateRect(hw, NULL, TRUE);
        return 0;
    }

    case WM_LBUTTONUP: {
        if (!g_lDn) return 0;
        g_lDn = false;
        ReleaseCapture();

        if (g_dm == DM_CREATE) {
            if (g_mag.right - g_mag.left > 10 && g_mag.bottom - g_mag.top > 10)
                g_magOn = true;
            else
                SetRectEmpty(&g_mag);
        }
        g_dm = DM_NONE;
        InvalidateRect(hw, NULL, TRUE);
        return 0;
    }

    case WM_LBUTTONDBLCLK: {
        POINT pt = { GET_X_LPARAM(lp), GET_Y_LPARAM(lp) };
        g_lDn = false; g_dm = DM_NONE;
        ReleaseCapture();

        if (g_magOn) {
            if (g_dc < MAX_DP) {
                SR  sr = CalcSR(g_mag, g_scale, g_img);
                int mw = g_mag.right - g_mag.left;
                int mh = g_mag.bottom - g_mag.top;
                int dx = (g_W > mw) ? rand() % (g_W - mw) : 0;
                int dy = (g_H > mh) ? rand() % (g_H - mh) : 0;
                g_dp[g_dc++] = { g_img, sr.x, sr.y, sr.w, sr.h, dx, dy, mw, mh };
            }
            else {
                g_dc = 0;   // 11번째 더블클릭: 전부 제거
            }
            InvalidateRect(hw, NULL, TRUE);
        }
        else {
            // 돋보기 없으면 새로 생성
            g_mag = { pt.x, pt.y, pt.x, pt.y };
            g_dm = DM_CREATE; g_lDn = true;
            SetCapture(hw);
        }
        return 0;
    }

                         // ── 타이머
    case WM_TIMER: {
        if (wp == TID_BOUNCE && g_magOn) {
            int mw = g_mag.right - g_mag.left;
            int mh = g_mag.bottom - g_mag.top;
            OffsetRect(&g_mag, g_bvx, g_bvy);

            if (g_mag.left < 0) { g_mag.left = 0;      g_mag.right = mw;     g_bvx = -g_bvx; }
            if (g_mag.right > g_W) { g_mag.right = g_W;    g_mag.left = g_W - mw; g_bvx = -g_bvx; }
            if (g_mag.top < 0) { g_mag.top = 0;      g_mag.bottom = mh;     g_bvy = -g_bvy; }
            if (g_mag.bottom > g_H) { g_mag.bottom = g_H;    g_mag.top = g_H - mh; g_bvy = -g_bvy; }

            UpdatePI();
            InvalidateRect(hw, NULL, TRUE);
        }

        if (wp == TID_SIZE && g_magOn) {
            g_szT += g_szSt;
            if (g_szT >= 2.0f && g_szSt > 0) { g_szT = 2.0f;  g_szSt = -g_szSt; }
            if (g_szT <= 0.4f && g_szSt < 0) { g_szT = 0.4f;  g_szSt = -g_szSt; }

            int ow = g_magOrig.right - g_magOrig.left;
            int oh = g_magOrig.bottom - g_magOrig.top;
            int cx = (g_magOrig.left + g_magOrig.right) / 2;
            int cy = (g_magOrig.top + g_magOrig.bottom) / 2;
            int nw = (int)(ow * g_szT);
            int nh = (int)(oh * g_szT);
            g_mag = { cx - nw / 2, cy - nh / 2, cx + nw / 2, cy + nh / 2 };
            InvalidateRect(hw, NULL, TRUE);
        }
        return 0;
    }

                 // ── 종료 
    case WM_DESTROY:
        KillTimer(hw, TID_BOUNCE);
        KillTimer(hw, TID_SIZE);
        if (g_hBmp[0]) DeleteObject(g_hBmp[0]);
        if (g_hBmp[1]) DeleteObject(g_hBmp[1]);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hw, msg, wp, lp);
}