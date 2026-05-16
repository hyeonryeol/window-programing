#pragma comment(lib, "msimg32.lib")
#include <windows.h>
#include <atlbase.h>
#include <atlImage.h>
#include <windowsx.h>
#include <cstdlib>
#include <ctime>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

PAINTSTRUCT ps;
static CImage imgGorani[5];
static CImage img[5];
static CImage imgBg;
static int xPos = 0, yPos = 200;
static int chaCount = 0;
static RECT rect;
int nomouserand = 0;
static CImage imgMouse[3];
static int mouseX = 400, mouseY = 100;
static int mouseCount = 0;
bool mouse = false;
bool gorani = false;
static int speed = 100;     // ★ 전역 static으로 이동 (지역 변수였던 버그 수정)

// ★ 먹이 관련 변수 추가
static int foodX = 0, foodY = 0;
static bool foodVisible = false;

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
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
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
    {
        srand((unsigned int)time(NULL));
        imgBg.Load(L"gorani.bmp");

        for (int i = 0; i < 5; i++) {
            wchar_t path[32];
            swprintf(path, 32, L"goranirun%d.png", i + 1);
            img[i].Load(path);
        }
        for (int i = 0; i < 3; i++) {
            wchar_t paths[32];
            swprintf(paths, 32, L"mouse%d.png", i + 1);
            imgMouse[i].Load(paths);
        }
        for (int i = 0; i < 3; i++) {
            wchar_t path[32];
            swprintf(path, 32, L"goraniscream%d.png", i + 1); 
            imgGorani[i].Load(path);
        }
        nomouserand = rand() % 4 + 1;
        GetClientRect(hWnd, &rect);
        xPos = rect.right / 2 - 150;
        yPos = rect.bottom / 2 - 150;
        SetTimer(hWnd, 1, speed, nullptr);
        return 0;
    }

    case WM_SIZE:
    {
        GetClientRect(hWnd, &rect);
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        mouse = true;
        mouseX = GET_X_LPARAM(lParam);
        mouseY = GET_Y_LPARAM(lParam);
        // ★ 판정 버그 수정: mouseY <= 300 → mouseY <= yPos + 300
        if (mouseX >= xPos && mouseY >= yPos &&
            mouseX <= xPos + 300 && mouseY <= yPos + 300)
        {
            gorani = true;
        }
        InvalidateRect(hWnd, NULL, FALSE);
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        if (wParam & MK_LBUTTON) {
            mouseX = GET_X_LPARAM(lParam);
            mouseY = GET_Y_LPARAM(lParam);
            InvalidateRect(hWnd, NULL, FALSE);
        }
        return 0;
    }

    case WM_LBUTTONUP:
    {
        mouse = false;
        gorani = false;
        nomouserand = rand() % 4 + 1;
        return 0;
    }

    // ★ 오른쪽 마우스 버튼 - 먹이 기능 추가
    case WM_RBUTTONDOWN:
    {
        foodX = GET_X_LPARAM(lParam);
        foodY = GET_Y_LPARAM(lParam);
        foodVisible = true;
        InvalidateRect(hWnd, NULL, FALSE);
        return 0;
    }

    case WM_PAINT:
    {
        HDC hdc = BeginPaint(hWnd, &ps);

        HDC     memDC = CreateCompatibleDC(hdc);
        HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, hBitmap);

        FillRect(memDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

        // 배경
        if (!imgBg.IsNull())
            imgBg.Draw(memDC, 0, 0, rect.right, rect.bottom,
                0, 0, imgBg.GetWidth(), imgBg.GetHeight());

        // ★ 먹이 그리기 (빨간 원)
        if (foodVisible)
        {
            HBRUSH hFoodBrush = CreateSolidBrush(RGB(255, 0, 0));
            HBRUSH oldBrush = (HBRUSH)SelectObject(memDC, hFoodBrush);
            Ellipse(memDC, foodX - 20, foodY - 20, foodX + 20, foodY + 20);
            SelectObject(memDC, oldBrush);
            DeleteObject(hFoodBrush);
        }

        // 스프라이트 (고양이)
        if (!img[chaCount].IsNull()) {
            HDC imgDC = img[chaCount].GetDC();
            TransparentBlt(memDC, xPos, yPos, 300, 300,
                imgDC, 0, 0,
                img[chaCount].GetWidth(),
                img[chaCount].GetHeight(),
                RGB(255, 255, 255));
            img[chaCount].ReleaseDC();
        }

        // 쥐 그리기
        if (mouse == true && gorani == false)
        {
            if (!imgMouse[mouseCount].IsNull()) {
                imgMouse[mouseCount].Draw(memDC, mouseX - 50, mouseY - 50, 100, 100,
                    0, 0,
                    imgMouse[mouseCount].GetWidth(),
                    imgMouse[mouseCount].GetHeight());
            }
        }

        if (gorani == true)
        {
            if (!imgGorani[chaCount].IsNull()) {
                imgGorani[chaCount].Draw(memDC, xPos, yPos, 300, 300,
                    0, 0,
                    imgGorani[chaCount].GetWidth(),
                    imgGorani[chaCount].GetHeight());
            }
        }
        else
        {
            if (!img[chaCount].IsNull()) {
                HDC imgDC = img[chaCount].GetDC();
                TransparentBlt(memDC, xPos, yPos, 300, 300,
                    imgDC, 0, 0,
                    img[chaCount].GetWidth(),
                    img[chaCount].GetHeight(),
                    RGB(255, 255, 255));
                img[chaCount].ReleaseDC();
            }
        }
     

        BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBmp);
        DeleteObject(hBitmap);
        DeleteDC(memDC);
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_TIMER:
    {
        // 랜덤 이동 (마우스/먹이 없을 때)
        if (mouse == false && !foodVisible)
        {
            if (nomouserand == 1) { xPos += 5; yPos -= 5; }
            else if (nomouserand == 2) { xPos += 5; yPos += 5; }
            else if (nomouserand == 3) { xPos -= 5; yPos += 5; }
            else if (nomouserand == 4) { xPos -= 5; yPos -= 5; }

            if (xPos > rect.right)        xPos = -300;
            if (xPos + 300 < rect.left)   xPos = rect.right;
            if (yPos + 300 < 0)           yPos = rect.bottom;
            if (yPos > rect.bottom)       yPos = -300;
        }

        // 쥐 추적 (배경 클릭 시, 고양이 클릭 아닐 때만)
        if (mouse == true && gorani == false)
        {
            int dx = mouseX - (xPos + 150);
            int dy = mouseY - (yPos + 150);
            if (dx > 0) xPos += 5;
            else if (dx < 0) xPos -= 5;
            if (dy > 0) yPos += 5;
            else if (dy < 0) yPos -= 5;

            mouseCount = (mouseCount + 1) % 3;
        }

        // 먹이 추적
        if (foodVisible)
        {
            int dx = foodX - (xPos + 150);
            int dy = foodY - (yPos + 150);
            if (dx > 0) xPos += 5;
            else if (dx < 0) xPos -= 5;
            if (dy > 0) yPos += 5;
            else if (dy < 0) yPos -= 5;

            if (abs(dx) < 30 && abs(dy) < 30)
            {
                foodVisible = false;
                speed = max(20, speed - 20);
                KillTimer(hWnd, 1);
                SetTimer(hWnd, 1, speed, nullptr);
                nomouserand = rand() % 4 + 1;
            }
        }

        chaCount = (chaCount + 1) % 5;
        InvalidateRect(hWnd, NULL, FALSE);
        return 0;
    }

    case WM_KEYDOWN:
        if (wParam == 'Q') PostQuitMessage(0);
        if (wParam == 'R')
        {
            xPos = 0;
            yPos = 0;
            speed = 100;
            foodVisible = false;
            KillTimer(hWnd, 1);
            SetTimer(hWnd, 1, speed, nullptr);
        }
        
        if (wParam == VK_OEM_PLUS)   
        {
            if (speed > 20) {
                speed -= 20;
                KillTimer(hWnd, 1);
                SetTimer(hWnd, 1, speed, nullptr);
            }
        }
        if (wParam == VK_OEM_MINUS)  
        {
            speed += 20;
            KillTimer(hWnd, 1);
            SetTimer(hWnd, 1, speed, nullptr);
        }
        return 0;

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        for (int i = 0; i < 5; i++) img[i].Destroy();
        imgBg.Destroy();
        for (int i = 0; i < 3; i++) imgGorani[i].Destroy();
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}