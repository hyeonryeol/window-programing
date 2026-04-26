#include <windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

HPEN   linepen, dotpen, signalboxpen, dotlinepen;
HBRUSH signalred, signalyellow, signalblue, carbrush;
int speed1 = 250;
int  carx[10] = {};
int  cary[10] = {};
static int mx, my;

bool redlight = false;
bool yellowlight = false;
bool bluelight = true;

bool stopped[10] = {};  // 1, 2
int  stoppedY[10] = {};  // 각 차가 멈춘 y 위치

// ── 차 그리기 (랩어라운드 처리) ──────────────────────────────
void DrawCarV(HDC hdc, int left, int right, int top, int screenH)
{
    int bottom = top + 100;
    
    if (top <= 0)
    {
        // 화면 아래쪽에 남은 조각
        Rectangle(hdc, left, screenH + top, right, screenH);
        // 화면 위쪽에 걸친 조각
        if (bottom >= 0)
            Rectangle(hdc, left, 0, right, bottom);
     
    }
    else if (bottom >= screenH)
    {
        Rectangle(hdc, left, top, right, screenH);
        Rectangle(hdc, left, 0, right, bottom - screenH);
    }
    else
    {
        Rectangle(hdc, left, top, right, bottom);
    }
}

// ── 타이머 ────────────────────────────────────────────────────
void CALLBACK TimerProc1(HWND hWnd, UINT, UINT, DWORD)
{
    RECT rc;
    GetClientRect(hWnd, &rc);
    int H = rc.bottom;
    int T = rc.top;
    // 1) 이동
    if (bluelight)
    {
        cary[1] -= 10;
        cary[2] -= 10;
        cary[3] += 10;
        cary[4] += 10;
    }

    if (redlight)
    {
        // ── 차1 처리 ──
        if (!stopped[1])
        {
            // 차2가 400에 서 있으면 차1은 510에 서야 함
            int myStop1 = (stopped[2] && stoppedY[2] == 510) ? 620 : 510;
            int top1 = 50 + cary[1];

            if (top1 > myStop1 && top1 - 10 <= myStop1)
            {
                // 정지선에 맞춰 세우기
                cary[1] = myStop1 - 50;
                stopped[1] = true;
                stoppedY[1] = myStop1;
            }
            else
            {
                cary[1] -= 10; // 접근 중이거나 이미 지남(랩 대기)
            }
        }

        // ── 차2 처리 (차1이 방금 멈췄을 수 있으니 stopped[1] 재확인) ──
        if (!stopped[2])
        {
            int myStop2 = (stopped[1] && stoppedY[1] == 510) ? 620 : 510;
            int top2 = 250 + cary[2];

            if (top2 > myStop2 && top2 - 10 <= myStop2)
            {
                cary[2] = myStop2 - 250;
                stopped[2] = true;
                stoppedY[2] = myStop2;
            }
            else
            {
                cary[2] -= 10;
            }
        }
        ////////////////////////////////////////////////////////////////
        if (!stopped[3])
        {
     
            int myStop1 = (stopped[4] && stoppedY[4] == 250) ? 140 : 250;
            int bottom1 = 450 + cary[3];

            if (bottom1 < myStop1 && bottom1 + 10 >= myStop1)
            {
                // 정지선에 맞춰 세우기
                cary[3] = myStop1 - 450;
                stopped[3] = true;
                stoppedY[3] = myStop1;
            }
            else
            {
                cary[3] += 10; // 접근 중이거나 이미 지남(랩 대기)
            }
        }

   
        if (!stopped[4])
        {
            int myStop2 = (stopped[3] && stoppedY[3] == 250) ? 140 : 250;
            int bottom2 = 650 + cary[4];

            if (bottom2 < myStop2 && bottom2 + 10 >= myStop2)
            {
                cary[4] = myStop2 - 650;
                stopped[4] = true;
                stoppedY[4] = myStop2;
            }
            else
            {
                cary[4] += 10;
            }
        }
    }

    // 2) 랩어라운드: 이동 후에 체크
    if (150 + cary[1] < 0) { cary[1] += H ;  stopped[1] = false; stoppedY[1] = 0; }
    if (350 + cary[2] < 0) { cary[2] += H ; stopped[2] = false; stoppedY[2] = 0; }
    if (350 + cary[3] > H) { cary[3] -= H;  stopped[3] = false; stoppedY[3] = 0; }
    if (550 + cary[4] > H) { cary[4] -= H; stopped[4] = false; stoppedY[4] = 0; }
    InvalidateRect(hWnd, NULL, TRUE);
}

// ── WinMain ───────────────────────────────────────────────────
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

    HWND hWnd = CreateWindowExW(0, CLASS_NAME, WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
        nullptr, nullptr, hInstance, nullptr);
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

// ── 윈도우 프로시저 ───────────────────────────────────────────
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static BOOL Selection;
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    int x = clientRect.right / 3;
    int y = clientRect.bottom / 3;
    int x1 = clientRect.right / 2;
    int y1 = clientRect.bottom / 2;

    switch (msg)
    {
    case WM_CREATE:
    {
        linepen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        dotpen = CreatePen(PS_DASH, 1, RGB(0, 0, 0));
        signalboxpen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        dotlinepen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
        signalred = CreateSolidBrush(RGB(255, 0, 0));
        signalyellow = CreateSolidBrush(RGB(255, 255, 0));
        signalblue = CreateSolidBrush(RGB(0, 0, 255));
        carbrush = CreateSolidBrush(RGB(0, 0, 255));
        SetTimer(hWnd, 1, speed1, (TIMERPROC)TimerProc1);
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        mx = LOWORD(lParam);
        my = HIWORD(lParam);
        Selection = TRUE;

        // 빨간불
        if (55 < mx && mx < 95 && 5 < my && my < 45)
        {
            redlight = true; yellowlight = false; bluelight = false;
        }
        // 파란불
        if (155 < mx && mx < 195 && 5 < my && my < 45)
        {
            redlight = false; yellowlight = false; bluelight = true;
            stopped[1] = stopped[2] =  stopped[3] = stopped[4] =false;
            stoppedY[1] = stoppedY[2] = stoppedY[3] = stopped[4] = 0;
        }
        return 0;
    }

    case WM_KEYDOWN:
    {
        if (wParam == VK_OEM_PLUS)
        {
            if (speed1 > 200)
            {

            KillTimer(hWnd, 1);
            speed1 -= 200;
            SetTimer(hWnd, 1, speed1, (TIMERPROC)TimerProc1);
            }
        }
        if (wParam == VK_OEM_MINUS)
        {
            KillTimer(hWnd, 1);
            speed1 += 200;
            SetTimer(hWnd, 1, speed1, (TIMERPROC)TimerProc1);
        }
    }
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // 차선
        HPEN oldpen = (HPEN)SelectObject(hdc, linepen);
        MoveToEx(hdc, x, 0, NULL); LineTo(hdc, x, y);
        MoveToEx(hdc, x, 2 * y, NULL); LineTo(hdc, x, 3 * y);
        MoveToEx(hdc, 2 * x, 0, NULL); LineTo(hdc, 2 * x, y);
        MoveToEx(hdc, 2 * x, 2 * y, NULL); LineTo(hdc, 2 * x, 3 * y);
        MoveToEx(hdc, 0, y, NULL); LineTo(hdc, x, y);
        MoveToEx(hdc, 2 * x, y, NULL); LineTo(hdc, 3 * x, y);
        MoveToEx(hdc, 0, 2 * y, NULL); LineTo(hdc, x, 2 * y);
        MoveToEx(hdc, 2 * x, 2 * y, NULL); LineTo(hdc, 3 * x, 2 * y);
        SelectObject(hdc, oldpen);

        oldpen = (HPEN)SelectObject(hdc, dotlinepen);
        MoveToEx(hdc, x1, 0, NULL); LineTo(hdc, x1, 3 * y);
        MoveToEx(hdc, 0, y1, NULL); LineTo(hdc, 2 * x1, y1);
        SelectObject(hdc, oldpen);

        oldpen = (HPEN)SelectObject(hdc, dotpen);
        MoveToEx(hdc, x, y, NULL); LineTo(hdc, x, 2 * y);
        MoveToEx(hdc, 2 * x, y, NULL); LineTo(hdc, 2 * x, 2 * y);
        MoveToEx(hdc, x, y, NULL); LineTo(hdc, 2 * x, y);
        MoveToEx(hdc, x, 2 * y, NULL); LineTo(hdc, 2 * x, 2 * y);
        SelectObject(hdc, oldpen);

        // 신호등 박스
        oldpen = (HPEN)SelectObject(hdc, signalboxpen);
        Rectangle(hdc, 50, 0, 200, 50);
        Rectangle(hdc, 550, 550, 700, 600);
        SelectObject(hdc, oldpen);

        // 신호등
        HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, signalred);
        Ellipse(hdc, 55, 5, 95, 45);
        Ellipse(hdc, 555, 555, 595, 595);
        SelectObject(hdc, oldbrush);
        oldbrush = (HBRUSH)SelectObject(hdc, signalyellow);
        Ellipse(hdc, 105, 5, 145, 45);
        Ellipse(hdc, 605, 555, 645, 595);
        SelectObject(hdc, oldbrush);
        oldbrush = (HBRUSH)SelectObject(hdc, signalblue);
        Ellipse(hdc, 155, 5, 195, 45);
        Ellipse(hdc, 655, 555, 695, 595);
        SelectObject(hdc, oldbrush);

        // 자동차 (랩어라운드)
        {
            int H = clientRect.bottom;
          
            oldbrush = (HBRUSH)SelectObject(hdc, carbrush);
            DrawCarV(hdc, 405 + carx[1], 460 + carx[1], 50 + cary[1], H);
            DrawCarV(hdc, 405 + carx[2], 460 + carx[2], 250 + cary[2], H);
            DrawCarV(hdc, 320 + carx[3], 375 + carx[3], 350 + cary[3], H);
            DrawCarV(hdc, 320 + carx[4], 375 + carx[4], 550 + cary[4], H);
            SelectObject(hdc, oldbrush);
        }

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        DeleteObject(linepen);
        DeleteObject(dotpen);
        DeleteObject(signalboxpen);
        DeleteObject(dotlinepen);
        DeleteObject(signalred);
        DeleteObject(signalyellow);
        DeleteObject(signalblue);
        DeleteObject(carbrush);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}