#pragma comment(lib, "msimg32.lib")
#include <windows.h>
#include <atlbase.h>
#include <atlImage.h>
#include <windowsx.h>
#include <cstdlib>
#include <ctime>
#include <cmath>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"실습 5-6";

PAINTSTRUCT ps;
static RECT rect;

static CImage imgsD[2];
static int     packmanSize = 100;
static COLORREF packmanColor = RGB(255, 220, 0);
static float   angle = 0.0f;
static CImage  imgBg;
static CImage  imgs[2];
static int     frameCount = 0;
static int     xPos = 0, yPos = 0;
static int     packmanmove = 1;
static int     timerInterval = 100;

static int packmanjump = 0;

static bool isClickAnim = false;
static int  clickAnimCount = 0;

static const int HIST_SIZE = 60;
static int  histX[HIST_SIZE] = {};
static int  histY[HIST_SIZE] = {};
static int  histHead = 0;
static int  histCount = 0;
static int  twinCount = 0;

static const int MAX_SHOT = 100;
static int  shot = 0;
static int  shotpos[MAX_SHOT] = {};
static int  shotposy[MAX_SHOT] = {};
static int  shotdir[MAX_SHOT] = {};
static bool shotUsed[MAX_SHOT] = {};

static const int OB_COUNT = 8;
static int  obposx[OB_COUNT] = {};
static int  obposy[OB_COUNT] = {};
static bool obVisible[OB_COUNT] = {};
static int  obSize[OB_COUNT] = {};
static int  obHp[OB_COUNT] = {};

static const int MAX_FOOD = 100;
static int      foodx[MAX_FOOD] = {};
static int      foody[MAX_FOOD] = {};
static bool     foodVis[MAX_FOOD] = {};
static COLORREF foodColor[MAX_FOOD] = {};
static int      foodMoveX[MAX_FOOD] = {};
static int      foodMoveY[MAX_FOOD] = {};
static int      foodCount = 20;
static int      foodRevealTimer = 0;
static int      foodRevealIdx = 0;

static HBRUSH gunbrush;
static HBRUSH obbrush;

static void setAngleForDir(int dir) {
    switch (dir) {
    case 1: angle = 0.0f; break;
    case 2: angle = 180.0f; break;
    case 3: angle = 270.0f; break;
    case 4: angle = 90.0f; break;
    }
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

    HWND hWnd = CreateWindowExW(0, CLASS_NAME, WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
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
        GetClientRect(hWnd, &rect);

        imgBg.Load(L"background.bmp");
        for (int i = 0; i < 2; i++) {
            wchar_t path[32];
            swprintf(path, 32, L"packman%d.png", i + 1);
            imgs[i].Load(path);
        }
        for (int i = 0; i < 2; i++) {
            wchar_t path[32];
            swprintf(path, 32, L"packmanD%d.png", i + 1);
            imgsD[i].Load(path);
        }
        xPos = rect.right / 2 - 50;
        yPos = rect.bottom / 2 - 50;

        // 장애물 초기화 (8개, 랜덤 크기, 크기에 따른 HP)
        for (int i = 0; i < OB_COUNT; ++i) {
            obSize[i] = rand() % 80 + 50;
            obHp[i] = obSize[i] / 30 + 1;
            obposx[i] = rand() % max(1, rect.right - obSize[i]);
            obposy[i] = rand() % max(1, rect.bottom - obSize[i]);
            obVisible[i] = true;
        }

        // 먹이 초기화 (20개, 랜덤 색, 전부 숨김 후 순차 등장)
        for (int i = 0; i < foodCount; ++i) {
            foodx[i] = rand() % max(1, rect.right - 20);
            foody[i] = rand() % max(1, rect.bottom - 20);
            foodVis[i] = false;
            foodColor[i] = RGB(rand() % 200 + 55, rand() % 200 + 55, rand() % 200 + 55);
            if (rand() % 2 == 0) { foodMoveX[i] = (rand() % 2 == 0) ? 2 : -2; foodMoveY[i] = 0; }
            else { foodMoveX[i] = 0; foodMoveY[i] = (rand() % 2 == 0) ? 2 : -2; }
        }
        foodRevealIdx = 0;
        foodRevealTimer = 10;

        gunbrush = CreateSolidBrush(RGB(255, 0, 0));
        obbrush = CreateSolidBrush(RGB(0, 0, 255));

        SetTimer(hWnd, 1, timerInterval, nullptr);
        return 0;
    }

    case WM_SIZE:
        GetClientRect(hWnd, &rect);
        return 0;

    case WM_LBUTTONDOWN:
    {
        int mx = GET_X_LPARAM(lParam);
        int my = GET_Y_LPARAM(lParam);

        if (mx >= xPos && mx <= xPos + packmanSize &&
            my >= yPos && my <= yPos + packmanSize)
        {
            isClickAnim = true;
            clickAnimCount = 20;
            int newDir;
            do { newDir = rand() % 4 + 1; } while (newDir == packmanmove);
            packmanmove = newDir;
            setAngleForDir(packmanmove);
        }
        InvalidateRect(hWnd, NULL, FALSE);
        return 0;
    }

    case WM_RBUTTONDOWN:
    {
        int mx = GET_X_LPARAM(lParam);
        int my = GET_Y_LPARAM(lParam);

        for (int i = 0; i < OB_COUNT; ++i) {
            if (!obVisible[i]) continue;
            if (mx >= obposx[i] && mx <= obposx[i] + obSize[i] &&
                my >= obposy[i] && my <= obposy[i] + obSize[i])
            {
                obposx[i] += rand() % 100 - 50;
                obposy[i] += rand() % 100 - 50;
                obposx[i] = max(0, min(obposx[i], rect.right - obSize[i]));
                obposy[i] = max(0, min(obposy[i], rect.bottom - obSize[i]));
                InvalidateRect(hWnd, NULL, FALSE);
                break;
            }
        }
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

        // 먹이 (개별 랜덤 색)
        for (int i = 0; i < foodCount; ++i) {
            if (!foodVis[i]) continue;
            HBRUSH fb = CreateSolidBrush(foodColor[i]);
            HBRUSH olb = (HBRUSH)SelectObject(memDC, fb);
            Ellipse(memDC, foodx[i] - 10, foody[i] - 10, foodx[i] + 10, foody[i] + 10);
            SelectObject(memDC, olb);
            DeleteObject(fb);
        }

        // 장애물 (랜덤 크기)
        HBRUSH oldbrush = (HBRUSH)SelectObject(memDC, obbrush);
        for (int i = 0; i < OB_COUNT; ++i)
            if (obVisible[i])
                Rectangle(memDC, obposx[i], obposy[i], obposx[i] + obSize[i], obposy[i] + obSize[i]);

        // 총알
        SelectObject(memDC, gunbrush);
        for (int i = 0; i < shot; ++i)
            if (!shotUsed[i])
                Ellipse(memDC, shotpos[i] + 30, shotposy[i] + 30, shotpos[i] + 60, shotposy[i] + 60);

        SelectObject(memDC, oldbrush);

        float rad = angle * 3.14159f / 180.0f;
        float cx = (float)(xPos + packmanSize / 2);
        float cy = (float)(yPos + packmanSize / 2);

        // 트윈 팩맨
        for (int t = 0; t < twinCount; ++t) {
            int delay = (t + 1) * 10;
            if (histCount >= delay) {
                int idx = (histHead - delay + HIST_SIZE) % HIST_SIZE;
                int tx = histX[idx];
                int ty = histY[idx];
                float tcx = (float)(tx + packmanSize / 2);
                float tcy = (float)(ty + packmanSize / 2);

                XFORM txform;
                txform.eM11 = (float)cos(rad);
                txform.eM12 = (float)sin(rad);
                txform.eM21 = -(float)sin(rad);
                txform.eM22 = (float)cos(rad);
                txform.eDx = tcx - tcx * (float)cos(rad) + tcy * (float)sin(rad);
                txform.eDy = tcy - tcx * (float)sin(rad) - tcy * (float)cos(rad);

                SetGraphicsMode(memDC, GM_ADVANCED);
                SetWorldTransform(memDC, &txform);

                if (!imgs[frameCount].IsNull())
                    imgs[frameCount].Draw(memDC, tx, ty, packmanSize, packmanSize,
                        0, 0, imgs[frameCount].GetWidth(), imgs[frameCount].GetHeight());

                ModifyWorldTransform(memDC, NULL, MWT_IDENTITY);
                SetGraphicsMode(memDC, GM_COMPATIBLE);
            }
        }

        // 팩맨 (회전 + 먹이 색 배경)
        XFORM xform;
        xform.eM11 = (float)cos(rad);
        xform.eM12 = (float)sin(rad);
        xform.eM21 = -(float)sin(rad);
        xform.eM22 = (float)cos(rad);
        xform.eDx = cx - cx * (float)cos(rad) + cy * (float)sin(rad);
        xform.eDy = cy - cx * (float)sin(rad) - cy * (float)cos(rad);

        SetGraphicsMode(memDC, GM_ADVANCED);
        SetWorldTransform(memDC, &xform);

        // 먹이 색으로 팩맨 배경 채우기
        HBRUSH colorBrush = CreateSolidBrush(packmanColor);
        HBRUSH oldbr = (HBRUSH)SelectObject(memDC, colorBrush);
        Ellipse(memDC, xPos, yPos, xPos + packmanSize, yPos + packmanSize);
        SelectObject(memDC, oldbr);
        DeleteObject(colorBrush);

        CImage* drawImgs = isClickAnim ? imgsD : imgs;
        if (!drawImgs[frameCount].IsNull())
            drawImgs[frameCount].Draw(memDC, xPos, yPos, packmanSize, packmanSize,
                0, 0, drawImgs[frameCount].GetWidth(), drawImgs[frameCount].GetHeight());

        ModifyWorldTransform(memDC, NULL, MWT_IDENTITY);
        SetGraphicsMode(memDC, GM_COMPATIBLE);

        // 클릭 애니메이션 표시
        if (isClickAnim) {
            HPEN hpen = CreatePen(PS_SOLID, 4, RGB(255, 165, 0));
            HPEN oldpen = (HPEN)SelectObject(memDC, hpen);
            SelectObject(memDC, GetStockObject(NULL_BRUSH));
            Ellipse(memDC, xPos - 10, yPos - 10, xPos + packmanSize + 10, yPos + packmanSize + 10);
            SelectObject(memDC, oldpen);
            DeleteObject(hpen);
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
        // 팩맨 이동
        if (packmanmove == 1) xPos += 5;
        if (packmanmove == 2) xPos -= 5;
        if (packmanmove == 3) yPos -= 5;
        if (packmanmove == 4) yPos += 5;

        // 화면 경계
        if (xPos > rect.right)            xPos = -packmanSize;
        if (xPos + packmanSize < 0)       xPos = rect.right;
        if (yPos > rect.bottom)           yPos = -packmanSize;
        if (yPos + packmanSize < 0)       yPos = rect.bottom;

        // 점프
        if (packmanjump == 1) { yPos -= 30; packmanjump = 2; }
        else if (packmanjump == 2) { yPos += 30; packmanjump = 0; }
        else if (packmanjump == 3) { xPos -= 30; packmanjump = 4; }
        else if (packmanjump == 4) { xPos += 30; packmanjump = 0; }
        else if (packmanjump == 5) { xPos += 30; packmanjump = 6; }
        else if (packmanjump == 6) { xPos -= 30; packmanjump = 0; }
        else if (packmanjump == 7) { yPos += 30; packmanjump = 8; }
        else if (packmanjump == 8) { yPos -= 30; packmanjump = 0; }

        // 클릭 카운트다운
        if (isClickAnim && --clickAnimCount <= 0)
            isClickAnim = false;

        // 트윈 히스토리 저장
        histX[histHead] = xPos;
        histY[histHead] = yPos;
        histHead = (histHead + 1) % HIST_SIZE;
        if (histCount < HIST_SIZE) histCount++;

        // 먹이 이동 (왔다갔다)
        for (int i = 0; i < foodCount; ++i) {
            if (!foodVis[i]) continue;
            foodx[i] += foodMoveX[i];
            foody[i] += foodMoveY[i];
            if (foodx[i] < 10 || foodx[i] > rect.right - 10) foodMoveX[i] = -foodMoveX[i];
            if (foody[i] < 10 || foody[i] > rect.bottom - 10) foodMoveY[i] = -foodMoveY[i];
        }

        // 팩맨과 먹이 충돌
        for (int i = 0; i < foodCount; ++i) {
            if (!foodVis[i]) continue;
            if (abs((xPos + packmanSize / 2) - foodx[i]) < 40 &&
                abs((yPos + packmanSize / 2) - foody[i]) < 40)
            {
                packmanColor = foodColor[i];
                foodVis[i] = false;
                packmanSize += 10;
            }
        }

        // 총알 이동
        for (int i = 0; i < shot; ++i) {
            if (shotUsed[i]) continue;
            if (shotdir[i] == 1) shotpos[i] += 20;
            if (shotdir[i] == 2) shotpos[i] -= 20;
            if (shotdir[i] == 3) shotposy[i] -= 20;
            if (shotdir[i] == 4) shotposy[i] += 20;
        }

        // 총알과 장애물 충돌 (HP 기반, 크기에 따라 속도 부스트 다름)
        for (int i = 0; i < shot; ++i) {
            if (shotUsed[i]) continue;
            int bx1 = shotpos[i] + 30, by1 = shotposy[i] + 30;
            int bx2 = shotpos[i] + 60, by2 = shotposy[i] + 60;
            for (int j = 0; j < OB_COUNT; ++j) {
                if (!obVisible[j]) continue;
                if (bx2 > obposx[j] && bx1 < obposx[j] + obSize[j] &&
                    by2 > obposy[j] && by1 < obposy[j] + obSize[j])
                {
                    obHp[j]--;
                    shotUsed[i] = true;
                    if (obHp[j] <= 0) {
                        obVisible[j] = false;
                        int boost = obSize[j] / 20 + 5;
                        timerInterval = max(30, timerInterval - boost);
                        KillTimer(hWnd, 1);
                        SetTimer(hWnd, 1, timerInterval, nullptr);
                    }
                    break;
                }
            }
        }

        // 총알과 먹이 충돌
        for (int i = 0; i < shot; ++i) {
            if (shotUsed[i]) continue;
            int bx = shotpos[i] + 45;
            int by = shotposy[i] + 45;
            for (int j = 0; j < foodCount; ++j) {
                if (!foodVis[j]) continue;
                if (abs(bx - foodx[j]) < 25 && abs(by - foody[j]) < 25) {
                    foodVis[j] = false;
                    shotUsed[i] = true;
                    for (int k = 0; k < 2 && foodCount < MAX_FOOD; ++k) {
                        foodx[foodCount] = max(10, min(foodx[j] + rand() % 100 - 50, rect.right - 10));
                        foody[foodCount] = max(10, min(foody[j] + rand() % 100 - 50, rect.bottom - 10));
                        foodVis[foodCount] = true;
                        foodColor[foodCount] = RGB(rand() % 200 + 55, rand() % 200 + 55, rand() % 200 + 55);
                        if (rand() % 2 == 0) { foodMoveX[foodCount] = 2; foodMoveY[foodCount] = 0; }
                        else { foodMoveX[foodCount] = 0; foodMoveY[foodCount] = 2; }
                        foodCount++;
                    }
                    break;
                }
            }
        }

        // 먹이 순차 등장
        if (foodRevealTimer > 0) {
            if (--foodRevealTimer == 0 && foodRevealIdx < foodCount) {
                foodVis[foodRevealIdx++] = true;
                if (foodRevealIdx < foodCount)
                    foodRevealTimer = 10;
            }
        }

        frameCount = (frameCount + 1) % 2;
        InvalidateRect(hWnd, NULL, FALSE);
        return 0;
    }

    case WM_KEYDOWN:
    {
        if (wParam == 'Q') PostQuitMessage(0);

        if (wParam == VK_RIGHT) { packmanmove = 1; setAngleForDir(1); InvalidateRect(hWnd, NULL, FALSE); return 0; }
        if (wParam == VK_LEFT) { packmanmove = 2; setAngleForDir(2); InvalidateRect(hWnd, NULL, FALSE); return 0; }
        if (wParam == VK_UP) { packmanmove = 3; setAngleForDir(3); InvalidateRect(hWnd, NULL, FALSE); return 0; }
        if (wParam == VK_DOWN) { packmanmove = 4; setAngleForDir(4); InvalidateRect(hWnd, NULL, FALSE); return 0; }

        if (wParam == 'J') {
            switch (packmanmove) {
            case 1: packmanjump = 1; break;
            case 2: packmanjump = 7; break;
            case 3: packmanjump = 3; break;
            case 4: packmanjump = 5; break;
            }
        }

        if (wParam == 'T' && twinCount < 3) twinCount++;

        if (wParam == 'A') {
            for (int i = 0; i < foodCount; ++i) foodVis[i] = false;
            foodRevealIdx = 0;
            foodRevealTimer = 10;
        }

        if (wParam == VK_RETURN && shot < MAX_SHOT) {
            shotpos[shot] = xPos;
            shotposy[shot] = yPos;
            shotdir[shot] = packmanmove;
            shotUsed[shot] = false;
            shot++;
        }

        return 0;
    }

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        for (int i = 0; i < 2; i++) imgs[i].Destroy();
        imgBg.Destroy();
        DeleteObject(gunbrush);
        DeleteObject(obbrush);
        for (int i = 0; i < 2; i++) imgsD[i].Destroy();
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}