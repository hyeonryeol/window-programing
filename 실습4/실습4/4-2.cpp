#include <windows.h>

// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 클래스 이름 / 창 제목
const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";
HPEN linepen;
HPEN dotpen;
HBRUSH signalboxbrush;
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
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
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
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    int x = clientRect.right / 3;
    int y = clientRect.bottom / 3;
    switch (msg)
    {
    case WM_CREATE:
    {
        linepen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
        dotpen = CreatePen(PS_DASH, 1, RGB(0, 0, 0)); // 점선
        signalboxbrush = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
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

        // TODO: 여기에 그리기 코드 작성
        HPEN oldpen = (HPEN)SelectObject(hdc, linepen);
        MoveToEx(hdc, x, 0, NULL);
        LineTo(hdc, x, y);
        MoveToEx(hdc, x, 2 * y, NULL);
        LineTo(hdc, x, 3 * y);

        MoveToEx(hdc, 2*x, 0, NULL);
        LineTo(hdc, 2*x, y);
        MoveToEx(hdc, 2 * x, 2*y, NULL);
        LineTo(hdc, 2 * x, 3*y);

        MoveToEx(hdc, 0, y, NULL);
        LineTo(hdc, x, y);
        MoveToEx(hdc, 2*x, y, NULL);
        LineTo(hdc, 3*x, y);

        MoveToEx(hdc, 0, 2*y, NULL);
        LineTo(hdc, x, 2*y);
        MoveToEx(hdc, 2*x, 2 * y, NULL);
        LineTo(hdc, 3*x, 2 * y);

        oldpen = (HPEN)SelectObject(hdc, dotpen);
        MoveToEx(hdc, x, y, NULL);
        LineTo(hdc, x, 2*y);
        MoveToEx(hdc, 2 * x, y, NULL);
        LineTo(hdc, 2*x, 2 * y);
        MoveToEx(hdc, x, y, NULL);
        LineTo(hdc, 2*x, y);
        MoveToEx(hdc, x, 2 * y, NULL);
        LineTo(hdc, 2*x, 2 * y);
        SelectObject(hdc, oldpen);
        HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, signalboxbrush);
        Rectangle(hdc, 150, 50, 200, 50);
        SelectObject(hdc, oldbrush);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        DeleteObject(linepen);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}