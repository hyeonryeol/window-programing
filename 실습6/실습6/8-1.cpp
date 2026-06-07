#include <windows.h>
#include <windowsx.h>

HINSTANCE g_hInst;

// ── Grid & map constants ────────────────────────────────────────────
#define CELL    30          // pixels per grid cell
#define GCOLS   15
#define GROWS   15
#define MAP_N    5          // number of connected maps
#define TILE_N   6          // tile type count
#define BG_N     5          // background count
#define MAP_W   (CELL * GCOLS)   // 450
#define MAP_H   (CELL * GROWS)   // 450

// ── Control IDs ────────────────────────────────────────────────────
#define ID_PREV    100
#define ID_NEXT    101
#define ID_DELETE  102
#define ID_TEST    103
#define ID_LIST    120
// Tile buttons: 110 ~ 115
#define ID_TILE(n) (110 + (n))

// ── Global state ───────────────────────────────────────────────────
int  g_tiles[MAP_N][GROWS][GCOLS];   // -1=empty, 0..5=tile type
int  g_mapBg[MAP_N];                 // background index per map
int  g_curMap  = 0;
int  g_selTile = -1;   // -1=none, -2=eraser, 0..5=tile
bool g_test    = false;

// Test-mode character (pixel coords in current map)
int  g_charX   = 10;
int  g_charY   = MAP_H - 60;

HWND g_hMap   = NULL;
HWND g_hList  = NULL;
HWND g_hTile[TILE_N] = {};
HWND g_hLabel = NULL;

// ── Color tables ───────────────────────────────────────────────────
COLORREF g_bgClr[BG_N] = {
    RGB(100, 200, 255),   // 광야배경1  (sky blue)
    RGB(255, 200,  80),   // 산/바다 배경 (sunset)
    RGB( 60,  80, 100),   // 강/바다 배경 (cave)
    RGB( 50, 180, 180),   // 광야배경2  (water)
    RGB( 15,  15,  50),   // 밤배경      (night)
};
LPCWSTR g_bgName[BG_N] = {
    L"광야배경1", L"산/바다 배경", L"강/바다 배경", L"광야배경2", L"밤배경"
};

struct TileInfo { COLORREF top, bot; };
TileInfo g_ti[TILE_N] = {
    { RGB( 50, 200,  50), RGB(120,  80, 40) },   // 풀
    { RGB(160, 110,  60), RGB(110,  65, 25) },   // 흙
    { RGB(180, 150, 110), RGB(130, 100, 70) },   // 벽돌
    { RGB(160, 160, 160), RGB(110, 110,110) },   // 돌
    { RGB(240, 240, 255), RGB(210, 210,235) },   // 구름
    { RGB( 30, 140,  30), RGB( 15,  95, 15) },   // 파이프
};

// ── Forward declarations ────────────────────────────────────────────
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MapProc(HWND, UINT, WPARAM, LPARAM);

// ── Helpers ─────────────────────────────────────────────────────────
void RefreshMap()  { if (g_hMap) InvalidateRect(g_hMap, NULL, FALSE); }
void UpdateLabel() {
    WCHAR buf[64];
    wsprintf(buf, L"현재맵이지 : %d", g_curMap + 1);
    if (g_hLabel) SetWindowText(g_hLabel, buf);
}
void InvalidateTiles() {
    for (int i = 0; i < TILE_N; i++)
        if (g_hTile[i]) InvalidateRect(g_hTile[i], NULL, FALSE);
}

void DrawTileCell(HDC hdc, int col, int row, int t)
{
    int x = col * CELL, y = row * CELL;
    RECT ra = { x, y,       x + CELL, y + CELL / 2 };
    RECT rb = { x, y + CELL / 2, x + CELL, y + CELL };
    HBRUSH ba = CreateSolidBrush(g_ti[t].top);
    HBRUSH bb = CreateSolidBrush(g_ti[t].bot);
    FillRect(hdc, &ra, ba);
    FillRect(hdc, &rb, bb);
    DeleteObject(ba);
    DeleteObject(bb);
    RECT rr = { x, y, x + CELL, y + CELL };
    FrameRect(hdc, &rr, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

// ── Entry point ─────────────────────────────────────────────────────
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
    g_hInst = hInst;
    memset(g_tiles, -1, sizeof(g_tiles));
    memset(g_mapBg,  0, sizeof(g_mapBg));

    WNDCLASSEX wc = { sizeof(wc) };
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpfnWndProc   = WndProc;
    wc.lpszClassName = L"MapToolMain";
    RegisterClassEx(&wc);

    wc.lpfnWndProc   = MapProc;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = L"MapView";
    RegisterClassEx(&wc);

    HWND hWnd = CreateWindow(L"MapToolMain", L"맵툴 만들기",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 870, 630,
        NULL, NULL, hInst, NULL);

    ShowWindow(hWnd, nShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

// ── Main window proc ────────────────────────────────────────────────
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        // Map child window (left area)
        g_hMap = CreateWindow(L"MapView", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            50, 30, MAP_W, MAP_H,
            hWnd, NULL, g_hInst, NULL);

        // Navigation buttons (◄ ►) outside the map
        CreateWindow(L"button", L"\x25C4",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            12, 30 + MAP_H / 2 - 25, 30, 50,
            hWnd, (HMENU)ID_PREV, g_hInst, NULL);
        CreateWindow(L"button", L"\x25BA",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50 + MAP_W + 8, 30 + MAP_H / 2 - 25, 30, 50,
            hWnd, (HMENU)ID_NEXT, g_hInst, NULL);

        // Right panel base X
        int rx = 50 + MAP_W + 55;

        // 6 tile buttons (3 cols × 2 rows), owner-drawn for color preview
        for (int i = 0; i < TILE_N; i++) {
            int c = i % 3, r = i / 3;
            g_hTile[i] = CreateWindow(L"button", NULL,
                WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                rx + c * 52, 30 + r * 52, 48, 48,
                hWnd, (HMENU)(UINT_PTR)ID_TILE(i), g_hInst, NULL);
        }

        // Delete button (right of tile buttons)
        CreateWindow(L"button", L"Delete",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            rx + 3 * 52, 30, 68, 104,
            hWnd, (HMENU)ID_DELETE, g_hInst, NULL);

        // Background listbox
        g_hList = CreateWindow(L"listbox", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_VSCROLL,
            rx, 148, 225, 220,
            hWnd, (HMENU)ID_LIST, g_hInst, NULL);
        for (int i = 0; i < BG_N; i++)
            SendMessage(g_hList, LB_ADDSTRING, 0, (LPARAM)g_bgName[i]);
        SendMessage(g_hList, LB_SETCURSEL, 0, 0);

        // Test button (bottom of right panel)
        CreateWindow(L"button", L"테스트",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            rx, 30 + MAP_H - 40, 225, 38,
            hWnd, (HMENU)ID_TEST, g_hInst, NULL);

        // Status label at bottom
        g_hLabel = CreateWindow(L"static", L"현재맵이지 : 1",
            WS_CHILD | WS_VISIBLE,
            50, 30 + MAP_H + 12, 260, 22,
            hWnd, (HMENU)-1, g_hInst, NULL);

        return 0;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wp);

        if (id == ID_PREV) {
            if (g_curMap > 0) { g_curMap--; UpdateLabel(); RefreshMap(); }
        }
        else if (id == ID_NEXT) {
            if (g_curMap < MAP_N - 1) { g_curMap++; UpdateLabel(); RefreshMap(); }
        }
        else if (id == ID_DELETE) {
            g_selTile = -2;   // eraser
            InvalidateTiles();
        }
        else if (id >= 110 && id <= 115) {
            g_selTile = id - 110;
            InvalidateTiles();
        }
        else if (id == ID_TEST) {
            g_test = !g_test;
            if (g_test) {
                g_charX = 10;
                g_charY = MAP_H - 60;
                SetWindowText(GetDlgItem(hWnd, ID_TEST), L"편집으로");
                SetFocus(hWnd);
            } else {
                SetWindowText(GetDlgItem(hWnd, ID_TEST), L"테스트");
            }
            RefreshMap();
        }
        else if (id == ID_LIST && HIWORD(wp) == LBN_SELCHANGE) {
            int sel = (int)SendMessage(g_hList, LB_GETCURSEL, 0, 0);
            if (sel != LB_ERR) { g_mapBg[g_curMap] = sel; RefreshMap(); }
        }
        return 0;
    }

    // Arrow-key movement in test mode
    case WM_KEYDOWN:
        if (!g_test) return 0;
        if (wp == VK_LEFT) {
            g_charX -= 12;
            if (g_charX < 0) {
                if (g_curMap > 0) { g_curMap--; g_charX = MAP_W - 30; UpdateLabel(); }
                else g_charX = 0;
            }
            RefreshMap();
        }
        else if (wp == VK_RIGHT) {
            g_charX += 12;
            if (g_charX > MAP_W - 24) {
                if (g_curMap < MAP_N - 1) { g_curMap++; g_charX = 0; UpdateLabel(); }
                else g_charX = MAP_W - 24;
            }
            RefreshMap();
        }
        return 0;

    // Draw owner-drawn tile buttons
    case WM_DRAWITEM:
    {
        DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lp;
        int id = (int)dis->CtlID;
        if (id < 110 || id > 115) break;
        int t = id - 110;
        RECT rc = dis->rcItem;
        int h = rc.bottom - rc.top;

        RECT ra = { rc.left, rc.top,        rc.right, rc.top + h / 2 };
        RECT rb = { rc.left, rc.top + h / 2, rc.right, rc.bottom };
        HBRUSH ba = CreateSolidBrush(g_ti[t].top);
        HBRUSH bb = CreateSolidBrush(g_ti[t].bot);
        FillRect(dis->hDC, &ra, ba);
        FillRect(dis->hDC, &rb, bb);
        DeleteObject(ba);
        DeleteObject(bb);

        // Highlight selected tile
        if (g_selTile == t) {
            HPEN pen = CreatePen(PS_SOLID, 3, RGB(255, 255, 0));
            HPEN op  = (HPEN)SelectObject(dis->hDC, pen);
            SelectObject(dis->hDC, GetStockObject(NULL_BRUSH));
            Rectangle(dis->hDC, rc.left + 1, rc.top + 1, rc.right - 1, rc.bottom - 1);
            SelectObject(dis->hDC, op);
            DeleteObject(pen);
        }
        return TRUE;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wp, lp);
}

// ── Map child window proc ───────────────────────────────────────────
LRESULT CALLBACK MapProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_SETFOCUS:
        // Keep keyboard focus on parent so WM_KEYDOWN reaches WndProc
        SetFocus(GetParent(hWnd));
        return 0;

    case WM_LBUTTONDOWN:
    {
        if (g_test) { SetFocus(GetParent(hWnd)); return 0; }

        int mx = GET_X_LPARAM(lp), my = GET_Y_LPARAM(lp);
        int c = mx / CELL, r = my / CELL;
        if (c < 0 || c >= GCOLS || r < 0 || r >= GROWS) return 0;

        if (g_selTile == -2)
            g_tiles[g_curMap][r][c] = -1;
        else if (g_selTile >= 0)
            g_tiles[g_curMap][r][c] = g_selTile;

        InvalidateRect(hWnd, NULL, FALSE);
        SetFocus(GetParent(hWnd));
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc; GetClientRect(hWnd, &rc);

        // ── Background ───────────────────────────────────────────
        HBRUSH bgBr = CreateSolidBrush(g_bgClr[g_mapBg[g_curMap]]);
        FillRect(hdc, &rc, bgBr);
        DeleteObject(bgBr);

        // Simple cloud decorations for sky-type backgrounds
        if (g_mapBg[g_curMap] == 0 || g_mapBg[g_curMap] == 1) {
            HBRUSH cBr = CreateSolidBrush(RGB(255, 255, 255));
            int clouds[][3] = { {30,40,80}, {200,70,60}, {350,30,70} };
            for (auto& cl : clouds) {
                RECT cr = { cl[0], cl[1], cl[0]+cl[2], cl[1]+25 };
                FillRect(hdc, &cr, cBr);
            }
            DeleteObject(cBr);
        }

        // ── Placed tiles ─────────────────────────────────────────
        for (int r = 0; r < GROWS; r++)
            for (int c = 0; c < GCOLS; c++) {
                int t = g_tiles[g_curMap][r][c];
                if (t >= 0) DrawTileCell(hdc, c, r, t);
            }

        // ── Grid lines (edit mode only) ───────────────────────────
        if (!g_test) {
            COLORREF lineClr = (g_mapBg[g_curMap] >= 2)
                ? RGB(180, 180, 180) : RGB(255, 255, 255);
            HPEN pen = CreatePen(PS_SOLID, 1, lineClr);
            HPEN op  = (HPEN)SelectObject(hdc, pen);
            for (int c = 0; c <= GCOLS; c++) {
                MoveToEx(hdc, c * CELL, 0, NULL);
                LineTo(hdc,   c * CELL, MAP_H);
            }
            for (int r = 0; r <= GROWS; r++) {
                MoveToEx(hdc, 0,     r * CELL, NULL);
                LineTo(hdc,   MAP_W, r * CELL);
            }
            SelectObject(hdc, op);
            DeleteObject(pen);
        }

        // ── Character (test mode) ─────────────────────────────────
        if (g_test) {
            // Body
            RECT body = { g_charX,    g_charY,      g_charX + 24, g_charY + 36 };
            HBRUSH bBr = CreateSolidBrush(RGB(220, 50, 50));
            FillRect(hdc, &body, bBr);
            DeleteObject(bBr);
            // Head
            RECT head = { g_charX + 4, g_charY - 22, g_charX + 20, g_charY };
            HBRUSH hBr = CreateSolidBrush(RGB(255, 200, 150));
            FillRect(hdc, &head, hBr);
            DeleteObject(hBr);
        }

        EndPaint(hWnd, &ps);
        return 0;
    }
    }
    return DefWindowProc(hWnd, msg, wp, lp);
}
