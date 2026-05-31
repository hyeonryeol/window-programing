#include <windows.h>
#include <windowsx.h>
#include "resource.h"


const int CELL = 12;      // 한 칸 크기(px)
const int MAXN = 256;     // 격자 최대 칸 수

HINSTANCE g_hInst;
HWND      g_hMain;
HWND      g_hDlg;

int  g_cell[MAXN][MAXN];  // 칸 색 인덱스 (-1 = 빈 칸)
bool g_drawMode = false;  // 그리기 모드
bool g_painting = false;  // 드래그 중
int  g_color    = 0;      // 선택 색 인덱스(0~5)

COLORREF g_palette[6] = {
    RGB(255,   0,   0),   // Red
    RGB(  0, 170,   0),   // Green
    RGB(  0,   0, 255),   // Blue
    RGB(  0, 200, 200),   // Cyan
    RGB(230, 220,   0),   // Yellow
    RGB(255,   0, 255),   // Magenta
};

void ClearBoard()
{
    for (int r = 0; r < MAXN; r++)
        for (int c = 0; c < MAXN; c++)
            g_cell[r][c] = -1;
}

// 한 칸을 즉시 화면에 칠하고 배열에 저장 (드래그 중 사용)
void PaintCellImmediate(int x, int y)
{
    if (x < 0 || y < 0) return;
    int c = x / CELL, r = y / CELL;
    if (c < 0 || r < 0 || c >= MAXN || r >= MAXN) return;
    if (g_cell[r][c] == g_color) return;

    g_cell[r][c] = g_color;
    HDC hdc = GetDC(g_hMain);
    HBRUSH br = CreateSolidBrush(g_palette[g_color]);
    RECT cr = { c * CELL + 1, r * CELL + 1, (c + 1) * CELL, (r + 1) * CELL };
    FillRect(hdc, &cr, br);
    DeleteObject(br);
    ReleaseDC(g_hMain, hdc);
}

void DrawScene(HDC hdc, RECT rc)
{
    FillRect(hdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

    int cols = rc.right / CELL + 1;
    int rows = rc.bottom / CELL + 1;
    if (cols > MAXN) cols = MAXN;
    if (rows > MAXN) rows = MAXN;

    // 칠해진 칸
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            if (g_cell[r][c] >= 0) {
                HBRUSH br = CreateSolidBrush(g_palette[g_cell[r][c]]);
                RECT cr = { c * CELL + 1, r * CELL + 1, (c + 1) * CELL, (r + 1) * CELL };
                FillRect(hdc, &cr, br);
                DeleteObject(br);
            }

    // 격자선
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(190, 190, 190));
    HPEN old = (HPEN)SelectObject(hdc, pen);
    for (int x = 0; x <= rc.right; x += CELL) {
        MoveToEx(hdc, x, 0, NULL); LineTo(hdc, x, rc.bottom);
    }
    for (int y = 0; y <= rc.bottom; y += CELL) {
        MoveToEx(hdc, 0, y, NULL); LineTo(hdc, rc.right, y);
    }
    SelectObject(hdc, old);
    DeleteObject(pen);
}

// 컨트롤 패널 대화상자 (모덜리스)
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
    case WM_INITDIALOG:
        CheckRadioButton(hDlg, IDC_RAD_RED, IDC_RAD_MAGENTA, IDC_RAD_RED);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BTN_DRAW:                       // 그리기 모드 토글
            g_drawMode = !g_drawMode;
            SetDlgItemText(hDlg, IDC_BTN_DRAW, g_drawMode ? L"Draw (ON)" : L"Draw");
            break;

        case IDC_BTN_DELETE:                     // 모두 지우기
            ClearBoard();
            InvalidateRect(g_hMain, NULL, FALSE);
            break;

        case IDC_BTN_QUIT:                       // 종료
        case IDCANCEL:
            DestroyWindow(g_hMain);
            break;

        case IDC_RAD_RED:     g_color = 0; break;
        case IDC_RAD_GREEN:   g_color = 1; break;
        case IDC_RAD_BLUE:    g_color = 2; break;
        case IDC_RAD_CYAN:    g_color = 3; break;
        case IDC_RAD_YELLOW:  g_color = 4; break;
        case IDC_RAD_MAGENTA: g_color = 5; break;
        }
        return TRUE;

    case WM_CLOSE:
        DestroyWindow(g_hMain);
        return TRUE;
    }
    return FALSE;
}

// 메인 윈도우 (보드판)
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        ClearBoard();
        return 0;

    case WM_LBUTTONDOWN:
        if (g_drawMode) {
            g_painting = true;
            SetCapture(hWnd);
            PaintCellImmediate(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        }
        return 0;

    case WM_MOUSEMOVE:
        if (g_painting)
            PaintCellImmediate(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        return 0;

    case WM_LBUTTONUP:
        if (g_painting) { g_painting = false; ReleaseCapture(); }
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
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = TEXT("Board64");
    wc.hCursor       = LoadCursor(NULL, IDC_CROSS);
    RegisterClass(&wc);

    g_hMain = CreateWindow(TEXT("Board64"), TEXT("Main Window"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1000, 720,
        NULL, NULL, hInstance, NULL);
    ShowWindow(g_hMain, nCmdShow);
    UpdateWindow(g_hMain);

    // 컨트롤 패널 대화상자를 메인 윈도우 우측에 배치
    g_hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG4), g_hMain, DlgProc);
    RECT mr; GetWindowRect(g_hMain, &mr);
    SetWindowPos(g_hDlg, HWND_TOP, mr.right - 200, mr.top + 40, 0, 0, SWP_NOSIZE);
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
