#include <windows.h>
#include <atlimage.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#pragma comment(lib, "msimg32.lib")

#define WIN_W       800
#define WIN_H       500
#define SKY_H       300
#define GND_H       200
#define GROUND_Y    385
#define AIR_Y       (GROUND_Y - 130)
#define CHAR_W      50
#define CHAR_H      70
#define CHAR_CROP_H 28
#define MON_W       65
#define MON_H       55
#define EXP_W       100
#define EXP_H       100
#define MAX_MON     5
#define GRAVITY     0.55f
#define JUMP_FORCE  (-13.0f)
#define CHAR_SPEED  5
#define SKY_SPD     1.8f
#define GND_SPD     0.9f
#define TID_ANIM    1
#define TID_PHYS    2
#define ANIM_MS     150
#define PHYS_MS     16

enum CharState { ST_RUN, ST_JUMP, ST_CROUCH };

struct Character { float x, y, vy; CharState state; int frame, invTimer; };
struct Monster { float x, y, spd; int type, frame; bool alive; };
struct Explosion { float x, y; int frame, tick; bool alive; };

static Character gChar;
static Monster   gMons[MAX_MON];
static Explosion gExp;
static float     gSkyOff = 0, gGndOff = 0;
static int       gAnimTick = 0, gSpawnTimer = 0;
static bool      gKeys[256] = {};
static HDC       hBufDC;
static HBITMAP   hBufBmp;

// ↓↓↓ CImage 이미지 객체 ↓↓↓
static CImage imgBgSky, imgBgGnd;
static CImage imgCharF[2], imgCharJump, imgCharCrouch;
static CImage imgMonF[2][2];
static CImage imgExpF[2];

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static void ImgLoad(CImage& img, const wchar_t* file) {
    img.Load(file);
}

static void ImgDraw(HDC hdc, CImage& img, int x, int y, int dw, int dh) {
    if (img.IsNull()) return;
    img.Draw(hdc, x, y, dw, dh, 0, 0, img.GetWidth(), img.GetHeight());
}
static void ImgDrawWhiteKey(HDC hdc, CImage& img, int x, int y, int dw, int dh) {
    if (img.IsNull()) return;
    int sw = img.GetWidth(), sh = img.GetHeight();

    BITMAPINFOHEADER bih = {};
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = sw;
    bih.biHeight = -sh;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hDib = CreateDIBSection(NULL, (BITMAPINFO*)&bih, DIB_RGB_COLORS, &bits, NULL, 0);
    if (!hDib) return;

    HDC mdc = CreateCompatibleDC(NULL);
    HBITMAP mold = (HBITMAP)SelectObject(mdc, hDib);

    RECT rc = { 0, 0, sw, sh };
    FillRect(mdc, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
    img.Draw(mdc, 0, 0, sw, sh, 0, 0, sw, sh);

    TransparentBlt(hdc, x, y, dw, dh, mdc, 0, 0, sw, sh, RGB(255, 255, 255));

    SelectObject(mdc, mold);
    DeleteObject(hDib);
    DeleteDC(mdc);
}
static void BgDraw(HDC hdc, CImage& img, int y, int layerH, float offset) {
    if (img.IsNull()) return;
    int bw = img.GetWidth();
    int bh = img.GetHeight();
    int off = (int)fmodf(offset, (float)bw);
    for (int x = -off; x < WIN_W; x += bw)
        img.Draw(hdc, x, y, bw, layerH, 0, 0, bw, bh);
}

static void GameInit() {
    srand((unsigned)time(NULL));
    gChar = { 560.0f, (float)GROUND_Y, 0.0f, ST_RUN, 0, 0 };
    gExp.alive = false;
    gSkyOff = gGndOff = 0;
    gAnimTick = gSpawnTimer = 0;
    for (int i = 0; i < MAX_MON; i++) gMons[i].alive = false;
}

static void SpawnMon() {
    for (int i = 0; i < MAX_MON; i++) {
        if (!gMons[i].alive) {
            gMons[i].alive = true;
            gMons[i].type = rand() % 2;
            gMons[i].x = -(float)MON_W;
            gMons[i].y = (gMons[i].type == 0) ? (float)GROUND_Y : (float)AIR_Y;
            gMons[i].spd = 2.0f + (rand() % 25) * 0.12f;
            gMons[i].frame = 0;
            break;
        }
    }
}

static bool AABB(float ax, float ay, int aw, int ah,
    float bx, float by, int bw, int bh) {
    return (ax < bx + bw) && (ax + aw > bx) &&
        (ay < by + bh) && (ay + ah > by);
}

static void GameUpdate() {
    gSkyOff += SKY_SPD;
    gGndOff += GND_SPD;

    if (gKeys[VK_LEFT] && gChar.x > 0)              gChar.x -= CHAR_SPEED;
    if (gKeys[VK_RIGHT] && gChar.x < WIN_W - CHAR_W) gChar.x += CHAR_SPEED;

    if (gChar.state == ST_JUMP) {
        gChar.vy += GRAVITY;
        gChar.y += gChar.vy;
        if (gChar.y >= GROUND_Y) {
            gChar.y = (float)GROUND_Y; gChar.vy = 0; gChar.state = ST_RUN;
        }
    }

    if (++gSpawnTimer >= 100 + rand() % 100) { SpawnMon(); gSpawnTimer = 0; }

    for (int i = 0; i < MAX_MON; i++) {
        if (!gMons[i].alive) continue;
        gMons[i].x += gMons[i].spd;
        if (gMons[i].x > WIN_W) gMons[i].x = -(float)MON_W;

        if (gChar.invTimer == 0) {
            int   ch = (gChar.state == ST_CROUCH) ? CHAR_CROP_H : CHAR_H;
            float my = gMons[i].y - MON_H;
            if (AABB(gChar.x, gChar.y - ch, CHAR_W, ch, gMons[i].x, my, MON_W, MON_H)) {
                gExp = { gMons[i].x + MON_W / 2.0f - EXP_W / 2.0f, my - EXP_H / 2.0f, 0, 0, true };
                gMons[i].alive = false;
                gChar.invTimer = 90;
            }
        }
    }
    if (gChar.invTimer > 0) gChar.invTimer--;

    if (gExp.alive && ++gExp.tick >= 10) {
        gExp.tick = 0;
        if (++gExp.frame >= 2) gExp.alive = false;
    }
}

static void GameDraw(HDC hdc) {
    BgDraw(hdc, imgBgSky, 0, SKY_H, gSkyOff);
    BgDraw(hdc, imgBgGnd, SKY_H, GND_H, gGndOff);

    for (int i = 0; i < MAX_MON; i++) {
        if (!gMons[i].alive) continue;
        ImgDraw(hdc, imgMonF[gMons[i].type][gMons[i].frame],
            (int)gMons[i].x, (int)gMons[i].y - MON_H, MON_W, MON_H);
    }

    bool blink = (gChar.invTimer > 0) && (gAnimTick % 2 == 0);
    if (!blink) {
        int x = (int)gChar.x, y = (int)gChar.y;
        if (gChar.state == ST_JUMP)   ImgDrawWhiteKey(hdc, imgCharJump, x, y - CHAR_H, CHAR_W, CHAR_H);
        else if (gChar.state == ST_CROUCH) ImgDrawWhiteKey(hdc, imgCharCrouch, x, y - CHAR_CROP_H, CHAR_W, CHAR_CROP_H);
        else                               ImgDrawWhiteKey(hdc, imgCharF[gChar.frame], x, y - CHAR_H, CHAR_W, CHAR_H);
    }

    if (gExp.alive)
        ImgDraw(hdc, imgExpF[gExp.frame], (int)gExp.x, (int)gExp.y, EXP_W, EXP_H);
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE, LPSTR, int ns) {
    WNDCLASSA wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hi;
    wc.lpszClassName = "JS57";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA("JS57", "Jump Game 5-7",
        WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
        CW_USEDEFAULT, CW_USEDEFAULT, WIN_W + 16, WIN_H + 39,
        NULL, NULL, hi, NULL);

    ShowWindow(hwnd, ns);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {

    case WM_CREATE: {
        // ↓↓↓ 이미지 파일명 입력 (L"파일명.png") ↓↓↓
        ImgLoad(imgBgSky, L"sky.png");   // 하늘 배경
        ImgLoad(imgBgGnd, L"ground.png");   // 땅 배경
        ImgLoad(imgCharF[0], L"goranirun1.png");   // 캐릭터 달리기 프레임0
        ImgLoad(imgCharF[1], L"goranirun2.png");   // 캐릭터 달리기 프레임1
        ImgLoad(imgCharJump, L"goranirun3.png");   // 캐릭터 점프
        ImgLoad(imgCharCrouch, L"goranirun5.png");   // 캐릭터 엎드리기
        ImgLoad(imgMonF[0][0], L"packman1.png");   // 바닥 몬스터 프레임0
        ImgLoad(imgMonF[0][1], L"packman2.png");   // 바닥 몬스터 프레임1
        ImgLoad(imgMonF[1][0], L"packmanD.png");   // 공중 몬스터 프레임0
        ImgLoad(imgMonF[1][1], L"packmanD2.png");   // 공중 몬스터 프레임1
        ImgLoad(imgExpF[0], L"boom2.png");   // 폭발 프레임0
        ImgLoad(imgExpF[1], L"boom.png");   // 폭발 프레임1
        // ↑↑↑ 이미지 파일명 입력 ↑↑↑

        HDC hdc = GetDC(hwnd);
        hBufDC = CreateCompatibleDC(hdc);
        hBufBmp = CreateCompatibleBitmap(hdc, WIN_W, WIN_H);
        SelectObject(hBufDC, hBufBmp);
        ReleaseDC(hwnd, hdc);

        GameInit();
        SetTimer(hwnd, TID_ANIM, ANIM_MS, NULL);
        SetTimer(hwnd, TID_PHYS, PHYS_MS, NULL);
        return 0;
    }

    case WM_TIMER:
        if (wp == TID_ANIM) {
            gAnimTick++;
            gChar.frame = gAnimTick % 2;
            for (int i = 0; i < MAX_MON; i++)
                if (gMons[i].alive) gMons[i].frame = gAnimTick % 2;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        else if (wp == TID_PHYS) {
            GameUpdate();
        }
        return 0;

    case WM_KEYDOWN:
        if (wp < 256) gKeys[wp] = true;
        if (wp == VK_ESCAPE) { DestroyWindow(hwnd); return 0; }
        if (wp == 0x4A && !(lp & 0x40000000)) {
            if (GetKeyState(VK_SHIFT) & 0x8000) {
                if (gChar.state == ST_RUN) gChar.state = ST_CROUCH;
            }
            else {
                if (gChar.state == ST_RUN) { gChar.state = ST_JUMP; gChar.vy = JUMP_FORCE; }
            }
        }
        return 0;

    case WM_KEYUP:
        if (wp < 256) gKeys[wp] = false;
        if (wp == 0x4A && gChar.state == ST_CROUCH) gChar.state = ST_RUN;
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc = { 0, 0, WIN_W, WIN_H };
        FillRect(hBufDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));
        GameDraw(hBufDC);
        BitBlt(hdc, 0, 0, WIN_W, WIN_H, hBufDC, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_DESTROY:
        KillTimer(hwnd, TID_ANIM);
        KillTimer(hwnd, TID_PHYS);
        DeleteObject(hBufBmp);
        DeleteDC(hBufDC);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}