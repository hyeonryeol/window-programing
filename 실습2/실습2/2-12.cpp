#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <vector>
#include <random>
#include <ctime>

constexpr int COLS = 40;
constexpr int ROWS = 40;

// ===== 타입 =====
enum class ShapeType { VERTICAL, RECT, CIRCLE, ELLIPSE };
enum class CellType { EMPTY, BLOCK, COLOR, SHRINK, GROW, SHAPE, GOAL };

struct Player
{
    int x = 0, y = 0;            // 칸 좌표
    COLORREF color = RGB(0, 0, 0);
    float scale = 1.0f;          // 0.5 ~ 2.0 정도
    ShapeType shape = ShapeType::CIRCLE;
    int shapeRemain = 0;         // 모양 변경 유지 턴 수
    ShapeType originalShape = ShapeType::CIRCLE;
};

struct Cell
{
    CellType type = CellType::EMPTY;
    int level = 0;               // shrink/grow 강도(1,2,3)
    COLORREF fixedColor = RGB(255, 255, 255); // 색 변경칸 배경색 표시용
};

// ===== 전역 =====
Cell g_board[ROWS][COLS];
Player g_players[2];
int g_turn = 0; // 0 or 1

POINT g_goal = { COLS / 2, ROWS - 2 };
bool g_gameOver = false;
int g_winner = -1;

std::mt19937 g_rng((unsigned)time(nullptr));

int RandInt(int a, int b) {
    std::uniform_int_distribution<int> dist(a, b);
    return dist(g_rng);
}

COLORREF RandomStoneColor() {
    static COLORREF palette[] = {
        RGB(220,20,60), RGB(30,144,255), RGB(34,139,34), RGB(255,140,0),
        RGB(138,43,226), RGB(255,215,0), RGB(0,206,209), RGB(255,105,180)
    };
    return palette[RandInt(0, (int)(sizeof(palette) / sizeof(palette[0])) - 1)];
}

ShapeType RandomShape() {
    return (ShapeType)RandInt(0, 3);
}

void InitBoard()
{
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            g_board[y][x] = {};
        }
    }

    // 목표칸
    g_board[g_goal.y][g_goal.x].type = CellType::GOAL;

    // 최소 20칸 이상 특수칸: 여기선 30칸 배치
    auto placeUnique = [&](CellType t, int cnt, int levelMin = 1, int levelMax = 3) {
        int placed = 0;
        while (placed < cnt) {
            int x = RandInt(0, COLS - 1);
            int y = RandInt(0, ROWS - 1);

            // 시작점/목표점/이미 배치된 곳 제외
            if ((x == 0 && y == 0) || (x == COLS - 1 && y == 0) || (x == g_goal.x && y == g_goal.y)) continue;
            if (g_board[y][x].type != CellType::EMPTY) continue;

            g_board[y][x].type = t;
            if (t == CellType::SHRINK || t == CellType::GROW) {
                g_board[y][x].level = RandInt(levelMin, levelMax);
            }
            if (t == CellType::COLOR) {
                g_board[y][x].fixedColor = RandomStoneColor();
            }
            ++placed;
        }
        };

    placeUnique(CellType::BLOCK, 10);
    placeUnique(CellType::COLOR, 6);
    placeUnique(CellType::SHRINK, 5);
    placeUnique(CellType::GROW, 5);
    placeUnique(CellType::SHAPE, 4);
}

void InitPlayers()
{
    // 좌상단, 우상단 시작
    g_players[0].x = 0;         g_players[0].y = 0;
    g_players[1].x = COLS - 1;  g_players[1].y = 0;

    for (int i = 0; i < 2; ++i) {
        g_players[i].color = RandomStoneColor();
        g_players[i].shape = RandomShape();
        g_players[i].originalShape = g_players[i].shape;
        g_players[i].scale = (float)(RandInt(80, 140) / 100.0); // 0.8~1.4
        g_players[i].shapeRemain = 0;
    }
}

void ApplyCellEffect(Player& p, const Cell& c)
{
    switch (c.type)
    {
    case CellType::COLOR:
        p.color = RandomStoneColor(); // 이미지 요구: 랜덤 색상 변경
        break;

    case CellType::SHRINK:
    {
        // 1:10%, 2:30%, 3:50% 감소
        float f = (c.level == 1 ? 0.9f : (c.level == 2 ? 0.7f : 0.5f));
        p.scale *= f;
        p.scale = std::max(0.25f, p.scale);
        break;
    }

    case CellType::GROW:
    {
        // 1:10%, 2:30%, 3:50% 증가
        float f = (c.level == 1 ? 1.1f : (c.level == 2 ? 1.3f : 1.5f));
        p.scale *= f;
        p.scale = std::min(3.0f, p.scale);
        break;
    }

    case CellType::SHAPE:
        p.originalShape = p.shape;
        p.shape = RandomShape();
        p.shapeRemain = 4; // 예: 4번 이동 후 원상복구
        break;

    default:
        break;
    }
}

void AdvanceShapeTimer(Player& p)
{
    if (p.shapeRemain > 0) {
        p.shapeRemain--;
        if (p.shapeRemain == 0) {
            p.shape = p.originalShape;
        }
    }
}

bool CanMoveTo(int nx, int ny)
{
    if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) return false;
    if (g_board[ny][nx].type == CellType::BLOCK) return false; // 장애물 통과 불가
    return true;
}

void TryMoveCurrentPlayer(int dx, int dy)
{
    if (g_gameOver) return;

    Player& p = g_players[g_turn];
    int nx = p.x + dx;
    int ny = p.y + dy;

    if (!CanMoveTo(nx, ny)) return;

    p.x = nx;
    p.y = ny;

    // 칸 효과 적용
    ApplyCellEffect(p, g_board[p.y][p.x]);

    // 목표 도착 판정
    if (p.x == g_goal.x && p.y == g_goal.y) {
        g_gameOver = true;
        g_winner = g_turn;
        return;
    }

    // 모양 복귀 턴 감소
    AdvanceShapeTimer(p);

    // 턴 넘기기
    g_turn = 1 - g_turn;
}

void DrawCellBackground(HDC hdc, int x, int y, int cellW, int cellH)
{
    const Cell& c = g_board[y][x];
    HBRUSH brush = nullptr;

    switch (c.type) {
    case CellType::BLOCK:  brush = CreateSolidBrush(RGB(255, 0, 0)); break;      // 빨강
    case CellType::COLOR:  brush = CreateSolidBrush(c.fixedColor); break;         // 색 변경칸
    case CellType::SHRINK: brush = CreateSolidBrush(RGB(170, 200, 255)); break;   // 축소칸
    case CellType::GROW:   brush = CreateSolidBrush(RGB(200, 255, 200)); break;   // 확대칸
    case CellType::SHAPE:  brush = CreateSolidBrush(RGB(220, 165, 255)); break;
    case CellType::GOAL:   brush = CreateSolidBrush(RGB(0, 0, 0)); break;
    default: break;
    }

    RECT r{ x * cellW, y * cellH, (x + 1) * cellW, (y + 1) * cellH };

    if (brush) {
        FillRect(hdc, &r, brush);
        DeleteObject(brush);
    }

    // ===== 축소/확대 칸에 숫자(1,2,3) 표시 =====
    if (c.type == CellType::SHRINK || c.type == CellType::GROW) {
        SetBkMode(hdc, TRANSPARENT);

        // 가독성 위해 글자색 다르게
        if (c.type == CellType::SHRINK) SetTextColor(hdc, RGB(0, 0, 0));
        else                            SetTextColor(hdc, RGB(0, 0, 0));

        wchar_t num[8];
        wsprintf(num, L"%d", c.level); // 1,2,3

        DrawText(hdc, num, -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}
void DrawStone(HDC hdc, const Player& p, int cellW, int cellH)
{
    int cx = p.x * cellW + cellW / 2;
    int cy = p.y * cellH + cellH / 2;

    int base = std::min(cellW, cellH) / 2 - 2;
    int r = (int)(base * p.scale);
    r = std::max(2, r);

    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH brush = CreateSolidBrush(p.color);
    HGDIOBJ oldPen = SelectObject(hdc, pen);
    HGDIOBJ oldBrush = SelectObject(hdc, brush);

    switch (p.shape)
    {
    case ShapeType::CIRCLE:
        Ellipse(hdc, cx - r, cy - r, cx + r, cy + r);
        break;
    case ShapeType::ELLIPSE:
        Ellipse(hdc, cx - r, cy - (int)(r * 0.65f), cx + r, cy + (int)(r * 0.65f));
        break;
    case ShapeType::RECT:
        Rectangle(hdc, cx - r, cy - r, cx + r, cy + r);
        break;
    case ShapeType::VERTICAL:
        Rectangle(hdc, cx - (int)(r * 0.45f), cy - r, cx + (int)(r * 0.45f), cy + r);
        break;
    }

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

void DrawGrid(HDC hdc, int width, int height)
{
    int cellW = std::max(1, width / COLS);
    int cellH = std::max(1, height / ROWS);

    // 배경칸 먼저
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            DrawCellBackground(hdc, x, y, cellW, cellH);
        }
    }

    // 격자
    HPEN gridPen = CreatePen(PS_SOLID, 1, RGB(40, 40, 40));
    HGDIOBJ oldPen = SelectObject(hdc, gridPen);

    for (int x = 0; x <= COLS; ++x) {
        int px = x * cellW;
        MoveToEx(hdc, px, 0, nullptr);
        LineTo(hdc, px, ROWS * cellH);
    }
    for (int y = 0; y <= ROWS; ++y) {
        int py = y * cellH;
        MoveToEx(hdc, 0, py, nullptr);
        LineTo(hdc, COLS * cellW, py);
    }

    SelectObject(hdc, oldPen);
    DeleteObject(gridPen);

    // 플레이어
    DrawStone(hdc, g_players[0], cellW, cellH);
    DrawStone(hdc, g_players[1], cellW, cellH);

    // 안내 텍스트
    SetBkMode(hdc, TRANSPARENT);
    wchar_t info[256];
    if (!g_gameOver) {
        wsprintf(info, L"Turn: P%d   Controls: P1(WASD), P2(Arrow)  - 번갈아 이동", g_turn + 1);
    }
    else {
        wsprintf(info, L"Game Over! Winner: P%d  (R: restart)", g_winner + 1);
    }
    TextOut(hdc, 8, 8, info, lstrlen(info));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        InitBoard();
        InitPlayers();
        return 0;

    case WM_KEYDOWN:
    {
        if (wParam == 'R') {
            g_gameOver = false;
            g_winner = -1;
            g_turn = 0;
            InitBoard();
            InitPlayers();
            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;
        }

        // 현재 턴 플레이어만 조작 가능
        // P1: WASD, P2: 방향키
        if (g_turn == 0) {
            if (wParam == 'W') TryMoveCurrentPlayer(0, -1);
            else if (wParam == 'S') TryMoveCurrentPlayer(0, +1);
            else if (wParam == 'A') TryMoveCurrentPlayer(-1, 0);
            else if (wParam == 'D') TryMoveCurrentPlayer(+1, 0);
        }
        else {
            if (wParam == 'I') TryMoveCurrentPlayer(0, -1);
            else if (wParam == 'K') TryMoveCurrentPlayer(0, +1);
            else if (wParam == 'J') TryMoveCurrentPlayer(-1, 0);
            else if (wParam == 'L') TryMoveCurrentPlayer(+1, 0);
        }

        InvalidateRect(hwnd, nullptr, TRUE);
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rc;
        GetClientRect(hwnd, &rc);
        DrawGrid(hdc, rc.right - rc.left, rc.bottom - rc.top);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"GridWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, L"Practice 2-12: 40x40 Board",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 900,
        nullptr, nullptr, hInstance, nullptr
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}