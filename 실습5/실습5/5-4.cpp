#include <windows.h>
#include <windowsx.h>
#include <cstdlib>
#include <ctime>
#include <vector>

#define IDM_GAME_START  1001
#define IDM_GAME_END    1002
#define IDM_SCORE_64    1003
#define IDM_SCORE_32    1004
#define IDM_OBS_2       1005
#define IDM_OBS_3       1006
#define IDM_OBS_4       1007

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"실습 5-4";

const int CELL = 70;
const int OX = 30;
const int OY = 50;

int  g_cells[6][6];
int  g_targetScore = 64;
int  g_obstacleCount = 2;
bool g_gameRunning = false;
int  g_score = 0;

bool g_dragging = false;
int  g_startX = 0;
int  g_startY = 0;

HINSTANCE g_hInstance;
HBITMAP   g_numBmps[6];

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

    
void LoadNumberBitmaps()
{
    const wchar_t* files[] = {
        L"num2.bmp", L"num4.bmp", L"num8.bmp",
        L"num16.bmp", L"num32.bmp", L"num64.bmp"
    };
    for (int i = 0; i < 6; i++)
    {
        g_numBmps[i] = (HBITMAP)LoadImage(
            NULL, files[i], IMAGE_BITMAP,
            CELL, CELL,
            LR_LOADFROMFILE  
        );
    }
}

void DeleteNumberBitmaps()
{
    for (int i = 0; i < 6; i++)
        DeleteObject(g_numBmps[i]);
}

int GetBmpIndex(int val)
{
    switch (val)
    {
    case  2: return 0;  case  4: return 1;
    case  8: return 2;  case 16: return 3;
    case 32: return 4;  case 64: return 5;
    default: return 5;
    }
}



void InitBoard()
{
    memset(g_cells, 0, sizeof(g_cells));
    g_score = 0;

    for (int placed = 0; placed < g_obstacleCount; )
    {
        int r = rand() % 6, c = rand() % 6;
        if (g_cells[r][c] == 0) { g_cells[r][c] = -1; placed++; }
    }
    for (int placed = 0; placed < 2; )
    {
        int r = rand() % 6, c = rand() % 6;
        if (g_cells[r][c] == 0) { g_cells[r][c] = 2; placed++; }
    }
}

bool SpawnNew2()
{
    std::vector<std::pair<int, int>> empties;
    for (int r = 0; r < 6; r++)
        for (int c = 0; c < 6; c++)
            if (g_cells[r][c] == 0)
                empties.push_back({ r, c });
    if (empties.empty()) return false;
    auto& p = empties[rand() % empties.size()];
    g_cells[p.first][p.second] = 2;
    return true;
}

int PushLeft(int* seg, int len)
{
    std::vector<int> v;
    for (int i = 0; i < len; i++)
        if (seg[i]) v.push_back(seg[i]);

    std::vector<int> merged;
    bool didMerge = false;
    int  score = 0;
    for (int val : v)
    {
        if (!merged.empty() && !didMerge && merged.back() == val)
        {
            merged.back() *= 2;
            score += merged.back();
            didMerge = true;
        }
        else { merged.push_back(val); didMerge = false; }
    }
    for (int i = 0; i < len; i++)
        seg[i] = (i < (int)merged.size()) ? merged[i] : 0;
    return score;
}

int PushRight(int* seg, int len)
{
    std::vector<int> rev(len);
    for (int i = 0; i < len; i++) rev[i] = seg[len - 1 - i];
    int score = PushLeft(rev.data(), len);
    for (int i = 0; i < len; i++) seg[i] = rev[len - 1 - i];
    return score;
}

void DoLeft()
{
    for (int r = 0; r < 6; r++)
    {
        int start = 0;
        for (int c = 0; c <= 6; c++)
            if (c == 6 || g_cells[r][c] == -1)
            {
                if (c > start) g_score += PushLeft(&g_cells[r][start], c - start);
                start = c + 1;
            }
    }
}

void DoRight()
{
    for (int r = 0; r < 6; r++)
    {
        int start = 0;
        for (int c = 0; c <= 6; c++)
            if (c == 6 || g_cells[r][c] == -1)
            {
                if (c > start) g_score += PushRight(&g_cells[r][start], c - start);
                start = c + 1;
            }
    }
}

void DoUp()
{
    for (int col = 0; col < 6; col++)
    {
        int start = 0;
        for (int r = 0; r <= 6; r++)
            if (r == 6 || g_cells[r][col] == -1)
            {
                int len = r - start;
                if (len > 0)
                {
                    std::vector<int> seg(len);
                    for (int i = 0; i < len; i++) seg[i] = g_cells[start + i][col];
                    g_score += PushLeft(seg.data(), len);
                    for (int i = 0; i < len; i++) g_cells[start + i][col] = seg[i];
                }
                start = r + 1;
            }
    }
}

void DoDown()
{
    for (int col = 0; col < 6; col++)
    {
        int start = 0;
        for (int r = 0; r <= 6; r++)
            if (r == 6 || g_cells[r][col] == -1)
            {
                int len = r - start;
                if (len > 0)
                {
                    std::vector<int> seg(len);
                    for (int i = 0; i < len; i++) seg[i] = g_cells[start + i][col];
                    g_score += PushRight(seg.data(), len);
                    for (int i = 0; i < len; i++) g_cells[start + i][col] = seg[i];
                }
                start = r + 1;
            }
    }
}

bool CheckWin()
{
    for (int r = 0; r < 6; r++)
        for (int c = 0; c < 6; c++)
            if (g_cells[r][c] >= g_targetScore) return true;
    return false;
}

bool CheckLose()
{
    for (int r = 0; r < 6; r++)
        for (int c = 0; c < 6; c++)
            if (g_cells[r][c] == 0) return false;
    return true;
}


void DrawBoard(HDC hdc)
{
    for (int r = 0; r < 6; r++)
    {
        for (int c = 0; c < 6; c++)
        {
            int x = OX + c * CELL;
            int y = OY + r * CELL;
            int val = g_cells[r][c];

            if (val == 0)           // 빈 칸
            {
                HBRUSH br = CreateSolidBrush(RGB(205, 193, 180));
                HBRUSH old = (HBRUSH)SelectObject(hdc, br);
                Rectangle(hdc, x, y, x + CELL, y + CELL);
                SelectObject(hdc, old);
                DeleteObject(br);
            }
            else if (val == -1)     // 장애물 
            {
                HBRUSH br = CreateSolidBrush(RGB(100, 149, 237));
                HBRUSH old = (HBRUSH)SelectObject(hdc, br);
                Rectangle(hdc, x, y, x + CELL, y + CELL);
                SelectObject(hdc, old);
                DeleteObject(br);
            }
            else                    // 숫자 비트맵
            {
                HDC     memDC = CreateCompatibleDC(hdc);
                HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, g_numBmps[GetBmpIndex(val)]);
                StretchBlt(hdc, x, y, CELL, CELL, memDC, 0, 0, 70, 70, SRCCOPY);
                SelectObject(memDC, oldBmp);
                DeleteDC(memDC);
            }
        }
    }


}



HMENU CreateAppMenu()
{
    HMENU hMenu = CreateMenu();

    HMENU hGame = CreatePopupMenu();
    AppendMenu(hGame, MF_STRING, IDM_GAME_START, L"게임 시작");
    AppendMenu(hGame, MF_STRING, IDM_GAME_END, L"게임 종료");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hGame, L"게임");

    HMENU hScore = CreatePopupMenu();
    AppendMenu(hScore, MF_STRING, IDM_SCORE_64, L"64");
    AppendMenu(hScore, MF_STRING, IDM_SCORE_32, L"32");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hScore, L"목표 점수");

    HMENU hObs = CreatePopupMenu();
    AppendMenu(hObs, MF_STRING, IDM_OBS_2, L"2");
    AppendMenu(hObs, MF_STRING, IDM_OBS_3, L"3");
    AppendMenu(hObs, MF_STRING, IDM_OBS_4, L"4");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hObs, L"장애물 숫자");

    return hMenu;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    g_hInstance = hInstance;
    srand((unsigned)time(nullptr));

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;
    RegisterClassExW(&wc);

    HWND hWnd = CreateWindowExW(0, CLASS_NAME, WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 530, 580,
        nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return 0;

    SetMenu(hWnd, CreateAppMenu());
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        LoadNumberBitmaps();
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_GAME_START:
            g_gameRunning = true;
            InitBoard();
            InvalidateRect(hWnd, nullptr, TRUE);
            break;
        case IDM_GAME_END:
            g_gameRunning = false;
            memset(g_cells, 0, sizeof(g_cells));
            InvalidateRect(hWnd, nullptr, TRUE);
            break;
        case IDM_SCORE_64: g_targetScore = 64; break;
        case IDM_SCORE_32: g_targetScore = 32; break;
        case IDM_OBS_2:    g_obstacleCount = 2;  break;
        case IDM_OBS_3:    g_obstacleCount = 3;  break;
        case IDM_OBS_4:    g_obstacleCount = 4;  break;
        }
        return 0;

    case WM_LBUTTONDOWN:
        if (g_gameRunning)
        {
            g_dragging = true;
            g_startX = GET_X_LPARAM(lParam);
            g_startY = GET_Y_LPARAM(lParam);
        }
        return 0;

    case WM_LBUTTONUP:
    {
        if (!g_dragging || !g_gameRunning) return 0;
        g_dragging = false;

        int dx = GET_X_LPARAM(lParam) - g_startX;
        int dy = GET_Y_LPARAM(lParam) - g_startY;
        if (abs(dx) < 10 && abs(dy) < 10) return 0;

        if (abs(dx) >= abs(dy))
        {
            if (dx > 0) DoRight(); else DoLeft();
        }
        else
        {
            if (dy > 0) DoDown();  else DoUp();
        }

        bool spawned = SpawnNew2();

        if (CheckWin())
        {
            MessageBox(hWnd, L"목표 달성! 승리", L"게임 종료", MB_OK);
            g_gameRunning = false;
        }
        else if (!spawned || CheckLose())
        {
            MessageBox(hWnd, L"움직일 칸이 없습니다. 패배", L"게임 종료", MB_OK);
            g_gameRunning = false;
        }

        InvalidateRect(hWnd, nullptr, TRUE);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT cr;
        GetClientRect(hWnd, &cr);
        HDC     memDC = CreateCompatibleDC(hdc);
        HBITMAP memBmp = CreateCompatibleBitmap(hdc, cr.right, cr.bottom);
        HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

        FillRect(memDC, &cr, (HBRUSH)(COLOR_WINDOW + 1));

        if (g_gameRunning)
            DrawBoard(memDC);
        else
        {
            SetBkMode(memDC, TRANSPARENT);
            SetTextColor(memDC, RGB(0, 0, 0));
            TextOut(memDC, OX, OY, L"게임 메뉴에서 '게임 시작'을 선택하세요.", 20);
        }

        BitBlt(hdc, 0, 0, cr.right, cr.bottom, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBmp);
        DeleteObject(memBmp);
        DeleteDC(memDC);

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        DeleteNumberBitmaps();
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}