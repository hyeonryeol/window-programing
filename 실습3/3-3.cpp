#include <windows.h>
#include <cstdlib>
#include <ctime>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";
HBRUSH rectbrush;
HBRUSH playerbrush;
HBRUSH itembrush;
HBRUSH tailbrush;

int position = 0;
int ip[20];
int r[20];
int g[20];
int b[20];
int tailr[20];
int tailg[20];
int tailb[20];
int tailip[20];
int g_rect[1600][4];
int g_player[1600][4];
int g_tail[1600][4];
int itemcount = 20;
int movetailwon = 0;

int tailx[20] = {};
int taildx[20];
int taily[20] = {};
int taildy[20];
int tailxy[20];  
// 플레이어 이동 방향 (처음엔 오른쪽)
int pdx = 1;  // x 방향: -1 왼쪽, 1 오른쪽, 0 정지
int pdy = 0;  // y 방향: -1 위, 1 아래, 0 정지

void makerectangle()
{
    int w = 20, h = 20;
    for (int i = 0; i < 40; ++i)
        for (int j = 0; j < 40; ++j)
        {
            int count = i * 40 + j;
            g_rect[count][0] = i * w + 30;
            g_rect[count][1] = j * h + 30;
            g_rect[count][2] = g_rect[count][0] + w;
            g_rect[count][3] = g_rect[count][1] + h;
        }
}
void player()
{
    int w = 20, h = 20;
    for (int i = 0; i < 40; ++i)
        for (int j = 0; j < 40; ++j)
        {
            int count = i * 40 + j;
            g_player[count][0] = i * w + 30;
            g_player[count][1] = j * h + 30;
            g_player[count][2] = g_player[count][0] + w;
            g_player[count][3] = g_player[count][1] + h;
        }
}
void tailwon()
{
    int w = 20, h = 20;
    for (int i = 0; i < 40; ++i)
        for (int j = 0; j < 40; ++j)
        {
            int count = i * 40 + j;
            g_tail[count][0] = i * w + 30;
            g_tail[count][1] = j * h + 30;
            g_tail[count][2] = g_tail[count][0] + w;
            g_tail[count][3] = g_tail[count][1] + h;
        }
}

void CheckItemCollision(HWND hWnd)
{
    taildx[movetailwon] = 1;
    taildy[movetailwon] = 1;
    for (int i = 0; i < itemcount; ++i)
    {
        if (g_player[position][0] == g_rect[ip[i]][0] && g_player[position][1] == g_rect[ip[i]][1])
        {
            DeleteObject(playerbrush);
            playerbrush = CreateSolidBrush(RGB(r[i], g[i], b[i]));
            tailr[movetailwon] = r[i];
            tailg[movetailwon] = g[i];
            tailb[movetailwon] = b[i];
            tailip[movetailwon] = rand() % 1600;
            tailx[movetailwon] = 0;
            taildx[movetailwon] = 1;

            for (int j = i; j < itemcount; ++j)
            {
                ip[j] = ip[j + 1];
                r[j] = r[j + 1];
                g[j] = g[j + 1];
                b[j] = b[j + 1];
            }
            movetailwon++;
            itemcount--;
            i = -1;
        }
    }
    tailxy[movetailwon] = rand() % 4 + 1; 
}

// 플레이어 자동이동 타이머
void CALLBACK PlayerTimerProc(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime)
{
    g_player[position][0] += pdx * 20;
    g_player[position][2] += pdx * 20;
    g_player[position][1] += pdy * 20;
    g_player[position][3] += pdy * 20;

    // 벽 통과
    if (g_player[position][0] > 810) { g_player[position][0] = 30;  g_player[position][2] = 50; }
    if (g_player[position][0] < 30) { g_player[position][0] = 810; g_player[position][2] = 830; }
    if (g_player[position][1] > 810) { g_player[position][1] = 30;  g_player[position][3] = 50; }
    if (g_player[position][1] < 30) { g_player[position][1] = 810; g_player[position][3] = 830; }

    CheckItemCollision(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);
}

// 꼬리 자동이동 타이머
void CALLBACK TailTimerProc(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime)
{
    for (int i = 0; i < movetailwon; ++i)
    {
        if (tailxy[i] == 1)
        {
            tailx[i] += taildx[i] * 20;
            int curLeft = g_tail[tailip[i]][0] + tailx[i];
            int curRight = g_tail[tailip[i]][2] + tailx[i];
            if (curRight >= 830) { tailx[i] = 830 - g_tail[tailip[i]][2]; taildx[i] = -1; }
            if (curLeft <= 30) { tailx[i] = 30 - g_tail[tailip[i]][0]; taildx[i] = 1; }
        }
        else if(tailxy[i] == 2)
        {
            taily[i] += taildy[i] * 20;
            int curTop = g_tail[tailip[i]][1] + taily[i];
            int curBottom = g_tail[tailip[i]][3] + taily[i];
            if (curBottom >= 810) { taily[i] = 810 - g_tail[tailip[i]][3]; taildy[i] = -1; }
            if (curTop <= 30) { taily[i] = 30 - g_tail[tailip[i]][1]; taildy[i] = 1; }
        }
        else if (tailxy[i] == 3)
        {
            
        }
        else if (tailxy[i] == 4)
        {
            g_tail[tailip[i]][0] + tailx[i] + 20;
            g_tail[tailip[i]][1] + taily[i] + 20;
            g_tail[tailip[i]][3] + tailx[i] + 20;
            g_tail[tailip[i]][4] + taily[i] + 20;
        }
    }
   
    InvalidateRect(hWnd, NULL, TRUE);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
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

    HWND hWnd = CreateWindowExW(0, CLASS_NAME, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 1000, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) return 0;

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
    {
        srand((unsigned)time(NULL));
        position = rand() % 1600;
        for (int i = 0; i < itemcount; ++i)
            ip[i] = rand() % 1600;
        player();
        makerectangle();
        tailwon();
        for (int i = 0; i < itemcount; ++i)
        {
            r[i] = rand() % 255;
            g[i] = rand() % 255;
            b[i] = rand() % 255;
        }
        rectbrush = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
        playerbrush = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));

        SetTimer(hWnd, 1, 300, (TIMERPROC)PlayerTimerProc);  // 플레이어 자동이동
        SetTimer(hWnd, 2, 500, (TIMERPROC)TailTimerProc);    // 꼬리 자동이동
        return 0;
    }
    case WM_KEYDOWN:
    {
        // 방향만 바꾸기
        if (wParam == VK_LEFT) { pdx = -1; pdy = 0; }
        if (wParam == VK_RIGHT) { pdx = 1; pdy = 0; }
        if (wParam == VK_UP) { pdx = 0; pdy = -1; }
        if (wParam == VK_DOWN) { pdx = 0; pdy = 1; }
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, rectbrush);
        for (int i = 0; i < 1600; ++i)
            Rectangle(hdc, g_rect[i][0], g_rect[i][1], g_rect[i][2], g_rect[i][3]);
        SelectObject(hdc, oldbrush);

        oldbrush = (HBRUSH)SelectObject(hdc, playerbrush);
        Ellipse(hdc, g_player[position][0], g_player[position][1], g_player[position][2], g_player[position][3]);
        SelectObject(hdc, oldbrush);

        for (int i = 0; i < itemcount; ++i)
        {
            itembrush = (HBRUSH)CreateSolidBrush(RGB(r[i], g[i], b[i]));
            oldbrush = (HBRUSH)SelectObject(hdc, itembrush);
            Rectangle(hdc, g_rect[ip[i]][0] + 5, g_rect[ip[i]][1] + 5, g_rect[ip[i]][0] + 15, g_rect[ip[i]][1] + 15);
            DeleteObject(itembrush);
        }
        SelectObject(hdc, oldbrush);

        for (int i = 0; i < movetailwon; ++i)
        {
            tailbrush = (HBRUSH)CreateSolidBrush(RGB(tailr[i], tailg[i], tailb[i]));
            oldbrush = (HBRUSH)SelectObject(hdc, tailbrush);
            Ellipse(hdc,
                g_tail[tailip[i]][0] + tailx[i], g_tail[tailip[i]][1] + taily[i],
                g_tail[tailip[i]][2] + tailx[i], g_tail[tailip[i]][3] + taily[i]);
            DeleteObject(tailbrush);
        }
        SelectObject(hdc, oldbrush);

        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        DeleteObject(playerbrush);
        DeleteObject(rectbrush);
        KillTimer(hWnd, 1);
        KillTimer(hWnd, 2);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}