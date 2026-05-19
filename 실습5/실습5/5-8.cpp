#include <windows.h>
#include <windowsx.h>
#include <atlimage.h>
#include <time.h>
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Msimg32.lib")  // TransparentBlt 라이브러리

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define IMG_COUNT1 6
int g_frame = 0;
int g_frameTimer = 0;
CImage g_img[IMG_COUNT1];
static CImage  imgBg;
int g_x[40] = {};
int obx = 400, oby = 100;
int obw = 50, obh = 50;
int grand[40] = {};
int g_W = 1, g_H = 1;
int dx = 0;
int dy = 0;
int ox = 0;
int oy = 0;
bool click = false;
bool paint = false;
HPEN  rectpen;
int dbrect1[10] = {};
int dbrect2[10] = {};
int obrect1[10] = {};
int obrect2[10] = {};
int clickcount = 0;
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

        srand((unsigned)time(NULL));
        rectpen = (HPEN)CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        imgBg.Load(TEXT("back.bmp"));
        g_img[0].Load(TEXT("ch1.bmp"));
        g_img[1].Load(TEXT("ch2.bmp"));
        g_img[2].Load(TEXT("ch3.bmp"));
        g_img[3].Load(TEXT("ch4.bmp"));
        g_img[4].Load(TEXT("ch5.bmp"));
        g_img[5].Load(TEXT("ch6.bmp"));
        SetTimer(hWnd, 1, 16, NULL);
        for (int i = 0; i < 40; ++i)
        {
            grand[i] = rand() % rect.right;
        }
        HDC hdc = GetDC(hWnd);
        hMemDC = CreateCompatibleDC(hdc);
        hBitmap = CreateCompatibleBitmap(hdc, 800, 600);
        SelectObject(hMemDC, hBitmap);
        ReleaseDC(hWnd, hdc);
        return 0;
    }

  
    case WM_LBUTTONDOWN: {
        dx = GET_X_LPARAM(lParam);
        dy = GET_Y_LPARAM(lParam) ;
        dbrect1[1] = dx;
        dbrect2[1] = dy;
        click = true;
        return 0;
    }
    case WM_MOUSEMOVE: {
        
        ox = GET_X_LPARAM(lParam);
        oy = GET_Y_LPARAM(lParam) ;
       
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }
    case WM_LBUTTONUP: {
        click = false;
        paint = true;
        obrect1[1] = ox;
        obrect2[1] = oy;
        InvalidateRect(hWnd, NULL, TRUE);
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
           
            g_frameTimer++;
            if (g_frameTimer >= 10) {
                g_frameTimer = 0;
                g_frame = (g_frame + 1) % IMG_COUNT1;
            }
            for (int i = 0; i < 40; ++i)
            {

            if (g_x[i] > rect.bottom - 30) g_x[i] = rect.bottom - 30;

            if (g_x[i] < obrect2[1] -30)
            {
                g_x[i] += 2;
            }
            else if (grand[i] >= dbrect1[1] && grand[i] <= obrect1[1])
            {
                g_x[i] += 2;
            }
            }
           
            

            InvalidateRect(hWnd, NULL, FALSE);
            return 0;
        }
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HDC     memDC = CreateCompatibleDC(hdc);
        HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, hBitmap);

        FillRect(memDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
     
            imgBg.Draw(hMemDC, 0, 0, rect.right, rect.bottom,
                0, 0, imgBg.GetWidth(), imgBg.GetHeight());
            // 장애물 사각형 그리기
            if (paint == true)
            {

                HPEN oldpen = (HPEN)SelectObject(hMemDC, rectpen);
                Rectangle(hMemDC, dbrect1[1], dbrect2[1], obrect1[1], obrect2[1]);
                
                SelectObject(hMemDC, oldpen);

            }
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
            for (int i = 0; i < 40; ++i)
            {

                g_img[g_frame].Draw(hMemDC, grand[i], g_x[i], 30, 30);
            }

       

        BitBlt(hdc, 0, 0, W, H, hMemDC, 0, 0, SRCCOPY);

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_DESTROY:
    {
        imgBg.Destroy();
        KillTimer(hWnd, 1);
        DeleteObject(rectpen);
        for (int i = 0; i < IMG_COUNT1; i++)
            g_img[i].Destroy();
        DeleteDC(hMemDC);
        DeleteObject(hBitmap);
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}