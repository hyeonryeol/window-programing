#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"windows program";

// 한 번의 출력 정보를 담는 구조체 
struct DrawInfo
{
    int x, y;           // 출력 위치
    int n;              // 출력할 숫자
    int count;          // 가로/세로 반복 횟수
    COLORREF textColor; // 글자 색상
    COLORREF bkColor;   // 배경 색상
   
};

DrawInfo history[10]; // 이전 출력 기록 a키용
int historyCount = 0;  // 기록 개수
bool showAll = false;  // a키 눌렀는지 여부

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

// 새 랜덤값 생성
DrawInfo makeRandom()
{
    DrawInfo info;
    info.x = rand() % 600;
    info.y = rand() % 400;
    info.n = rand() % 200;
    info.count = rand() % 16 + 5; // 5~20
    info.textColor = RGB(rand() % 256, rand() % 256, rand() % 256);
    info.bkColor = RGB(rand() % 256, rand() % 256, rand() % 256);
    return info;
}

//DrawInfo 하나를 화면에 그리기 
void drawOne(HDC hDC, DrawInfo& info)
{
    // 숫자를 문자열로 변환
    TCHAR numStr[20];
    wsprintf(numStr, L"%d", info.n);
    int numLen = lstrlen(numStr);

    // 글자 하나의 크기 측정
    SIZE sz;
    GetTextExtentPoint32(hDC, numStr, numLen, &sz);
    int charW = sz.cx; // 숫자 문자열 너비
    int charH = sz.cy; // 글자 높이

    // 한 줄 문자열 만들기 (count번 반복)
    
    TCHAR rowStr[500];
    rowStr[0] = L'\0';
    for (int i = 0; i < info.count; i++)
        lstrcat(rowStr, numStr);
    int rowLen = lstrlen(rowStr);

    // 배경 사각형 그리기
    int rectW = charW * info.count;
    int rectH = charH * info.count;
    HBRUSH hBrush = CreateSolidBrush(info.bkColor);
    RECT r = { info.x, info.y, info.x + rectW, info.y + rectH };
    FillRect(hDC, &r, hBrush);
    DeleteObject(hBrush);

    // 텍스트 출력 (count줄)
    SetTextColor(hDC, info.textColor);
    SetBkMode(hDC, TRANSPARENT);
    for (int i = 0; i < info.count; i++)
        TextOut(hDC, info.x, info.y + i * charH, rowStr, rowLen);

    // 왼쪽 아래에 값 표시
    TCHAR info_str[100];
    wsprintf(info_str, L"%d %d %d %d", info.x, info.y, info.n, info.count);
    SetTextColor(hDC, RGB(0, 0, 0));
    TextOut(hDC, info.x, info.y + rectH + 2, info_str, lstrlen(info_str));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    srand(time(NULL));

    // 첫 번째 랜덤값 생성
    history[historyCount++] = makeRandom();

    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInst = hInstance;

    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc = WndProc;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = lpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&WndClass);

    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&Message, 0, 0, 0))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }
    return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hDC;

    switch (iMessage)
    {
    case WM_CHAR:
    {
        TCHAR ch = (TCHAR)wParam;

        if (ch == 'q') // 종료
        {
            PostQuitMessage(0);
        }
        else if (ch == 'r') // 리셋 - 새 랜덤값으로 교체
        {
            showAll = false;
            historyCount = 0;
            history[historyCount++] = makeRandom();
            InvalidateRect(hWnd, NULL, TRUE);
        }
        else if (ch == 'a') // 이전 출력 누적
        {
            showAll = true;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        return 0;
    }

    case WM_KEYDOWN:
    {
        if (wParam == VK_RETURN) // 엔터키 - 새 랜덤값 추가
        {
            if (historyCount < 10)
                history[historyCount++] = makeRandom();
            showAll = false; // 엔터키는 최신 것만 보여줌
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    }

    case WM_PAINT:
    {
        hDC = BeginPaint(hWnd, &ps);

        if (showAll) // a키: 지금까지 모든 출력
        {
            for (int i = 0; i < historyCount; i++)
                drawOne(hDC, history[i]);
        }
        else // 엔터키: 최신 것만
        {
            drawOne(hDC, history[historyCount - 1]);
        }

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, iMessage, wParam, lParam);
}