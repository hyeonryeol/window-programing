#include <windows.h>
#include <windowsx.h>   // GET_X_LPARAM, GET_Y_LPARAM
#include <vector>
#include <random>
#include <algorithm>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

const wchar_t CLASS_NAME[] = L"PuzzleClass";
const wchar_t WND_TITLE[] = L"조각 퍼즐 맞추기";
HINSTANCE g_hInstance;

// ── 비트맵 ──────────────────────────────────────────────
HBITMAP hBmp[2] = {};
BITMAP  bmpInfo[2] = {};
int     curImg = 0;      // 0: 그림1, 1: 그림2

// ── 퍼즐 보드 ────────────────────────────────────────────
int              N = 3;
std::vector<int> board;      // board[r*N+c] = 조각번호, -1 = 빈칸
int              emptyR = 0, emptyC = 0;

enum class GState { IDLE, PLAYING, FULL, ENDED };
GState gState = GState::IDLE;

// ── v/h 스트립 모드 ──────────────────────────────────────
enum class DivMode { GRID, VERT, HORIZ };
DivMode          divMode = DivMode::GRID;
std::vector<int> strip;      // 1D 스트립 순서

// ── 우클릭 제거 스택 ─────────────────────────────────────
std::vector<int> removedStack;

// ── 슬라이딩 애니메이션 ──────────────────────────────────
bool  animOn = false;
int   animFR, animFC, animTR, animTC;
float animT = 0.f;
#define TIMER_ANIM 1
static const float ANIM_SPEED = 1.0f / 18.0f;   // 18프레임 슬라이딩

// ── 마우스 드래그 ────────────────────────────────────────
bool  lmbHeld = false;
POINT lmbPt = {};
bool  dragDone = false;

// =====================================================
// 헬퍼 함수
// =====================================================

void InitBoard() {
    int total = N * N;
    board.resize(total);
    for (int i = 0; i < total - 1; i++) board[i] = i;
    board[total - 1] = -1;

    std::mt19937 rng(GetTickCount());
    for (int i = total - 1; i > 0; i--)
        std::swap(board[i], board[rng() % (i + 1)]);

    for (int i = 0; i < total; i++)
        if (board[i] == -1) { emptyR = i / N; emptyC = i % N; }

    removedStack.clear();
}

bool CheckWin() {
    for (int i = 0; i < N * N; i++)
        if (board[i] != -1 && board[i] != i) return false;
    return true;
}

void StartAnim(HWND hWnd, int fr, int fc, int tr, int tc) {
    animOn = true;
    animFR = fr; animFC = fc;
    animTR = tr; animTC = tc;
    animT = 0.f;
    SetTimer(hWnd, TIMER_ANIM, 16, nullptr);
}

void FinishAnim(HWND hWnd) {
    std::swap(board[animFR * N + animFC], board[animTR * N + animTC]);
    emptyR = animFR; emptyC = animFC;
    animOn = false;
    KillTimer(hWnd, TIMER_ANIM);
    InvalidateRect(hWnd, nullptr, FALSE);
    if (gState == GState::PLAYING && CheckWin()) {
        MessageBox(hWnd, L"퍼즐 완성! 축하합니다!", L"Game Clear",
            MB_OK | MB_ICONINFORMATION);
        gState = GState::ENDED;
    }
}

// 인접한 조각을 빈칸 방향으로 이동 시도
void TrySlide(HWND hWnd, int pr, int pc) {
    if (animOn || gState != GState::PLAYING || divMode != DivMode::GRID) return;
    if (pr < 0 || pr >= N || pc < 0 || pc >= N) return;
    if (abs(pr - emptyR) + abs(pc - emptyC) != 1) return;
    StartAnim(hWnd, pr, pc, emptyR, emptyC);
}

// 픽셀 좌표 → 셀 (r, c)
void PixToCell(const RECT& rc, int mx, int my, int& r, int& c) {
    if (rc.right == 0 || rc.bottom == 0) { r = c = 0; return; }
    c = mx / (rc.right / N);
    r = my / (rc.bottom / N);
    if (c < 0) c = 0; if (c >= N) c = N - 1;
    if (r < 0) r = 0; if (r >= N) r = N - 1;
}

// =====================================================
// 그리기 함수
// =====================================================

void DrawFull(HDC hdc, const RECT& rc) {
    if (!hBmp[curImg]) return;
    BITMAP& bi = bmpInfo[curImg];
    HDC mdc = CreateCompatibleDC(hdc);
    HBITMAP old = (HBITMAP)SelectObject(mdc, hBmp[curImg]);
    StretchBlt(hdc, 0, 0, rc.right, rc.bottom,
        mdc, 0, 0, bi.bmWidth, bi.bmHeight, SRCCOPY);
    SelectObject(mdc, old);
    DeleteDC(mdc);
}

void DrawPuzzle(HDC hdc, const RECT& rc) {
    if (!hBmp[curImg] || rc.right == 0 || rc.bottom == 0) return;
    BITMAP& bi = bmpInfo[curImg];
    int cw = rc.right / N, ch = rc.bottom / N;
    int sw = bi.bmWidth / N, sh = bi.bmHeight / N;

    HDC mdc = CreateCompatibleDC(hdc);
    HBITMAP old = (HBITMAP)SelectObject(mdc, hBmp[curImg]);

    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            int piece = board[r * N + c];
            if (piece == -1) continue;   // 빈칸은 그리지 않음

            // 화면 좌표 (애니메이션 오프셋 반영)
            int dx = c * cw, dy = r * ch;
            if (animOn && r == animFR && c == animFC) {
                dx = (int)(animFC * cw + animT * (animTC - animFC) * cw);
                dy = (int)(animFR * ch + animT * (animTR - animFR) * ch);
            }

            // 소스 이미지에서의 위치
            int sr = piece / N, sc = piece % N;
            StretchBlt(hdc, dx, dy, cw, ch,
                mdc, sc * sw, sr * sh, sw, sh, SRCCOPY);
        }
    }
    SelectObject(mdc, old);
    DeleteDC(mdc);

    // 격자선
    HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN oldP = (HPEN)SelectObject(hdc, pen);
    for (int i = 1; i < N; i++) {
        MoveToEx(hdc, i * cw, 0, nullptr);        LineTo(hdc, i * cw, rc.bottom);
        MoveToEx(hdc, 0, i * ch, nullptr);         LineTo(hdc, rc.right, i * ch);
    }
    SelectObject(hdc, oldP);
    DeleteObject(pen);
}

void DrawStrips(HDC hdc, const RECT& rc, DivMode mode) {
    if (!hBmp[curImg] || strip.empty() || rc.right == 0 || rc.bottom == 0) return;
    BITMAP& bi = bmpInfo[curImg];
    HDC mdc = CreateCompatibleDC(hdc);
    HBITMAP old = (HBITMAP)SelectObject(mdc, hBmp[curImg]);

    HPEN pen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    HPEN oldP = (HPEN)SelectObject(hdc, pen);

    if (mode == DivMode::VERT) {
        int dw = rc.right / N, sw = bi.bmWidth / N;
        for (int i = 0; i < N; i++) {
            int p = strip[i];
            StretchBlt(hdc, i * dw, 0, dw, rc.bottom, mdc, p * sw, 0, sw, bi.bmHeight, SRCCOPY);
        }
        for (int i = 1; i < N; i++) {
            MoveToEx(hdc, i * dw, 0, nullptr); LineTo(hdc, i * dw, rc.bottom);
        }
    }
    else {  // HORIZ
        int dh = rc.bottom / N, sh = bi.bmHeight / N;
        for (int i = 0; i < N; i++) {
            int p = strip[i];
            StretchBlt(hdc, 0, i * dh, rc.right, dh, mdc, 0, p * sh, bi.bmWidth, sh, SRCCOPY);
        }
        for (int i = 1; i < N; i++) {
            MoveToEx(hdc, 0, i * dh, nullptr); LineTo(hdc, rc.right, i * dh);
        }
    }

    SelectObject(hdc, oldP);
    DeleteObject(pen);
    SelectObject(mdc, old);
    DeleteDC(mdc);
}

// =====================================================
// WinMain
// =====================================================
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    g_hInstance = hInstance;

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; // 더블클릭 활성화 필수!
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
    wc.lpszClassName = CLASS_NAME;
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    RegisterClassExW(&wc);

    HWND hWnd = CreateWindowExW(0, CLASS_NAME, WND_TITLE,
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

// =====================================================
// WndProc
// =====================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static RECT rc = {};

    switch (msg)
    {
        // ──────────────────────────────────────────────────
    case WM_CREATE:
        hBmp[0] = (HBITMAP)LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
        hBmp[1] = (HBITMAP)LoadBitmap(g_hInstance, MAKEINTRESOURCE(IDB_BITMAP2));
        GetObject(hBmp[0], sizeof(BITMAP), &bmpInfo[0]);
        GetObject(hBmp[1], sizeof(BITMAP), &bmpInfo[1]);
        return 0;   // ← 반드시 return 0! (없으면 WM_KEYDOWN으로 fall-through)

        // ──────────────────────────────────────────────────
    case WM_SIZE:
        GetClientRect(hWnd, &rc);
        return 0;

        // ──────────────────────────────────────────────────
    case WM_ERASEBKGND:
        return 1;   // 더블버퍼링 사용 → 기본 배경 지우기 생략

<<<<<<< HEAD
		(HBITMAP)SelectObject(hMemDC, hBitmap); //--- 비트맵과 메모리 DC 연결하기
		// TODO: 여기에 그리기 코드 작성
		StretchBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY); //--- 윈도우 크기에 맞기 비트
		if (divide == true)
		{
			StretchBlt(hdc, rect.right / 3, 0, rect.right / 3, rect.bottom, hMemDC, 0, 0, bmp.bmWidth / 3, bmp.bmHeight, SRCCOPY);
			StretchBlt(hdc, 0, 0, rect.right / 3, rect.bottom, hMemDC, bmp.bmWidth / 3, 0, bmp.bmWidth / 3, bmp.bmHeight, SRCCOPY);
			StretchBlt(hdc, 2*rect.right / 3, 0, rect.right / 3, rect.bottom, hMemDC, 2*bmp.bmWidth / 3, 0, bmp.bmWidth / 3, bmp.bmHeight, SRCCOPY);
		}
		DeleteDC(hMemDC);
		}
		if (show2 == true)
		{
=======
        // ──────────────────────────────────────────────────
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_PICTURE_1:       curImg = 0; InvalidateRect(hWnd, nullptr, FALSE); break;
        case ID_PICTURE_2:       curImg = 1; InvalidateRect(hWnd, nullptr, FALSE); break;
        case ID_PICTUREDIVIDE_3: N = 3; break;
        case ID_PICTUREDIVIDE_4: N = 4; break;
        case ID_PICTUREDIVIDE_5: N = 5; break;
        case ID_GAME_GAMESTART:
            InitBoard();
            gState = GState::PLAYING;
            divMode = DivMode::GRID;
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        case ID_GAME_ALLPICTURESHOW:
            gState = GState::FULL;
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        case ID_GAME_ENDGAME:
            gState = GState::ENDED;
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        }
        return 0;

        // ──────────────────────────────────────────────────
    case WM_KEYDOWN:
        switch (wParam)
        {
        case 'S':
            InitBoard();
            gState = GState::PLAYING;
            divMode = DivMode::GRID;
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        case 'F':
            gState = GState::FULL;
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        case 'Q':
            PostQuitMessage(0);
            break;
        case 'V': {
            divMode = DivMode::VERT;
            strip.resize(N);
            for (int i = 0; i < N; i++) strip[i] = i;
            std::mt19937 rng(GetTickCount());
            for (int i = N - 1; i > 0; i--) std::swap(strip[i], strip[rng() % (i + 1)]);
            gState = GState::PLAYING;
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        }
        case 'H': {
            divMode = DivMode::HORIZ;
            strip.resize(N);
            for (int i = 0; i < N; i++) strip[i] = i;
            std::mt19937 rng(GetTickCount());
            for (int i = N - 1; i > 0; i--) std::swap(strip[i], strip[rng() % (i + 1)]);
            gState = GState::PLAYING;
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        }
        }
        return 0;
>>>>>>> 0b44e00947150fd6fbf6f96f199d6e42391bd322

        // ──────────────────────────────────────────────────
    case WM_LBUTTONDOWN:
        lmbHeld = true;
        dragDone = false;
        lmbPt.x = GET_X_LPARAM(lParam);
        lmbPt.y = GET_Y_LPARAM(lParam);
        SetCapture(hWnd);
        return 0;

        // ──────────────────────────────────────────────────
    case WM_MOUSEMOVE:
        if (lmbHeld && !dragDone && !animOn && gState == GState::PLAYING
            && divMode == DivMode::GRID)
        {
            int dx = GET_X_LPARAM(lParam) - (int)lmbPt.x;
            int dy = GET_Y_LPARAM(lParam) - (int)lmbPt.y;
            const int THRESH = 20;

            if (abs(dx) > abs(dy) && abs(dx) > THRESH) {
                dragDone = true;
                // 오른쪽 드래그 → 빈칸 왼쪽 조각이 오른쪽으로
                // 왼쪽 드래그  → 빈칸 오른쪽 조각이 왼쪽으로
                TrySlide(hWnd, emptyR, (dx > 0) ? emptyC - 1 : emptyC + 1);
            }
            else if (abs(dy) > abs(dx) && abs(dy) > THRESH) {
                dragDone = true;
                // 아래 드래그 → 빈칸 위쪽 조각이 아래로
                // 위 드래그   → 빈칸 아래쪽 조각이 위로
                TrySlide(hWnd, (dy > 0) ? emptyR - 1 : emptyR + 1, emptyC);
            }
        }
        return 0;

        // ──────────────────────────────────────────────────
    case WM_LBUTTONUP:
        if (lmbHeld) {
            // v/h 모드: 마우스 뗀 위치의 스트립과 누른 위치의 스트립 교환
            if (!dragDone && gState == GState::PLAYING) {
                int mx = GET_X_LPARAM(lParam);
                int my = GET_Y_LPARAM(lParam);

                if (divMode == DivMode::VERT && rc.right > 0) {
                    int cw = rc.right / N;
                    int sc = (int)lmbPt.x / cw, tc = mx / cw;
                    if (sc < 0) sc = 0; if (sc >= N) sc = N - 1;
                    if (tc < 0) tc = 0; if (tc >= N) tc = N - 1;
                    if (sc != tc) {
                        std::swap(strip[sc], strip[tc]);
                        InvalidateRect(hWnd, nullptr, FALSE);
                    }
                }
                else if (divMode == DivMode::HORIZ && rc.bottom > 0) {
                    int ch = rc.bottom / N;
                    int sr = (int)lmbPt.y / ch, tr = my / ch;
                    if (sr < 0) sr = 0; if (sr >= N) sr = N - 1;
                    if (tr < 0) tr = 0; if (tr >= N) tr = N - 1;
                    if (sr != tr) {
                        std::swap(strip[sr], strip[tr]);
                        InvalidateRect(hWnd, nullptr, FALSE);
                    }
                }
            }
            lmbHeld = false;
            dragDone = false;
            ReleaseCapture();
        }
        return 0;

        // ──────────────────────────────────────────────────
    case WM_LBUTTONDBLCLK:
        // 더블클릭한 조각을 빈칸으로 순간이동
        if (gState == GState::PLAYING && !animOn && divMode == DivMode::GRID) {
            int r, c;
            PixToCell(rc, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), r, c);
            int idx = r * N + c;
            if (board[idx] != -1) {
                std::swap(board[idx], board[emptyR * N + emptyC]);
                emptyR = r; emptyC = c;
                InvalidateRect(hWnd, nullptr, FALSE);
                if (CheckWin()) {
                    MessageBox(hWnd, L"퍼즐 완성! 축하합니다!", L"Game Clear",
                        MB_OK | MB_ICONINFORMATION);
                    gState = GState::ENDED;
                }
            }
        }
        return 0;

        // ──────────────────────────────────────────────────
    case WM_RBUTTONDOWN:
        if (gState == GState::PLAYING && divMode == DivMode::GRID) {
            int r, c;
            PixToCell(rc, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), r, c);
            int idx = r * N + c;

            if (board[idx] != -1) {
                // 조각 제거 → 스택에 저장
                removedStack.push_back(board[idx]);
                board[idx] = -1;
                emptyR = r; emptyC = c;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            else {
                // 빈칸에 마지막으로 제거한 조각 복원
                if (removedStack.empty()) {
                    MessageBox(hWnd, L"추가할 그림이 없습니다!", L"오류",
                        MB_OK | MB_ICONERROR);
                }
                else {
                    board[idx] = removedStack.back();
                    removedStack.pop_back();
                    // 빈칸 위치 재탐색
                    emptyR = emptyC = -1;
                    for (int i = 0; i < N * N; i++)
                        if (board[i] == -1) { emptyR = i / N; emptyC = i % N; break; }
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
            }
        }
        return 0;

        // ──────────────────────────────────────────────────
    case WM_TIMER:
        if (wParam == TIMER_ANIM && animOn) {
            animT += ANIM_SPEED;
            if (animT >= 1.0f) {
                animT = 1.0f;
                FinishAnim(hWnd);          // 완료: 보드 업데이트
            }
            else {
                InvalidateRect(hWnd, nullptr, FALSE);
            }
        }
        return 0;

        // ──────────────────────────────────────────────────
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rc);

        // 더블 버퍼링 (깜빡임 방지)
        HDC     bufDC = CreateCompatibleDC(hdc);
        HBITMAP bufBmp = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
        HBITMAP oldBmp = (HBITMAP)SelectObject(bufDC, bufBmp);
        FillRect(bufDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));

        switch (gState) {
        case GState::FULL:
            DrawFull(bufDC, rc);
            break;
        case GState::PLAYING:
        case GState::ENDED:
            if (divMode == DivMode::GRID)   DrawPuzzle(bufDC, rc);
            else                             DrawStrips(bufDC, rc, divMode);
            break;
        default:  // IDLE
            if (hBmp[curImg]) DrawFull(bufDC, rc);
            break;
        }

        BitBlt(hdc, 0, 0, rc.right, rc.bottom, bufDC, 0, 0, SRCCOPY);
        SelectObject(bufDC, oldBmp);
        DeleteObject(bufBmp);
        DeleteDC(bufDC);

        EndPaint(hWnd, &ps);
        return 0;
    }

                 // ──────────────────────────────────────────────────
    case WM_DESTROY:
        if (hBmp[0]) DeleteObject(hBmp[0]);
        if (hBmp[1]) DeleteObject(hBmp[1]);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}