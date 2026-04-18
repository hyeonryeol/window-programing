#include <windows.h>

// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 클래스 이름 / 창 제목
const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

int g_rect[900][4];
int g_player[900][4];
HBRUSH boardbrush;
HBRUSH playerbrush;
int px = 0;
int py = 0;
void makeboard()
{
    int w = 30;
    int h = 30;
    for (int i = 0; i < 30; ++i)
    {
        for (int j = 0; j < 30; ++j)
        {
            int count = i * 30 + j;
            g_rect[count][0] = i * w + 30;
            g_rect[count][1] = j * h + 30;
            g_rect[count][2] = g_rect[count][0] + w;
            g_rect[count][3] = g_rect[count][1] + h;

        }
    }
}
void player()
{
    int w = 30;
    int h = 30;
    for (int i = 0; i < 30; ++i)
    {
        for (int j = 0; j < 30; ++j)
        {
            int count = i * 30 + j;
            g_player[count][0] = i * w + 30;
            g_player[count][1] = j * h + 30;
            g_player[count][2] = g_player[count][0] + w;
            g_player[count][3] = g_player[count][1] + h;

        }
    }
}
void CALLBACK PlayerTimerProc(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime)
{
   
    py += 30;
    InvalidateRect(hWnd, NULL, TRUE);
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
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 1000,
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
    switch (msg)
    {
    case WM_CREATE:
    {
        playerbrush = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
        boardbrush = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
        makeboard();
        player();
        SetTimer(hWnd, 1, 500, (TIMERPROC)PlayerTimerProc);
        return 0;

    }
    case WM_KEYDOWN:
    {
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, boardbrush);
        for (int i = 0;i < 900; ++i)
        {
            Rectangle(hdc, g_rect[i][0], g_rect[i][1], g_rect[i][2], g_rect[i][3]);
        }
        SelectObject(hdc, oldbrush);
        oldbrush = (HBRUSH)SelectObject(hdc, playerbrush);
        Ellipse(hdc, g_player[450][0] + px, g_player[450][1]+py, g_player[450][2]+px, g_player[450][3]+py);
        SelectObject(hdc, oldbrush);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        DeleteObject(boardbrush);
        DeleteObject(playerbrush);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}