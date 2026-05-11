#pragma comment (lib, "msimg32.lib")
#include <windows.h>
#include <atlImage.h>


// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HDC hDC, hMem1DC, hMem2DC;
PAINTSTRUCT ps;
HBITMAP OldBit[3];
BITMAP bmp;
static HBITMAP hBitBackground, hBitCha[4], hBit;
static RECT rectView;
static int chaX, chaY, chaCount = 0;
static int backWidth, backHeight, chaWidth, chaHeight;

// 클래스 이름 / 창 제목
const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

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
  
    static CImage img, imgSprite;
    static int xPos = 0, yPos = 0;
    static RECT rect;
    GetClientRect(hWnd, &rect);
    switch (msg)
    {
    case WM_CREATE:
    {
        hDC = GetDC(hWnd);
        GetClientRect(hWnd, &rectView);
        hBitBackground = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1)); //--- 배경 이미지 1개
        hBitCha[0] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2)); //--- 애니메이션 이미지 4개
        hBitCha[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
        hBitCha[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));
        hBitCha[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));
        img.Load(L"gorani.bmp"); //--- background
        imgSprite.Load(L"cat.gif"); //--- sprite image
        SetTimer(hWnd, 1, 16, nullptr);
        return 0;
    }
    case WM_KEYDOWN:
    {
        return 0;
    }

    case WM_PAINT:
    {

        hDC = BeginPaint(hWnd, &ps);
        HBITMAP hBitmap;
        HDC memDC;
        hBitmap = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
        memDC = CreateCompatibleDC(hDC);
        (HBITMAP)SelectObject(memDC, hBitmap);
        int w = img.GetWidth();
        int h = img.GetHeight();
        img.Draw(memDC, 0, 0, rect.right, rect.bottom, 0, 0, w, h); //--- 메모리 DC에 배경 그리기
        imgSprite.Draw(memDC, xPos, yPos, 250, 200, 0, 0, 500, 397); //--- 메모리 DC에 스프라이트 그리기
        BitBlt(hDC, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY); //--- 메모리 DC의 그림을 화면 DC에 복사하기
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_TIMER:
    {

        xPos += 5;
        yPos += 5;
        InvalidateRect(hWnd, NULL, false);
        break;
    }

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}