#include <windows.h>
#include <windowsx.h>
#include <atlimage.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Msimg32.lib")  // TransparentBlt 라이브러리

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define IMG_COUNT 3 

int g_frame = 0;
int g_frameTimer = 0;
CImage g_img[IMG_COUNT];
int g_x = 0;
int obx = 400, oby = 100;
int obw = 50, obh = 50;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = TEXT("MyWindow");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    HWND hWnd = CreateWindow(
        TEXT("MyWindow"), TEXT("CImage 애니메이션"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    static HDC     hMemDC;
    static HBITMAP hBitmap;
    RECT rect;
    GetClientRect(hWnd, &rect);
    int W = rect.right;
    int H = rect.bottom;

    switch (message) {
    case WM_CREATE:
    {
        g_img[0].Load(TEXT("goraniscream1.png"));
        g_img[1].Load(TEXT("goraniscream2.png"));
        g_img[2].Load(TEXT("goraniscream3.png"));
        SetTimer(hWnd, 1, 16, NULL);
        SetTimer(hWnd, 2, 500, NULL);  // ← 추가 (500ms마다 실행)
        HDC hdc = GetDC(hWnd);
        hMemDC = CreateCompatibleDC(hdc);
        hBitmap = CreateCompatibleBitmap(hdc, 800, 600);
        SelectObject(hMemDC, hBitmap);
        ReleaseDC(hWnd, hdc);
        return 0;
    }
    case WM_KEYDOWN:
    {
        switch (wParam) {
        case VK_LEFT:  break;
        case VK_RIGHT: break;
        case VK_UP:    break;
        case VK_DOWN:  break;
        }
        return 0;
    }
    case WM_TIMER:
    {
        if (wParam == 1)
        {

        g_x += 2;
        if (g_x > rect.right) g_x = 0;

        g_frameTimer++;
        if (g_frameTimer >= 10) {
            g_frameTimer = 0;
            g_frame = (g_frame + 1) % IMG_COUNT;
        }

        RECT rectA = { g_x, 100, g_x + 200, 100 + 200 };
        RECT rectB = { obx, oby, obx + obw, oby + obh };
        RECT rectResult;
        if (IntersectRect(&rectResult, &rectA, &rectB)) {
            g_x = 0;
        }
        else if (wParam == 2) {
            // ← 타이머 2 코드 여기에
        }

        InvalidateRect(hWnd, NULL, FALSE);
        return 0;
        }
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // 배경 지우기
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
        FillRect(hMemDC, &rect, hBrush);
        DeleteObject(hBrush);

        // ── TransparentBlt로 배경색 제거하며 이미지 출력 ──────────────
        // HDC hImgDC = CreateCompatibleDC(hMemDC);
        // HBITMAP hOld = (HBITMAP)SelectObject(hImgDC, g_img[g_frame].Detach());
        // TransparentBlt(
        //     hMemDC,                          // 출력 DC
        //     g_x, 100,                        // 출력 x, y
        //     200, 200,                        // 출력 너비, 높이
        //     hImgDC,                          // 원본 DC
        //     0, 0,                            // 원본 시작점
        //     g_img[g_frame].GetWidth(),
        //     g_img[g_frame].GetHeight(),
        //     RGB(255, 255, 255)               // ← 날릴 배경색 (흰색)
        // );
        // SelectObject(hImgDC, hOld);
        // DeleteDC(hImgDC);
        // ─────────────────────────────────────────────────────────────

        // 기본 Draw (배경 제거 없이 출력, TransparentBlt 쓸 때는 주석 처리)
        g_img[g_frame].Draw(hMemDC, g_x, 100, 200, 200);

        // 장애물 사각형 그리기
        HBRUSH hRedBrush = CreateSolidBrush(RGB(255, 0, 0));
        RECT drawB = { obx, oby, obx + obw, oby + obh };
        FillRect(hMemDC, &drawB, hRedBrush);
        DeleteObject(hRedBrush);

        BitBlt(hdc, 0, 0, W, H, hMemDC, 0, 0, SRCCOPY);

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_DESTROY:
    {
        KillTimer(hWnd, 1);
        KillTimer(hWnd, 2);  // ← 추가
        for (int i = 0; i < IMG_COUNT; i++)
            g_img[i].Destroy();
        DeleteDC(hMemDC);
        DeleteObject(hBitmap);
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}