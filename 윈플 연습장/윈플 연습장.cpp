#include <windows.h>
#include <time.h>
// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 클래스 이름 / 창 제목
const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";
int g_rect[900][4] = {};
HBRUSH boardbrush;
HBRUSH playerbrush;
HBRUSH objectbrush;
int p = 0;
int movex = 0;
int movey = 0;
int move = 0;
int objectp[15] = {};
int r[15] = {};
int g[15] = {};
int b[15] = {};
void makeboard()
{
    int w = 20;
    int h = 20;
    for (int i = 0; i < 30; ++i)
    {
        for (int j = 0; j < 30; ++j)
        {
            int c = i * 30 + j;
            g_rect[c][0] = i * w + 20;
            g_rect[c][1] = j * h + 20;
            g_rect[c][2] = g_rect[c][0] + w;
            g_rect[c][3] = g_rect[c][1] + h;
        }
    }
}


void CALLBACK TimerProc1(HWND hWnd, UINT iMsg,
    UINT idEvent, DWORD dwTime)
{
    movey += 20;
    if (g_rect[p][3] + movey == 640)
    {
        movey = 0;
    }
    
    InvalidateRect(hWnd, NULL, FALSE);
}
// 프로그램 시작점
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    // 1) 윈도우 클래스 등록
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    RegisterClassExW(&wc);

    // 2) 윈도우 생성
    HWND hWnd = CreateWindowExW(
        0,
        CLASS_NAME,
        WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
        nullptr, nullptr, hInstance, nullptr
    );

    if (!hWnd) return 0;

    // 3) 창 표시
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 4) 메시지 루프
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// 윈도우 메시지 처리 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc, mDC;
    HBITMAP hBitmap;
    RECT rt;
    GetClientRect(hWnd, &rt);
    switch (msg)
    {
    case WM_CREATE:
    {
        srand((unsigned)time(NULL));
        makeboard();
        for (int i = 0; i < 15; ++i)
        {
        objectp[i] = rand() % 900;
        r[i] = rand() % 255;
        b[i] = rand() % 255;
        b[i] = rand() % 255;
        
        }
        p = 450;
        boardbrush = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
        playerbrush = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
        SetTimer(hWnd, 1, 500, (TIMERPROC)TimerProc1);
        return 0;
    }
    case WM_KEYDOWN:
    {
        if (wParam == VK_LEFT)
        {
            if (g_rect[p][2] + movex <= 620 && g_rect[p][0] + movex > 20)
            {
                movex -= 20;
            }
        }
        if (wParam == VK_RIGHT)
        {
            if (g_rect[p][2] + movex < 620 && g_rect[p][0] + movex >= 0)
            {
                movex += 20;
            }
        }
        InvalidateRect(hWnd, NULL, FALSE);
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        mDC = CreateCompatibleDC(hdc);
        hBitmap = CreateCompatibleBitmap(hdc, rt.right, rt.bottom);
        SelectObject(mDC, (HBITMAP)hBitmap);
        Rectangle(mDC, 0, 0, rt.right, rt.bottom);
        // TODO: 여기에 그리기 코드 작성
        HBRUSH oldbrush = (HBRUSH)SelectObject(mDC, boardbrush);
        for (int i = 0; i < 900; ++i)
        {
            Rectangle(mDC, g_rect[i][0], g_rect[i][1], g_rect[i][2], g_rect[i][3]);
        }
        SelectObject(mDC, oldbrush);
        oldbrush = (HBRUSH)SelectObject(mDC, playerbrush);
        Ellipse(mDC, g_rect[p][0]+movex, g_rect[p][1]+movey, g_rect[p][2]+movex, g_rect[p][3]+movey);
        SelectObject(mDC, oldbrush);
        for (int i = 0; i < 15; ++i)
        {
            objectbrush = (HBRUSH)CreateSolidBrush(RGB(r[i], g[i], b[i]));
            oldbrush = (HBRUSH)SelectObject(mDC, objectbrush);
            Rectangle(mDC, g_rect[objectp[i]][0], g_rect[objectp[i]][1], g_rect[objectp[i]][2], g_rect[objectp[i]][3]);
            DeleteObject(objectbrush);
        }
        SelectObject(mDC, oldbrush);
        //--- 마지막에 메모리 DC의 내용을화면DC로복사한다.
        BitBlt(hdc, 0, 0, rt.right, rt.bottom, mDC, 0, 0, SRCCOPY);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        DeleteObject(boardbrush);
        DeleteObject(playerbrush);
        DeleteObject(objectbrush);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}