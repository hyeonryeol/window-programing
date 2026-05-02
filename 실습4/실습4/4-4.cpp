#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <cwchar>

#define ID_MENU_START 2001
#define ID_MENU_END   2002
#define ID_MENU_HINT  2003
#define ID_MENU_SCORE 2004

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"PieFindGame";
const wchar_t WINDOW_TITLE[] = L"실습 4-4 파이찾기";

const int GRID = 10;
const int CELL = 55;
const int OFFX = 10;
const int OFFY = 10;

enum CellType { CT_EMPTY, CT_MINE, CT_ITEM, CT_PIE };

struct Cell {
    CellType type;
    bool     open;
    int      pieSet;     // 0-4
    int      pieQuarter; // 0=TL,1=TR,2=BL,3=BR
};

Cell g_board[GRID][GRID];
bool g_gameStarted = false;
bool g_gameOver = false;
bool g_dragging = false;
int  g_dragR1 = -1, g_dragC1 = -1;
int  g_dragR2 = -1, g_dragC2 = -1;
bool g_hintActive = false;
bool g_showScore = false;
int  g_score = 0;

COLORREF g_pieColors[5] = {
    RGB(50,  200, 50),
    RGB(230, 130, 0),
    RGB(150, 0,   220),
    RGB(200, 50,  50),
    RGB(50,  130, 230),
};

void InitBoard()
{
    int arr[GRID * GRID], idx = 0;
    for (int i = 0;i < 20;i++) arr[idx++] = CT_MINE;
    for (int i = 0;i < 10;i++) arr[idx++] = CT_ITEM;
    for (int i = 0;i < 20;i++) arr[idx++] = CT_PIE;
    for (int i = 0;i < 30;i++) arr[idx++] = CT_EMPTY;
    for (int i = 99;i > 0;i--) { int j = rand() % (i + 1); int t = arr[i];arr[i] = arr[j];arr[j] = t; }

    // 파이 5세트 × 4조각 섞기
    int pSet[20], pQ[20];
    for (int s = 0;s < 5;s++) for (int q = 0;q < 4;q++) { pSet[s * 4 + q] = s; pQ[s * 4 + q] = q; }
    for (int i = 19;i > 0;i--) {
        int j = rand() % (i + 1);
        int ts = pSet[i];pSet[i] = pSet[j];pSet[j] = ts;
        int tq = pQ[i];pQ[i] = pQ[j];pQ[j] = tq;
    }

    int pIdx = 0;
    for (int r = 0;r < GRID;r++) for (int c = 0;c < GRID;c++) {
        g_board[r][c].type = (CellType)arr[r * GRID + c];
        g_board[r][c].open = false;
        g_board[r][c].pieSet = -1;
        g_board[r][c].pieQuarter = -1;
        if (g_board[r][c].type == CT_PIE) {
            g_board[r][c].pieSet = pSet[pIdx]; g_board[r][c].pieQuarter = pQ[pIdx]; pIdx++;
        }
    }
    g_gameOver = false; g_showScore = false; g_gameStarted = true; g_hintActive = false;
}

void OpenCell(HWND hWnd, int r, int c)
{
    if (r < 0 || r >= GRID || c < 0 || c >= GRID) return;
    if (!g_gameStarted || g_gameOver || g_board[r][c].open) return;
    g_board[r][c].open = true;

    if (g_board[r][c].type == CT_MINE) {
        // 지뢰: 게임 오버, 모든 지뢰 공개
        g_gameOver = true;
        for (int i = 0;i < GRID;i++) for (int j = 0;j < GRID;j++)
            if (g_board[i][j].type == CT_MINE) g_board[i][j].open = true;
        InvalidateRect(hWnd, NULL, TRUE);
        MessageBoxW(hWnd, L"지뢰를 밟았습니다! 게임 오버!", L"게임 오버", MB_OK);
    }
    else if (g_board[r][c].type == CT_ITEM) {
        // 열린 파이 조각이 있는 세트의 나머지 조각 모두 공개
        bool setHasOpen[5] = {};
        for (int i = 0;i < GRID;i++) for (int j = 0;j < GRID;j++)
            if (g_board[i][j].type == CT_PIE && g_board[i][j].open)
                setHasOpen[g_board[i][j].pieSet] = true;

        for (int i = 0;i < GRID;i++) for (int j = 0;j < GRID;j++)
            if (g_board[i][j].type == CT_PIE && setHasOpen[g_board[i][j].pieSet])
                g_board[i][j].open = true;
    }
    
}

int CalcScore()
{
    bool found[5][4] = {};
    for (int r = 0;r < GRID;r++) for (int c = 0;c < GRID;c++)
        if (g_board[r][c].type == CT_PIE && g_board[r][c].open)
            found[g_board[r][c].pieSet][g_board[r][c].pieQuarter] = true;
    int score = 0;
    for (int s = 0;s < 5;s++) {
        bool all = true;
        for (int q = 0;q < 4;q++) if (!found[s][q]) { all = false;break; }
        if (all) score++;
    }
    return score;
}

// 파이 1/4 조각 그리기 (cx,cy=중심, r=반지름, q=조각번호)
void DrawPieQ(HDC hdc, int cx, int cy, int r, int q, COLORREF col)
{
    int L = cx - r, T = cy - r, R = cx + r, B = cy + r;
    HBRUSH br = CreateSolidBrush(col);
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH ob = (HBRUSH)SelectObject(hdc, br);
    HPEN   op = (HPEN)SelectObject(hdc, pen);
    switch (q) {
    case 0: Pie(hdc, L, T, R, B, cx, cy - r, cx - r, cy); break; // TL
    case 1: Pie(hdc, L, T, R, B, cx + r, cy, cx, cy - r); break; // TR
    case 2: Pie(hdc, L, T, R, B, cx - r, cy, cx, cy + r); break; // BL
    case 3: Pie(hdc, L, T, R, B, cx, cy + r, cx + r, cy); break; // BR
    }
    SelectObject(hdc, ob); SelectObject(hdc, op);
    DeleteObject(br); DeleteObject(pen);
}

// 셀 하나 그리기
void DrawCell(HDC hdc, int r, int c, bool forceOpen)
{
    int x = OFFX + c * CELL, y = OFFY + r * CELL;
    bool isOpen = g_board[r][c].open || forceOpen;

    // 배경
    HBRUSH bg = CreateSolidBrush(isOpen ? RGB(210, 210, 210) : RGB(80, 100, 170));
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(40, 40, 40));
    HBRUSH ob = (HBRUSH)SelectObject(hdc, bg);
    HPEN   op = (HPEN)SelectObject(hdc, pen);
    Rectangle(hdc, x, y, x + CELL, y + CELL);
    SelectObject(hdc, ob); SelectObject(hdc, op);
    DeleteObject(bg); DeleteObject(pen);

    if (!isOpen) return;

    int cx = x + CELL / 2, cy = y + CELL / 2, rr = CELL / 2 - 6;

    switch (g_board[r][c].type) {
    case CT_MINE: {
        // 지뢰: B 텍스트
        SetTextColor(hdc, RGB(180, 0, 0)); SetBkMode(hdc, TRANSPARENT);
        HFONT f = CreateFontW(26, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");
        HFONT of = (HFONT)SelectObject(hdc, f);
        RECT rc = { x,y,x + CELL,y + CELL };
        DrawTextW(hdc, L"B", 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        SelectObject(hdc, of); DeleteObject(f);
        break;
    }
    case CT_ITEM: {
        // 아이템: 다이아몬드
        POINT pts[4] = { {cx,cy - rr},{cx + rr,cy},{cx,cy + rr},{cx - rr,cy} };
        HBRUSH ibr = CreateSolidBrush(RGB(230, 180, 0));
        HPEN   ipen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        HBRUSH iob = (HBRUSH)SelectObject(hdc, ibr);
        HPEN   iop = (HPEN)SelectObject(hdc, ipen);
        Polygon(hdc, pts, 4);
        SelectObject(hdc, iob); SelectObject(hdc, iop);
        DeleteObject(ibr); DeleteObject(ipen);
        break;
    }
    case CT_PIE:
        DrawPieQ(hdc, cx, cy, rr, g_board[r][c].pieQuarter, g_pieColors[g_board[r][c].pieSet]);
        break;
    case CT_EMPTY:
        break;
    }
}

// 우측 패널: 완성된 파이 표시
void DrawSidePanel(HDC hdc)
{
    bool found[5][4] = {};
    for (int r = 0;r < GRID;r++) for (int c = 0;c < GRID;c++)
        if (g_board[r][c].type == CT_PIE && g_board[r][c].open)
            found[g_board[r][c].pieSet][g_board[r][c].pieQuarter] = true;

    int sx = OFFX + GRID * CELL + 20, ps = 50;
    SetBkMode(hdc, TRANSPARENT); SetTextColor(hdc, RGB(0, 0, 0));

    RECT titleRc = { sx,OFFY,sx + 140,OFFY + 20 };
    DrawTextW(hdc, L"맞춰진 파이", -1, &titleRc, DT_LEFT | DT_SINGLELINE);

    for (int s = 0;s < 5;s++) {
        int baseY = OFFY + 25 + s * (ps + 15);
        int cx = sx + ps / 2, cy = baseY + ps / 2, rr = ps / 2 - 3;

        // 원 배경
        HBRUSH bg = CreateSolidBrush(RGB(220, 220, 220));
        HPEN bpen = CreatePen(PS_SOLID, 1, RGB(130, 130, 130));
        HBRUSH ob = (HBRUSH)SelectObject(hdc, bg);
        HPEN   op = (HPEN)SelectObject(hdc, bpen);
        Ellipse(hdc, cx - rr, cy - rr, cx + rr, cy + rr);
        SelectObject(hdc, ob); SelectObject(hdc, op);
        DeleteObject(bg); DeleteObject(bpen);

        // 찾은 조각 표시
        for (int q = 0;q < 4;q++)
            if (found[s][q]) DrawPieQ(hdc, cx, cy, rr, q, g_pieColors[s]);

        // 세트 이름
        wchar_t lbl[8]; swprintf_s(lbl, 8, L"Set %d", s + 1);
        RECT lr = { sx + ps + 5,baseY + ps / 2 - 10,sx + ps + 70,baseY + ps / 2 + 10 };
        SetTextColor(hdc, RGB(0, 0, 0));
        DrawTextW(hdc, lbl, -1, &lr, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }

    // 스코어 표시
    if (g_showScore) {
        wchar_t buf[32]; swprintf_s(buf, 32, L"Score: %d / 5", g_score);
        SetTextColor(hdc, RGB(0, 0, 180));
        RECT sr = { sx,OFFY + 25 + 5 * (ps + 15) + 5,sx + 150,OFFY + 25 + 5 * (ps + 15) + 30 };
        DrawTextW(hdc, buf, -1, &sr, DT_LEFT | DT_SINGLELINE);
    }
}

// 힌트 타이머: 2초 후 닫기
void CALLBACK HintTimer(HWND hWnd, UINT, UINT_PTR, DWORD)
{
    KillTimer(hWnd, 10);
    g_hintActive = false;
    InvalidateRect(hWnd, NULL, TRUE);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    srand((unsigned)time(nullptr));

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc); wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc; wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;
    RegisterClassExW(&wc);

    HMENU hMenu = CreateMenu(), hSub = CreatePopupMenu();
    AppendMenuW(hSub, MF_STRING, ID_MENU_START, L"Game Start");
    AppendMenuW(hSub, MF_STRING, ID_MENU_END, L"Game End");
    AppendMenuW(hSub, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hSub, MF_STRING, ID_MENU_HINT, L"Hint");
    AppendMenuW(hSub, MF_STRING, ID_MENU_SCORE, L"Score");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hSub, L"게임");

    HWND hWnd = CreateWindowExW(0, CLASS_NAME, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 660, nullptr, hMenu, hInstance, nullptr);
    if (!hWnd) return 0;

    ShowWindow(hWnd, nCmdShow); UpdateWindow(hWnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case ID_MENU_START:
            InitBoard(); InvalidateRect(hWnd, NULL, TRUE); break;
        case ID_MENU_END:
            g_gameStarted = false; g_gameOver = true; InvalidateRect(hWnd, NULL, TRUE); break;
        case ID_MENU_HINT:
            if (g_gameStarted && !g_gameOver) {
                g_hintActive = true;
                SetTimer(hWnd, 10, 2000, HintTimer);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        case ID_MENU_SCORE:
            if (g_gameStarted) {
                g_score = CalcScore(); g_showScore = true; InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        }
        return 0;

    case WM_LBUTTONDOWN: {
        if (!g_gameStarted || g_gameOver) return 0;
        int mx = LOWORD(lParam), my = HIWORD(lParam);
        int c = (mx - OFFX) / CELL, r = (my - OFFY) / CELL;
        if (r >= 0 && r < GRID && c >= 0 && c < GRID && mx >= OFFX && my >= OFFY) {
            g_dragging = true;
            g_dragR1 = g_dragR2 = r; g_dragC1 = g_dragC2 = c;
            SetCapture(hWnd);
        }
        return 0;
    }

    case WM_MOUSEMOVE:
        if (g_dragging) {
            int mx = LOWORD(lParam), my = HIWORD(lParam);
            g_dragR2 = max(0, min(GRID - 1, (my - OFFY) / CELL));
            g_dragC2 = max(0, min(GRID - 1, (mx - OFFX) / CELL));
            InvalidateRect(hWnd, NULL, TRUE);
        }
        return 0;

    case WM_LBUTTONUP:
        if (g_dragging) {
            ReleaseCapture(); g_dragging = false;
            int r1 = min(g_dragR1, g_dragR2), r2 = max(g_dragR1, g_dragR2);
            int c1 = min(g_dragC1, g_dragC2), c2 = max(g_dragC1, g_dragC2);
            for (int r = r1;r <= r2;r++) for (int c = c1;c <= c2;c++) OpenCell(hWnd, r, c);
            g_dragR1 = g_dragR2 = g_dragC1 = g_dragC2 = -1;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps);
        RECT rt; GetClientRect(hWnd, &rt);

        // 더블 버퍼링
        HDC     mdc = CreateCompatibleDC(hdc);
        HBITMAP hBmp = CreateCompatibleBitmap(hdc, rt.right, rt.bottom);
        HBITMAP oldBmp = (HBITMAP)SelectObject(mdc, hBmp);
        HBRUSH  bgBr = CreateSolidBrush(RGB(240, 240, 240));
        FillRect(mdc, &rt, bgBr); DeleteObject(bgBr);

        if (g_gameStarted) {
            // 보드 그리기
            for (int r = 0;r < GRID;r++) for (int c = 0;c < GRID;c++) DrawCell(mdc, r, c, g_hintActive);

            // 드래그 영역 강조
            if (g_dragging && g_dragR1 >= 0) {
                int r1 = min(g_dragR1, g_dragR2), r2 = max(g_dragR1, g_dragR2);
                int c1 = min(g_dragC1, g_dragC2), c2 = max(g_dragC1, g_dragC2);
                HPEN hpen = CreatePen(PS_SOLID, 3, RGB(255, 200, 0));
                HPEN op = (HPEN)SelectObject(mdc, hpen);
                HBRUSH ob = (HBRUSH)SelectObject(mdc, GetStockObject(NULL_BRUSH));
                Rectangle(mdc, OFFX + c1 * CELL, OFFY + r1 * CELL, OFFX + (c2 + 1) * CELL, OFFY + (r2 + 1) * CELL);
                SelectObject(mdc, op); SelectObject(mdc, ob); DeleteObject(hpen);
            }

            DrawSidePanel(mdc);
        }
        else {
            SetTextColor(mdc, RGB(100, 100, 100)); SetBkMode(mdc, TRANSPARENT);
            DrawTextW(mdc, L"게임 메뉴에서 'Game Start'를 선택하세요.", -1, &rt,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }

        BitBlt(hdc, 0, 0, rt.right, rt.bottom, mdc, 0, 0, SRCCOPY);
        SelectObject(mdc, oldBmp); DeleteObject(hBmp); DeleteDC(mdc);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hWnd, 10); PostQuitMessage(0); return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}