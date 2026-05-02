#include <windows.h>

#define ID_MENU_HBLUE_VRED  1001
#define ID_MENU_HRED_VBLUE  1002
#define ID_MENU_AUTO        1003
#define ID_MENU_STOP        1004
#define ID_MENU_QUIT        1005

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

HPEN   linepen, dotpen, signalboxpen, dotlinepen;
HBRUSH signalred, signalyellow, signalblue, carbrush, peoplebrush;

int speed1 = 250;
int carx[10] = {};
int cary[10] = {};

bool redlight = false;
bool yellowlight = false;
bool bluelight = true;

bool redlight2 = true;
bool yellowlight2 = false;
bool bluelight2 = false;

bool stopped[10] = {};
int  stoppedY[10] = {};
int  stoppedX[10] = {};

// 사람 상태
int  personX = 0, personY = 0;
int  personBaseX = 0, personBaseY = 0;
bool personWalking = false;
bool personReturning = false;
bool waitForPerson = false;

bool autoMode = false;

void CALLBACK TimerProc1(HWND, UINT, UINT, DWORD);
void CALLBACK TimerProc2(HWND, UINT, UINT, DWORD);
void CALLBACK TimerProc3(HWND, UINT, UINT, DWORD);

// 세로 차 그리기 (랩어라운드)
void DrawCarV(HDC hdc, int left, int right, int top, int screenH)
{
    int bottom = top + 100;
    if (top <= 0) {
        Rectangle(hdc, left, screenH + top, right, screenH);
        if (bottom >= 0) Rectangle(hdc, left, 0, right, bottom);
    }
    else if (bottom >= screenH) {
        Rectangle(hdc, left, top, right, screenH);
        Rectangle(hdc, left, 0, right, bottom - screenH);
    }
    else {
        Rectangle(hdc, left, top, right, bottom);
    }
}

// 가로 차 그리기 (랩어라운드)
void DrawCarV1(HDC hdc, int screenW, int right, int top, int bottom)
{
    int left = right - 100;
    if (right <= 0) return;
    else if (left <= 0) {
        Rectangle(hdc, 0, top, right, bottom);
        Rectangle(hdc, screenW + left, top, screenW, bottom);
    }
    else {
        Rectangle(hdc, left, top, right, bottom);
    }
}

// 세로=파랑, 가로=빨강
void SetSignal_VBlue_HRed()
{
    redlight = false; yellowlight = false; bluelight = true;
    redlight2 = true; yellowlight2 = false; bluelight2 = false;
    for (int i = 6; i <= 9; i++) { stopped[i] = false; stoppedX[i] = 0; }
}

// 세로=빨강, 가로=파랑
void SetSignal_VRed_HBlue()
{
    redlight = true; yellowlight = false; bluelight = false;
    redlight2 = false; yellowlight2 = false; bluelight2 = true;
    for (int i = 1; i <= 4; i++) { stopped[i] = false; stoppedY[i] = 0; }
}

// 노랑 후 신호 전환
void CALLBACK TimerProc2(HWND hWnd, UINT, UINT, DWORD)
{
    if (redlight)       SetSignal_VBlue_HRed();
    else if (bluelight) SetSignal_VRed_HBlue();
    KillTimer(hWnd, 2);
    if (autoMode) SetTimer(hWnd, 3, 5000, (TIMERPROC)TimerProc3);
}

// 노랑 시작
void CALLBACK TimerProc3(HWND hWnd, UINT, UINT, DWORD)
{
    yellowlight = true; yellowlight2 = true;
    KillTimer(hWnd, 3);
    SetTimer(hWnd, 2, 1000, (TIMERPROC)TimerProc2);
}

// 메인 타이머: 차 및 사람 이동
void CALLBACK TimerProc1(HWND hWnd, UINT, UINT, DWORD)
{
    RECT rc; GetClientRect(hWnd, &rc);
    int H = rc.bottom, L = rc.right;
    int X = L / 3, Y = H / 3;

    bool allStoppedV = stopped[1] && stopped[2] && stopped[3] && stopped[4];
    bool allStoppedH = stopped[6] && stopped[7] && stopped[8] && stopped[9];

    // 사람 이동
    if (personWalking)
    {
        if (allStoppedV && allStoppedH)
        {
            // 모든 차 멈춤 → 대각선 이동
            personX += 5; personY -= 5;
            if (personX >= X + 40)
            {
                personBaseX += personX; personBaseY += personY;
                personX = 0; personY = 0;
                personWalking = false;
                personReturning = true;
                if (waitForPerson) { waitForPerson = false; SetSignal_VRed_HBlue(); }
            }
        }
        else
        {
            // 좌우 차만 멈춤 → 위로 이동
            personY -= 5;
            if (personY <= -Y)
            {
                personBaseX += personX; personBaseY += personY;
                personX = 0; personY = 0;
                personWalking = false;
                personReturning = true;
                if (waitForPerson) { waitForPerson = false; SetSignal_VRed_HBlue(); }
            }
        }
    }
    else if (personReturning)
    {
        // 원래 자리로 복귀
        if (personBaseX > 0) personBaseX -= 5;
        if (personBaseX < 0) personBaseX += 5;
        if (personBaseY < 0) personBaseY += 5;
        if (personBaseY > 0) personBaseY -= 5;
        if (personBaseX == 0 && personBaseY == 0)
            personReturning = false;
    }
    else
    {
        // 신호 조건 충족 시 이동 시작
        if (redlight && allStoppedV) personWalking = true;
        if (redlight2 && allStoppedH) personWalking = true;
    }

    // 파란불: 세로 차 이동
    if (bluelight)
    {
        cary[1] -= 10; cary[2] -= 10;
        cary[3] += 5; cary[4] += 10;
    }

    // 파란불2: 가로 차 이동
    if (bluelight2)
    {
        carx[6] -= 10; carx[7] -= 10;
        carx[8] += 10; carx[9] += 10;
    }

    // 빨강: 세로 차 정지
    if (redlight)
    {
        if (!stopped[1]) {
            int myStop = (stopped[2] && stoppedY[2] == 510) ? 630 : 510;
            int top1 = 50 + cary[1];
            if (top1 > myStop && top1 - 10 <= myStop) { cary[1] = myStop - 50;  stopped[1] = true; stoppedY[1] = myStop; }
            else cary[1] -= 10;
        }
        if (!stopped[2]) {
            int myStop = (stopped[1] && stoppedY[1] == 510) ? 630 : 510;
            int top2 = 250 + cary[2];
            if (top2 > myStop && top2 - 10 <= myStop) { cary[2] = myStop - 250; stopped[2] = true; stoppedY[2] = myStop; }
            else cary[2] -= 10;
        }
        if (!stopped[3]) {
            int myStop = (stopped[4] && stoppedY[4] == 250) ? 130 : 250;
            int bottom3 = 450 + cary[3];
            if (bottom3 <= myStop && bottom3 + 10 >= myStop) { cary[3] = myStop - 450; stopped[3] = true; stoppedY[3] = myStop; }
            else cary[3] += 10;
        }
        if (!stopped[4]) {
            int myStop = (stopped[3] && stoppedY[3] == 250) ? 130 : 250;
            int bottom4 = 650 + cary[4];
            if (bottom4 <= myStop && bottom4 + 10 >= myStop) { cary[4] = myStop - 650; stopped[4] = true; stoppedY[4] = myStop; }
            else cary[4] += 10;
        }
    }

    // 빨강2: 가로 차 정지
    if (redlight2)
    {
        if (!stopped[6]) {
            int myStop = (stopped[7] && stoppedX[7] == 630) ? 740 : 630;
            int right6 = 150 + carx[6];
            if (right6 <= myStop && right6 + 10 >= myStop) { carx[6] = myStop - 150; stopped[6] = true; stoppedX[6] = myStop; }
            else carx[6] -= 10;
        }
        if (!stopped[7]) {
            int myStop = (stopped[6] && stoppedX[6] == 630) ? 740 : 630;
            int right7 = 650 + carx[7];
            if (right7 <= myStop && right7 + 10 >= myStop) { carx[7] = myStop - 650; stopped[7] = true; stoppedX[7] = myStop; }
            else carx[7] -= 10;
        }
        if (!stopped[8]) {
            int myStop = (stopped[9] && stoppedX[9] == 200) ? 70 : 200;
            int right8 = 150 + carx[8];
            if (right8 >= myStop && right8 - 10 <= myStop) { carx[8] = myStop - 150; stopped[8] = true; stoppedX[8] = myStop; }
            else carx[8] += 10;
        }
        if (!stopped[9]) {
            int myStop = (stopped[8] && stoppedX[8] == 200) ? 70 : 200;
            int right9 = 650 + carx[9];
            if (right9 >= myStop && right9 - 10 <= myStop) { carx[9] = myStop - 650; stopped[9] = true; stoppedX[9] = myStop; }
            else carx[9] += 10;
        }
    }

    // 랩어라운드
    if (150 + cary[1] < 0) { cary[1] += H; stopped[1] = false; stoppedY[1] = 0; }
    if (350 + cary[2] < 0) { cary[2] += H; stopped[2] = false; stoppedY[2] = 0; }
    if (350 + cary[3] > H) { cary[3] -= H; stopped[3] = false; stoppedY[3] = 0; }
    if (550 + cary[4] > H) { cary[4] -= H; stopped[4] = false; stoppedY[4] = 0; }
    if (150 + carx[6] < 0) { carx[6] += L; stopped[6] = false; stoppedX[6] = 0; }
    if (650 + carx[7] < 0) { carx[7] += L; stopped[7] = false; stoppedX[7] = 0; }
    if (150 + carx[8] > L) { carx[8] -= L; stopped[8] = false; stoppedX[8] = 0; }
    if (650 + carx[9] > L) { carx[9] -= L; stopped[9] = false; stoppedX[9] = 0; }

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

    HMENU hMenu = CreateMenu();
    HMENU hSubMenu = CreatePopupMenu();
    AppendMenuW(hSubMenu, MF_STRING, ID_MENU_HBLUE_VRED, L"H-blue, V-red");
    AppendMenuW(hSubMenu, MF_STRING, ID_MENU_HRED_VBLUE, L"H-red, V-blue");
    AppendMenuW(hSubMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hSubMenu, MF_STRING, ID_MENU_AUTO, L"Auto");
    AppendMenuW(hSubMenu, MF_STRING, ID_MENU_STOP, L"Stop");
    AppendMenuW(hSubMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hSubMenu, MF_STRING, ID_MENU_QUIT, L"Quit");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"신호등");

    HWND hWnd = CreateWindowExW(0, CLASS_NAME, WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 800,
        nullptr, hMenu, hInstance, nullptr);
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
    RECT clientRect; GetClientRect(hWnd, &clientRect);
    int x = clientRect.right / 3;
    int y = clientRect.bottom / 3;
    int x1 = clientRect.right / 2;
    int y1 = clientRect.bottom / 2;

    switch (msg)
    {
    case WM_CREATE:
        linepen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        dotpen = CreatePen(PS_DASH, 1, RGB(0, 0, 0));
        signalboxpen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        dotlinepen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
        signalred = CreateSolidBrush(RGB(255, 0, 0));
        signalyellow = CreateSolidBrush(RGB(255, 255, 0));
        signalblue = CreateSolidBrush(RGB(0, 0, 255));
        carbrush = CreateSolidBrush(RGB(0, 0, 255));
        peoplebrush = CreateSolidBrush(RGB(255, 255, 50));
        SetTimer(hWnd, 1, speed1, (TIMERPROC)TimerProc1);
        return 0;

    case WM_LBUTTONDOWN:
    {
        int mx = LOWORD(lParam), my = HIWORD(lParam);
        bool clickedSignal = false;

        // 세로 신호등 클릭
        if (55 < mx && mx < 95 && 5 < my && my < 45) {
            redlight = true; yellowlight = false; bluelight = false;
            redlight2 = false; yellowlight2 = false; bluelight2 = true;
            for (int i = 6; i <= 9; i++) { stopped[i] = false; stoppedX[i] = 0; }
            clickedSignal = true;
        }
        if (105 < mx && mx < 145 && 5 < my && my < 45) {
            yellowlight = true; yellowlight2 = true;
            SetTimer(hWnd, 2, 1000, (TIMERPROC)TimerProc2);
            clickedSignal = true;
        }
        if (155 < mx && mx < 195 && 5 < my && my < 45) {
            redlight = false; yellowlight = false; bluelight = true;
            redlight2 = true; yellowlight2 = false; bluelight2 = false;
            for (int i = 1; i <= 4; i++) { stopped[i] = false; stoppedY[i] = 0; }
            clickedSignal = true;
        }

        // 가로 신호등 클릭
        if (555 < mx && mx < 595 && 555 < my && my < 595) {
            redlight2 = true; yellowlight2 = false; bluelight2 = false;
            redlight = false; yellowlight = false; bluelight = true;
            for (int i = 1; i <= 4; i++) { stopped[i] = false; stoppedY[i] = 0; }
            clickedSignal = true;
        }
        if (605 < mx && mx < 645 && 555 < my && my < 595) {
            yellowlight2 = true; yellowlight = true;
            SetTimer(hWnd, 2, 1000, (TIMERPROC)TimerProc2);
            clickedSignal = true;
        }
        if (655 < mx && mx < 695 && 555 < my && my < 595) {
            redlight2 = false; yellowlight2 = false; bluelight2 = true;
            redlight = true; yellowlight = false; bluelight = false;
            for (int i = 6; i <= 9; i++) { stopped[i] = false; stoppedX[i] = 0; }
            clickedSignal = true;
        }

        // 신호등 밖 클릭 → 전체 정지
        if (!clickedSignal) {
            redlight = true; yellowlight = false; bluelight = false;
            redlight2 = true; yellowlight2 = false; bluelight2 = false;
        }
        return 0;
    }

    case WM_RBUTTONDOWN:
        // 오른쪽 클릭: 사람 이동 중이면 완료 후 재개, 아니면 즉시 재개
        if (personWalking) waitForPerson = true;
        else               SetSignal_VRed_HBlue();
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_MENU_HBLUE_VRED: SetSignal_VRed_HBlue(); break;
        case ID_MENU_HRED_VBLUE: SetSignal_VBlue_HRed(); break;
        case ID_MENU_AUTO:
            autoMode = true;
            KillTimer(hWnd, 3);
            SetTimer(hWnd, 3, 5000, (TIMERPROC)TimerProc3);
            break;
        case ID_MENU_STOP:
            autoMode = false;
            KillTimer(hWnd, 3);
            break;
        case ID_MENU_QUIT: PostQuitMessage(0); break;
        }
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_OEM_PLUS && speed1 > 50) {
            KillTimer(hWnd, 1); speed1 -= 50;
            SetTimer(hWnd, 1, speed1, (TIMERPROC)TimerProc1);
        }
        if (wParam == VK_OEM_MINUS) {
            KillTimer(hWnd, 1); speed1 += 50;
            SetTimer(hWnd, 1, speed1, (TIMERPROC)TimerProc1);
        }
        if (wParam == 'A') {
            if (!autoMode) { autoMode = true;  SetTimer(hWnd, 3, 5000, (TIMERPROC)TimerProc3); }
            else { autoMode = false; KillTimer(hWnd, 3); }
        }
        if (wParam == 'Q') PostQuitMessage(0);
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

        // 신호등 원
        HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, signalred);
        if (redlight)  Ellipse(hdc, 55, 5, 95, 45);
        if (redlight2) Ellipse(hdc, 555, 555, 595, 595);
        SelectObject(hdc, oldbrush);

        oldbrush = (HBRUSH)SelectObject(hdc, signalyellow);
        if (yellowlight)  Ellipse(hdc, 105, 5, 145, 45);
        if (yellowlight2) Ellipse(hdc, 605, 555, 645, 595);
        SelectObject(hdc, oldbrush);

        oldbrush = (HBRUSH)SelectObject(hdc, signalblue);
        if (bluelight)  Ellipse(hdc, 155, 5, 195, 45);
        if (bluelight2) Ellipse(hdc, 655, 555, 695, 595);
        SelectObject(hdc, oldbrush);

        // 자동차
        {
            int H = clientRect.bottom, R = clientRect.right;
            oldbrush = (HBRUSH)SelectObject(hdc, carbrush);
            DrawCarV(hdc, 405, 460, 50 + cary[1], H);
            DrawCarV(hdc, 405, 460, 250 + cary[2], H);
            DrawCarV(hdc, 320, 375, 350 + cary[3], H);
            DrawCarV(hdc, 320, 375, 550 + cary[4], H);
            DrawCarV1(hdc, R, 150 + carx[6], 290, 350);
            DrawCarV1(hdc, R, 650 + carx[7], 290, 350);
            DrawCarV1(hdc, R, 150 + carx[8], 405, 465);
            DrawCarV1(hdc, R, 650 + carx[9], 405, 465);
            SelectObject(hdc, oldbrush);
        }

        // 사람
        oldbrush = (HBRUSH)SelectObject(hdc, peoplebrush);
        Ellipse(hdc,
            x - 40 + personBaseX + personX, 2 * y + personBaseY + personY,
            x + personBaseX + personX, 2 * y + 40 + personBaseY + personY);
        SelectObject(hdc, oldbrush);

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hWnd, 1); KillTimer(hWnd, 2); KillTimer(hWnd, 3);
        DeleteObject(linepen); DeleteObject(dotpen);
        DeleteObject(signalboxpen); DeleteObject(dotlinepen);
        DeleteObject(signalred); DeleteObject(signalyellow);
        DeleteObject(signalblue); DeleteObject(carbrush);
        DeleteObject(peoplebrush);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}