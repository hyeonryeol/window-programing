#include <windows.h>
#include <ctime>
#include <cstdlib>
#include <cwchar>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"실습 4-3 벽돌깨기";

// 블록 행/열 수, 크기, 시작 위치
const int ROWS = 3, COLS = 10;
const int BLOCK_W = 60, BLOCK_H = 30;
const int BLOCK_START_X = 100;
const int ROW_BASE_Y[3] = { 10, 45, 80 };
const int BALL_R = 15;
const int TIMER_BLOCK = 1, TIMER_BALL = 2;

// 화면에 출력할 텍스트 (P, T 키)
wchar_t g_textLine1[256] = {};
wchar_t g_textLine2[256] = {};
wchar_t g_textLine3[256] = {};
bool    g_showText = false;

// 블록 이동 방향 및 X 오프셋
int  g_blockX = 0;
bool g_moveRight = true;

// 블록 개별 상태
bool     g_blockVisible[ROWS][COLS];
int      g_blockSlideY[ROWS][COLS]; // 낙하 Y 오프셋
bool     g_blockSliding[ROWS][COLS]; // 낙하 중 여부
COLORREF g_rowColor[ROWS];       // 줄별 색상
COLORREF g_blockColor[ROWS][COLS]; // 블록 개별 색상
int      g_blockCooldown[ROWS][COLS]; // 충돌 후 무적 시간

// 바 위치 및 드래그 상태
const int BAR_BASE_LEFT = 300, BAR_BASE_TOP = 530;
const int BAR_W = 200, BAR_H = 20;
int  g_barOffsetX = 0;
bool g_dragging = false;
int  g_prevMouseX = 0;

// 공 위치, 방향, 활성 여부
int  g_ballCX = 0, g_ballCY = 0;
int  g_ballDX = 8, g_ballDY = -8;
bool g_ballActive = false;

// 게임 상태
bool   g_paused = false;
bool   g_gameStarted = false;
time_t g_startTime = 0;
int    g_colorChangedCount = 0; // 색 변한 벽돌 수
int    g_disappearedCount = 0; // 사라진 벽돌 수

// 블록 RECT 계산 (홀수 줄은 반대 방향)
void GetBlockRect(int row, int col, RECT& r)
{
    int xOff = (row == 1) ? -g_blockX : g_blockX;
    r.left = BLOCK_START_X + col * BLOCK_W + xOff;
    r.top = ROW_BASE_Y[row] + g_blockSlideY[row][col];
    r.right = r.left + BLOCK_W;
    r.bottom = r.top + BLOCK_H;
}

// 바 RECT 계산
void GetBarRect(RECT& r)
{
    r.left = BAR_BASE_LEFT + g_barOffsetX;
    r.top = BAR_BASE_TOP;
    r.right = r.left + BAR_W;
    r.bottom = r.top + BAR_H;
}

// 공을 바 위로 리셋
void ResetBall()
{
    RECT bar; GetBarRect(bar);
    g_ballCX = (bar.left + bar.right) / 2;
    g_ballCY = bar.top - BALL_R - 1;
    g_ballActive = false;
}

COLORREF RandomColor()
{
    return RGB(rand() % 200 + 55, rand() % 200 + 55, rand() % 200 + 55);
}

// 게임 전체 리셋
void ResetGame(HWND hWnd)
{
    g_blockX = 0;
    g_moveRight = true;
    for (int r = 0; r < ROWS; ++r) {
        g_rowColor[r] = RGB(255, 215, 0);
        for (int c = 0; c < COLS; ++c) {
            g_blockVisible[r][c] = true;
            g_blockSlideY[r][c] = 0;
            g_blockSliding[r][c] = false;
            g_blockCooldown[r][c] = 0;
            g_blockColor[r][c] = RGB(255, 215, 0);
        }
    }
    g_barOffsetX = 0;
    g_ballDX = 8;
    g_ballDY = -8;
    g_paused = false;
    g_gameStarted = false;
    g_colorChangedCount = 0;
    g_disappearedCount = 0;
    g_showText = false;
    g_textLine1[0] = g_textLine2[0] = g_textLine3[0] = L'\0';
    KillTimer(hWnd, TIMER_BALL);
    ResetBall();
    InvalidateRect(hWnd, NULL, FALSE);
}

// 블록 좌우 이동 + 낙하 중인 블록 처리
void CALLBACK TimerBlock(HWND hWnd, UINT, UINT_PTR, DWORD)
{
    if (g_paused) return;

    RECT rt; GetClientRect(hWnd, &rt);

    // 항상 이동 먼저
    if (g_moveRight) g_blockX += 10;
    else             g_blockX -= 10;

    int rightEdge = 0, leftEdge = rt.right;
    bool found = false;
    for (int row = 0; row < ROWS; ++row)
        for (int col = 0; col < COLS; ++col) {
            if (!g_blockVisible[row][col] || g_blockSliding[row][col]) continue;
            RECT r; GetBlockRect(row, col, r);
            if (r.right > rightEdge) rightEdge = r.right;
            if (r.left < leftEdge)  leftEdge = r.left;
            found = true;
        }

    if (found) {
        if (g_moveRight && rightEdge >= rt.right) {
            g_moveRight = false;
            g_blockX -= 10; // 경계 밖으로 나간 만큼 되돌림
        }
        else if (!g_moveRight && leftEdge <= rt.left) {
            g_moveRight = true;
            g_blockX += 10; // 경계 밖으로 나간 만큼 되돌림
        }
    }

    for (int row = 0; row < ROWS; ++row)
        for (int col = 0; col < COLS; ++col) {
            if (!g_blockSliding[row][col] || !g_blockVisible[row][col]) continue;
            g_blockSlideY[row][col] += 30;  // 0 → 30으로 수정
            g_blockColor[row][col] = RandomColor();
            RECT r; GetBlockRect(row, col, r);
            if (r.top >= rt.bottom) {
                g_blockVisible[row][col] = false;
                g_blockSliding[row][col] = false;
                g_disappearedCount++;
            }
            if (g_blockCooldown[row][col] > 0) g_blockCooldown[row][col]--;
        }

    InvalidateRect(hWnd, NULL, FALSE);
}

// 공 이동 + 충돌 처리
void CALLBACK TimerBall(HWND hWnd, UINT, UINT_PTR, DWORD)
{
    if (g_paused || !g_ballActive) return;

    RECT rt; GetClientRect(hWnd, &rt);
    g_ballCX += g_ballDX;
    g_ballCY += g_ballDY;

    // 벽 반사
    if (g_ballCX - BALL_R <= rt.left) { g_ballCX = rt.left + BALL_R; g_ballDX = abs(g_ballDX); }
    if (g_ballCX + BALL_R >= rt.right) { g_ballCX = rt.right - BALL_R; g_ballDX = -abs(g_ballDX); }
    if (g_ballCY - BALL_R <= rt.top) { g_ballCY = rt.top + BALL_R; g_ballDY = abs(g_ballDY); }

    // 바 반사
    RECT bar; GetBarRect(bar);
    if (g_ballDY > 0 &&
        g_ballCY + BALL_R >= bar.top && g_ballCY < bar.bottom &&
        g_ballCX + BALL_R >  bar.left && g_ballCX - BALL_R < bar.right)
    {
        g_ballCY = bar.top - BALL_R;
        g_ballDY = -abs(g_ballDY);
    }

    // 바닥 추락 시 리셋
    if (g_ballCY - BALL_R > rt.bottom) { ResetBall(); return; }

    // 블록 충돌 (맞은 블록 하나만 낙하)
    for (int row = 0; row < ROWS; ++row)
        for (int col = 0; col < COLS; ++col) {
            if (!g_blockVisible[row][col] || g_blockCooldown[row][col] > 0 || g_blockSliding[row][col]) continue;
            RECT r; GetBlockRect(row, col, r);
            if (g_ballCX + BALL_R > r.left && g_ballCX - BALL_R < r.right &&
                g_ballCY + BALL_R > r.top && g_ballCY - BALL_R < r.bottom)
            {
                g_ballDY = -g_ballDY;
                g_colorChangedCount += COLS;
                g_blockSliding[row][col] = true;
                g_blockCooldown[row][col] = 15;
                // 줄 전체 색 한 번만 변경
                COLORREF newColor = RandomColor();
                for (int c = 0; c < COLS; ++c)
                    g_blockColor[row][c] = newColor;
                goto done;
            }
        }
done:
    InvalidateRect(hWnd, NULL, FALSE);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
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
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
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
        ResetGame(hWnd);
        SetTimer(hWnd, TIMER_BLOCK, 200, TimerBlock);
        return 0;

    case WM_LBUTTONDOWN:
    {
        int mx = LOWORD(lParam), my = HIWORD(lParam);
        RECT bar; GetBarRect(bar);
        // 바 위에서 클릭 시 드래그 시작
        if (mx >= bar.left && mx <= bar.right &&
            my >= bar.top && my <= bar.bottom)
        {
            g_dragging = true;
            g_prevMouseX = mx;
            SetCapture(hWnd);
        }
        return 0;
    }

    case WM_MOUSEMOVE:
        if (g_dragging) {
            int mx = LOWORD(lParam);
            g_barOffsetX += mx - g_prevMouseX; // 이동량 누적
            g_prevMouseX = mx;
            if (!g_ballActive) ResetBall(); // 발사 전이면 공도 같이 이동
            InvalidateRect(hWnd, NULL, FALSE);
        }
        return 0;

    case WM_LBUTTONUP:
        g_dragging = false;
        ReleaseCapture();
        return 0;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case 'S': // 공 발사
            if (!g_ballActive && !g_paused) {
                g_ballActive = true;
                g_gameStarted = true;
                g_startTime = time(nullptr);
                SetTimer(hWnd, TIMER_BALL, 30, TimerBall);
            }
            break;

        case 'P': // 일시정지 / 재개
            g_paused = !g_paused;
            if (g_paused) {
                swprintf_s(g_textLine1, 256, L"[일시정지]");
                swprintf_s(g_textLine2, 256, L"색이 변한 벽돌 수 : %d개", g_colorChangedCount);
                swprintf_s(g_textLine3, 256, L"없어진 벽돌 수   : %d개", g_disappearedCount);
                g_showText = true;
            }
            else {
                g_showText = false;
            }
            InvalidateRect(hWnd, NULL, FALSE);
            break;

        case 'T': // 시간 출력
            if (g_gameStarted) {
                time_t now = time(nullptr);
                tm st, nt;
                localtime_s(&st, &g_startTime);
                localtime_s(&nt, &now);
                swprintf_s(g_textLine1, 256, L"시작 시간  : %02d:%02d:%02d", st.tm_hour, st.tm_min, st.tm_sec);
                swprintf_s(g_textLine2, 256, L"현재 시간  : %02d:%02d:%02d", nt.tm_hour, nt.tm_min, nt.tm_sec);
                swprintf_s(g_textLine3, 256, L"플레이 시간: %.0f 초", difftime(now, g_startTime));
                g_showText = true;
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;

        case VK_OEM_PLUS:  case VK_ADD:      // '+' 속도 증가
            g_ballDX = (g_ballDX > 0) ? g_ballDX + 2 : g_ballDX - 2;
            g_ballDY = (g_ballDY > 0) ? g_ballDY + 2 : g_ballDY - 2;
            break;

        case VK_OEM_MINUS: case VK_SUBTRACT: // '-' 속도 감소
            if (abs(g_ballDX) > 2) {
                g_ballDX = (g_ballDX > 0) ? g_ballDX - 2 : g_ballDX + 2;
                g_ballDY = (g_ballDY > 0) ? g_ballDY - 2 : g_ballDY + 2;
            }
            break;

        case 'R': ResetGame(hWnd);                    break;
        case 'Q': PostMessage(hWnd, WM_CLOSE, 0, 0); break;
        }
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rt; GetClientRect(hWnd, &rt);

        // 더블 버퍼링
        HDC     mdc = CreateCompatibleDC(hdc);
        HBITMAP hBmp = CreateCompatibleBitmap(hdc, rt.right, rt.bottom);
        HBITMAP oldBmp = (HBITMAP)SelectObject(mdc, hBmp);

        // 배경
        HBRUSH bgBr = CreateSolidBrush(RGB(50, 80, 150));
        FillRect(mdc, &rt, bgBr);
        DeleteObject(bgBr);

        // 블록
        for (int row = 0; row < ROWS; ++row) {
            for (int col = 0; col < COLS; ++col) {
                if (!g_blockVisible[row][col]) continue;
                RECT r; GetBlockRect(row, col, r);
                HBRUSH br = CreateSolidBrush(g_blockColor[row][col]);
                HBRUSH old = (HBRUSH)SelectObject(mdc, br);
                Rectangle(mdc, r.left, r.top, r.right, r.bottom);
                SelectObject(mdc, old);
                DeleteObject(br);
            }
        }

        // 바
        {
            RECT bar; GetBarRect(bar);
            HBRUSH br = CreateSolidBrush(RGB(255, 165, 0));
            HBRUSH old = (HBRUSH)SelectObject(mdc, br);
            Rectangle(mdc, bar.left, bar.top, bar.right, bar.bottom);
            SelectObject(mdc, old);
            DeleteObject(br);
        }

        // 공
        {
            HBRUSH br = CreateSolidBrush(RGB(30, 30, 220));
            HBRUSH old = (HBRUSH)SelectObject(mdc, br);
            Ellipse(mdc, g_ballCX - BALL_R, g_ballCY - BALL_R,
                g_ballCX + BALL_R, g_ballCY + BALL_R);
            SelectObject(mdc, old);
            DeleteObject(br);
        }

        // 텍스트 (P, T 키)
        if (g_showText) {
            SetBkMode(mdc, TRANSPARENT);
            SetTextColor(mdc, RGB(255, 255, 255));
            TextOutW(mdc, 20, rt.bottom - 80, g_textLine1, wcslen(g_textLine1));
            TextOutW(mdc, 20, rt.bottom - 60, g_textLine2, wcslen(g_textLine2));
            TextOutW(mdc, 20, rt.bottom - 40, g_textLine3, wcslen(g_textLine3));
        }

        BitBlt(hdc, 0, 0, rt.right, rt.bottom, mdc, 0, 0, SRCCOPY);
        SelectObject(mdc, oldBmp);
        DeleteObject(hBmp);
        DeleteDC(mdc);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hWnd, TIMER_BLOCK);
        KillTimer(hWnd, TIMER_BALL);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}