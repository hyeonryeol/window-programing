#include <string>
#include <windows.h>
#include <cstdlib>
#include <ctime>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

const int ROWS = 40;
const int COLS = 40;
const int CELL = 20;

HBRUSH brush;     // circle
HBRUSH tribrush;  // triangle
HBRUSH rectbrush; // rectangle
HPEN pen;
int selectedIndex = -1; // -1: 선택 없음, 0~9: 1~10번 도형
enum ShapeType { CIRCLE, TRIANGLE, RECTANGLE };
struct Shape {
    ShapeType type;
    int row;
    int col;
    int size;
    COLORREF color;
    ShapeType originalType;
    COLORREF originalColor;
    bool changedByC;
};
Shape shapes[1000];
int shapeCount = 0;

void DrawNumberCenter(HDC hdc, int x, int y, int n)
{
    std::wstring s = std::to_wstring(n);
    SIZE sz{};
    GetTextExtentPoint32W(hdc, s.c_str(), (int)s.size(), &sz);
    TextOutW(hdc, x - sz.cx / 2, y - sz.cy / 2, s.c_str(), (int)s.size());
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

    HWND hWnd = CreateWindowExW(
        0, CLASS_NAME, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 1000,
        nullptr, nullptr, hInstance, nullptr
    );
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
        srand((unsigned)time(NULL));
        brush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
        tribrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
        rectbrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
        pen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0)); // 빨간 테두리, 두께 3

        return 0;

    case WM_KEYDOWN:
        if ((wParam == 'E' || wParam == 'T' || wParam == 'R') ) {
            Shape s;
            s.row = rand() % ROWS;
            s.col = rand() % COLS;
            s.size = 1;
            s.color = RGB(rand() % 256, rand() % 256, rand() % 256); // 각 도형 고유색
            if (wParam == 'E') s.type = CIRCLE;
            else if (wParam == 'T') s.type = TRIANGLE;
            else s.type = RECTANGLE;
            s.originalType = s.type;
            s.originalColor = s.color;
            s.changedByC = false;

            // 최대 10개 유지
            if (shapeCount < 10) {
                shapes[shapeCount++] = s;
            }
            else {
                // 1번 삭제 + 앞으로 당기기
                for (int i = 1; i < 10; ++i) {
                    shapes[i - 1] = shapes[i];
                }
                // 새 도형을 10번 위치에
                shapes[9] = s;
            }

            // 선택 인덱스 보정 (선택이 있으면 한 칸 당겨짐)
            if (selectedIndex > 0) selectedIndex--;
            else if (selectedIndex == 0) selectedIndex = -1; // 삭제된 도형이 선택돼있던 경우
            
           
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        if (wParam >= '1' && wParam <= '9') {
            int idx = (int)(wParam - '1'); // '1'->0, '2'->1 ...
            selectedIndex = (idx < shapeCount) ? idx : -1;
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        if (wParam == '0') { // 10번 도형
            selectedIndex = (shapeCount >= 10) ? 9 : -1;
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        if (selectedIndex >= 0 && selectedIndex < shapeCount) 
        {
            if (wParam == VK_LEFT)
            {
                if (shapes[selectedIndex].col > 0)
                {
                    shapes[selectedIndex].col--;
                }
                else
                {
                    for (int i = 0; i < 39; ++i)
                    {
                    shapes[selectedIndex].col++;
                    }
                }
            }
           
            if (wParam == VK_RIGHT)
            {
                if (shapes[selectedIndex].col < COLS - 1)
                {
                shapes[selectedIndex].col++;
                }
                else
                {
                    for (int i = 0; i < 39; ++i)
                    {
                        shapes[selectedIndex].col--;
                    }
                }
            }

            if (wParam == VK_UP)
            {
                if (shapes[selectedIndex].row > 0)
                {
                shapes[selectedIndex].row--;
                }
                else
                {
                    for (int i = 0; i < 39; ++i)
                    {
                        shapes[selectedIndex].row++;
                    }
                }

            }
            if (wParam == VK_DOWN )
            {
                if(shapes[selectedIndex].row < ROWS - 1)
                {
                    shapes[selectedIndex].row++;
                }
                else
                {
                    for (int i = 0; i < 39; ++i)
                    {
                        shapes[selectedIndex].row--;
                    }
                }
            }
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        if (wParam == VK_OEM_PLUS)
        {
            if (selectedIndex >= 0 && selectedIndex < shapeCount) 
            {
                if (shapes[selectedIndex].size < 5) shapes[selectedIndex].size++;
                InvalidateRect(hWnd, nullptr, TRUE);
            }
        }
        if (wParam == VK_OEM_MINUS) 
        {
            if (selectedIndex >= 0 && selectedIndex < shapeCount) {
                if (shapes[selectedIndex].size > 1) shapes[selectedIndex].size--;
                InvalidateRect(hWnd, nullptr, TRUE);
            }
        }
        if (wParam == 'P') {   // Clear
            shapeCount = 0;    // 모든 도형 삭제
            selectedIndex = -1; // 선택 해제
            InvalidateRect(hWnd, nullptr, TRUE);
        }
        if (wParam == 'C') {
            if (selectedIndex >= 0 && selectedIndex < shapeCount) {
                ShapeType targetType = shapes[selectedIndex].type;
                COLORREF selectedColor = shapes[selectedIndex].color;

                // 현재 같은 모양 그룹이 이미 C로 바뀐 상태인지 확인
                bool alreadyChanged = false;
                for (int i = 0; i < shapeCount; i++) {
                    if (shapes[i].changedByC) {
                        alreadyChanged = true;
                        break;
                    }
                }

                if (!alreadyChanged) {
                    // 1차: 같은 모양 -> 다른 모양 + 선택 도형 색으로 변경
                    for (int i = 0; i < shapeCount; i++) {
                        if (shapes[i].type == targetType) {
                            shapes[i].originalType = shapes[i].type;
                            shapes[i].originalColor = shapes[i].color;
                            shapes[i].changedByC = true;

                            // "기존 모양 외 다른 모양"으로 변경
                            if (targetType == CIRCLE) {
                                shapes[i].type = (rand() % 2 == 0) ? TRIANGLE : RECTANGLE;
                            }
                            else if (targetType == TRIANGLE) {
                                shapes[i].type = (rand() % 2 == 0) ? CIRCLE : RECTANGLE;
                            }
                            else { // RECTANGLE
                                shapes[i].type = (rand() % 2 == 0) ? CIRCLE : TRIANGLE;
                            }

                            // 선택 도형 색으로 통일
                            shapes[i].color = selectedColor;
                        }
                    }
                }
                else {
                    // 2차: 원래 모양/원래 색 복구
                    for (int i = 0; i < shapeCount; i++) {
                        if (shapes[i].changedByC) {
                            shapes[i].type = shapes[i].originalType;
                            shapes[i].color = shapes[i].originalColor;
                            shapes[i].changedByC = false;
                        }
                    }
                }

                InvalidateRect(hWnd, nullptr, TRUE);
            }
        }
        if (wParam == 'D') {
            if (selectedIndex >= 0 && selectedIndex < shapeCount) {
                // 선택 도형 뒤에 있는 것들을 한 칸씩 앞으로
                for (int i = selectedIndex + 1; i < shapeCount; ++i) {
                    shapes[i - 1] = shapes[i];
                }

                shapeCount--;

                // 선택 처리: 삭제 후 같은 번호 위치를 계속 선택
                // (예: 2번 삭제 -> 원래 3번이 2번이 되므로 그걸 선택)
                if (shapeCount == 0) {
                    selectedIndex = -1; // 아무것도 없음
                }
                else if (selectedIndex >= shapeCount) {
                    selectedIndex = shapeCount - 1; // 마지막 삭제한 경우
                }
                // 그 외(selectedIndex < shapeCount)는 그대로 유지하면 자동으로 "당겨진 다음 도형" 선택됨

                InvalidateRect(hWnd, nullptr, TRUE);
            }
        }
        if (wParam == 'Q') PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        int startX = 20, startY = 20;

        for (int i = 0; i <= COLS; ++i) {
            MoveToEx(hdc, startX, startY + i * CELL, NULL);
            LineTo(hdc, startX + COLS * CELL, startY + i * CELL);
        }
        for (int i = 0; i <= ROWS; ++i) {
            MoveToEx(hdc, startX + i * CELL, startY, NULL);
            LineTo(hdc, startX + i * CELL, startY + ROWS * CELL);
        }

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 0, 0));

        for (int i = 0; i < shapeCount; i++) {
            int cx = startX + shapes[i].col * CELL + CELL / 2;
            int cy = startY + shapes[i].row * CELL + CELL / 2;
            int half = (CELL - 4) * shapes[i].size / 2; // 크기 반영

            int left = cx - half;
            int top = cy - half;
            int right = cx + half;
            int bottom = cy + half;
            HPEN oldPen = nullptr;
            if (i == selectedIndex) 
            { // 1번 도형만 강조
                oldPen = (HPEN)SelectObject(hdc, pen);
            }
            HBRUSH shapeBrush = CreateSolidBrush(shapes[i].color);
            HBRUSH old = (HBRUSH)SelectObject(hdc, shapeBrush);

            if (shapes[i].type == CIRCLE) {
               
                Ellipse(hdc, left, top, right, bottom);
               
            }
            else if (shapes[i].type == TRIANGLE) {
               
                int midX = (left + right) / 2;
                POINT tri[3] = { {midX, top}, {left, bottom}, {right, bottom} };
                Polygon(hdc, tri, 3);
               
            }
            else {
                
                Rectangle(hdc, left, top, right, bottom);
               
            }
            if (oldPen) {
                SelectObject(hdc, oldPen); // 반드시 복구!
            }
            DrawNumberCenter(hdc, (left + right) / 2, (top + bottom) / 2, i + 1);
            SelectObject(hdc, old);
            DeleteObject(shapeBrush);
        }
        
        
        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        DeleteObject(pen);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}