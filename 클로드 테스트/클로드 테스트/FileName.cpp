#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// --- Configuration ---
// ALERT_THRESHOLD_MS: 30 minutes of continuous mouse movement triggers the reminder
// Change to e.g. 10 * 1000 (10 seconds) for quick testing
constexpr DWORD POLL_INTERVAL_MS   = 1000;
constexpr DWORD ALERT_THRESHOLD_MS = 30 * 60 * 1000;

// --- GDI resource bundle (allocated on WM_CREATE, freed on WM_DESTROY) ---
struct CatDrawResources {
    HBRUSH brBackground, brFur, brEarPink, brEyeWhite;
    HBRUSH brIris, brPupil, brNose, brCheek;
    HPEN   penOutline, penMouth, penWhisker;
    HFONT  fontKorean;
    HWND   hButton;
};
static CatDrawResources g_res = {};

// Forward declaration
LRESULT CALLBACK CatWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Draw the cartoon cat and message onto the given HDC (client area: 400 x 510)
static void DrawCat(HDC hdc)
{
    // 1. Background fill
    RECT rcAll = {0, 0, 400, 510};
    FillRect(hdc, &rcAll, g_res.brBackground);

    // 2. Ear outers — drawn before head so head overlaps the bases
    SelectObject(hdc, g_res.penOutline);
    SelectObject(hdc, g_res.brFur);
    POINT lEarOuter[3] = {{110,60},{75,130},{145,115}};
    POINT rEarOuter[3] = {{290,60},{325,130},{255,115}};
    Polygon(hdc, lEarOuter, 3);
    Polygon(hdc, rEarOuter, 3);

    // 3. Head (covers ear bases, only tips poke out above)
    Ellipse(hdc, 90, 80, 310, 300);

    // 4. Ear inners (pink, no outline)
    SelectObject(hdc, (HPEN)GetStockObject(NULL_PEN));
    SelectObject(hdc, g_res.brEarPink);
    POINT lEarInner[3] = {{110,78},{93,122},{140,110}};
    POINT rEarInner[3] = {{290,78},{307,122},{260,110}};
    Polygon(hdc, lEarInner, 3);
    Polygon(hdc, rEarInner, 3);

    // 5. Body
    SelectObject(hdc, g_res.penOutline);
    SelectObject(hdc, g_res.brFur);
    Ellipse(hdc, 120, 285, 280, 400);

    // 6. Front paws
    Ellipse(hdc, 138, 380, 180, 410);
    Ellipse(hdc, 220, 380, 262, 410);

    // 7. Cheek blush (no outline)
    SelectObject(hdc, (HPEN)GetStockObject(NULL_PEN));
    SelectObject(hdc, g_res.brCheek);
    Ellipse(hdc, 118, 220, 162, 248);
    Ellipse(hdc, 238, 220, 282, 248);

    // 8. Eye sclera (white, with outline)
    SelectObject(hdc, g_res.penOutline);
    SelectObject(hdc, g_res.brEyeWhite);
    Ellipse(hdc, 140, 155, 190, 215);
    Ellipse(hdc, 210, 155, 260, 215);

    // 8b. Iris — bright green, no outline
    SelectObject(hdc, (HPEN)GetStockObject(NULL_PEN));
    SelectObject(hdc, g_res.brIris);
    Ellipse(hdc, 148, 163, 182, 207);
    Ellipse(hdc, 218, 163, 252, 207);

    // 8c. Pupil — vertical slit
    SelectObject(hdc, g_res.brPupil);
    Ellipse(hdc, 158, 163, 172, 207);
    Ellipse(hdc, 228, 163, 242, 207);

    // 8d. Eye highlight — small white dot
    SelectObject(hdc, g_res.brEyeWhite);
    Ellipse(hdc, 173, 166, 181, 178);
    Ellipse(hdc, 243, 166, 251, 178);

    // 9. Nose — pink diamond
    SelectObject(hdc, g_res.penOutline);
    SelectObject(hdc, g_res.brNose);
    POINT nose[4] = {{200,228},{210,236},{200,244},{190,236}};
    Polygon(hdc, nose, 4);

    // 10. Mouth — W-shape using two arcs
    SelectObject(hdc, g_res.penMouth);
    SelectObject(hdc, (HBRUSH)GetStockObject(NULL_BRUSH));
    Arc(hdc, 176, 244, 200, 268, 200, 256, 176, 256);
    Arc(hdc, 200, 244, 224, 268, 200, 256, 224, 256);

    // 11. Whiskers — three on each side
    SelectObject(hdc, g_res.penWhisker);
    MoveToEx(hdc, 185, 232, nullptr); LineTo(hdc,  80, 215);
    MoveToEx(hdc, 185, 238, nullptr); LineTo(hdc,  75, 238);
    MoveToEx(hdc, 185, 244, nullptr); LineTo(hdc,  82, 258);
    MoveToEx(hdc, 215, 232, nullptr); LineTo(hdc, 320, 215);
    MoveToEx(hdc, 215, 238, nullptr); LineTo(hdc, 325, 238);
    MoveToEx(hdc, 215, 244, nullptr); LineTo(hdc, 318, 258);

    // 12. Korean message text
    SelectObject(hdc, g_res.fontKorean);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(60, 40, 20));
    SetTextAlign(hdc, TA_CENTER);
    const wchar_t* line1 = L"스트레칭 할 시간이에요!";
    const wchar_t* line2 = L"자리에서 일어나 움직여보세요.";
    TextOutW(hdc, 200, 415, line1, (int)wcslen(line1));
    TextOutW(hdc, 200, 443, line2, (int)wcslen(line2));
}

LRESULT CALLBACK CatWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        g_res.brBackground = CreateSolidBrush(RGB(255, 245, 230));
        g_res.brFur        = CreateSolidBrush(RGB(255, 200, 140));
        g_res.brEarPink    = CreateSolidBrush(RGB(255, 160, 170));
        g_res.brEyeWhite   = CreateSolidBrush(RGB(255, 255, 255));
        g_res.brIris       = CreateSolidBrush(RGB( 80, 180,  80));
        g_res.brPupil      = CreateSolidBrush(RGB( 30,  30,  30));
        g_res.brNose       = CreateSolidBrush(RGB(255, 130, 150));
        g_res.brCheek      = CreateSolidBrush(RGB(255, 200, 200));
        g_res.penOutline   = CreatePen(PS_SOLID, 2, RGB(160, 100,  60));
        g_res.penMouth     = CreatePen(PS_SOLID, 2, RGB(160, 100,  60));
        g_res.penWhisker   = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
        g_res.fontKorean   = CreateFontW(
            22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
            L"맑은 고딕"
        );
        HINSTANCE hInst = (HINSTANCE)GetWindowLongPtrW(hWnd, GWLP_HINSTANCE);
        g_res.hButton = CreateWindowW(
            L"BUTTON", L"확인",
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            150, 465, 100, 35,
            hWnd, (HMENU)1001, hInst, nullptr
        );
        if (g_res.hButton && g_res.fontKorean)
            SendMessage(g_res.hButton, WM_SETFONT, (WPARAM)g_res.fontKorean, TRUE);
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        DrawCat(hdc);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == 1001)
            DestroyWindow(hWnd);
        return 0;
    case WM_LBUTTONDOWN:
        DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        DeleteObject(g_res.brBackground);
        DeleteObject(g_res.brFur);
        DeleteObject(g_res.brEarPink);
        DeleteObject(g_res.brEyeWhite);
        DeleteObject(g_res.brIris);
        DeleteObject(g_res.brPupil);
        DeleteObject(g_res.brNose);
        DeleteObject(g_res.brCheek);
        DeleteObject(g_res.penOutline);
        DeleteObject(g_res.penMouth);
        DeleteObject(g_res.penWhisker);
        DeleteObject(g_res.fontKorean);
        g_res = {};
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
}

void ShowCatWindow()
{
    HINSTANCE hInst = GetModuleHandleW(nullptr);
    const wchar_t* CAT_CLASS = L"MouseBreakCatWindow";

    WNDCLASSEXW wc   = {};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = CatWndProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = CAT_CLASS;
    wc.hIcon         = LoadIconW(nullptr, IDI_APPLICATION);
    wc.hIconSm       = wc.hIcon;
    RegisterClassExW(&wc);  // harmless on repeated calls (ERROR_CLASS_ALREADY_EXISTS)

    // Compute outer window dimensions that give exactly a 400x510 client area
    RECT rc = {0, 0, 400, 510};
    AdjustWindowRectEx(&rc,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, FALSE, WS_EX_TOPMOST);
    int winW = rc.right  - rc.left;
    int winH = rc.bottom - rc.top;

    int posX = (GetSystemMetrics(SM_CXSCREEN) - winW) / 2;
    int posY = (GetSystemMetrics(SM_CYSCREEN) - winH) / 2;

    HWND hWnd = CreateWindowExW(
        WS_EX_TOPMOST,
        CAT_CLASS,
        L"잠깐 쉬어가요!",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        posX, posY, winW, winH,
        nullptr, nullptr, hInst, nullptr
    );
    if (!hWnd) return;

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    // Modal message loop — blocks until user dismisses the window
    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

int main()
{
    // Hide the console; this runs as a silent background utility
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    POINT lastPos = {};
    GetCursorPos(&lastPos);
    DWORD movingStart = 0;
    bool  wasMoving   = false;

    while (true)
    {
        Sleep(POLL_INTERVAL_MS);

        POINT cur = {};
        GetCursorPos(&cur);
        bool moved = (cur.x != lastPos.x || cur.y != lastPos.y);

        if (moved) {
            if (!wasMoving) {
                // Mouse just started moving — begin timing
                movingStart = GetTickCount();
                wasMoving   = true;
            } else if (GetTickCount() - movingStart >= ALERT_THRESHOLD_MS) {
                // Continuous movement exceeded threshold — show cat reminder
                ShowCatWindow();
                wasMoving = false;  // reset; fresh count after dismissal
            }
            lastPos = cur;
        } else {
            // Mouse stopped moving — reset the continuous-movement timer
            wasMoving = false;
        }
    }

    return 0;
}
