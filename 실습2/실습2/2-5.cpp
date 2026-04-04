#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"windows program 3";

// ── 전역 변수 ─────────────────────────────────────────────────
int startX, startY;         // 시작 위치
COLORREF textColor;         // 텍스트 색상
TCHAR lines[5][21];         // 최대 5줄, 각 20자
int lineLens[5];            // 각 줄 길이
int curLine = 0;            // 현재 줄
int totalLines = 1;         // 총 줄 수

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    srand(time(NULL));

    // 랜덤 위치/색상 초기화
    startX = rand() % 600;       // 0~600
    startY = rand() % 500;       // 0~500
    int r = rand() % 256;
    int g = rand() % 256;
    int b = rand() % 256;
    textColor = RGB(r, g, b);

    // 줄 초기화
    for (int i = 0; i < 5; i++) {
        lineLens[i] = 0;
        lines[i][0] = L'\0';
    }

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

    while (GetMessage(&Message, 0, 0, 0)) {
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

        if (ch == 27) // ESC → 종료
        {
            PostQuitMessage(0);
        }
        else if (ch == '\b') // 백스페이스 → 마지막 문자 삭제
        {
            if (lineLens[curLine] > 0)
            {
                lineLens[curLine]--;
                lines[curLine][lineLens[curLine]] = L'\0';
                InvalidateRect(hWnd, NULL, TRUE);
            }
            // lineLens == 0 이면 더 이상 삭제 불가
        }
        else if (ch == '\r') // 엔터
        {
            if (curLine < 4) // 마지막 줄이 아니면
            {
                int nextLine = curLine + 1;

                //  현재 줄 내용을 다음 줄로 이동
                lineLens[nextLine] = lineLens[curLine];
                for (int i = 0; i <= lineLens[curLine]; i++)
                    lines[nextLine][i] = lines[curLine][i];

                //  현재 줄은 비우기
                lineLens[curLine] = 0;
                lines[curLine][0] = L'\0';

                curLine = nextLine;
                if (curLine >= totalLines) totalLines = curLine + 1;
            }
            // 마지막 줄(4)이면 그 자리 유지
            InvalidateRect(hWnd, NULL, TRUE);
        }
        else if (isalpha(ch)) // 알파벳 → 현재 줄에 추가
        {
            if (lineLens[curLine] < 20) // 20자 미만이면 현재 줄에 추가
            {
                lines[curLine][lineLens[curLine]++] = ch;
                lines[curLine][lineLens[curLine]] = L'\0';
            }
            else // 20자 넘으면 다음 줄로 자동 이동
            {
                if (curLine < 4)
                {
                    curLine++;
                    if (curLine >= totalLines) totalLines = curLine + 1;
                    lines[curLine][0] = ch;
                    lineLens[curLine] = 1;
                    lines[curLine][1] = L'\0';
                }
                // 5줄 꽉 찼으면 더 이상 추가 안 함
            }
            InvalidateRect(hWnd, NULL, TRUE);
        }
        return 0;
    }

    case WM_PAINT:
    {
        hDC = BeginPaint(hWnd, &ps);
        SetTextColor(hDC, textColor);
        SetBkMode(hDC, TRANSPARENT);

        // 각 줄 텍스트 출력
        for (int i = 0; i < totalLines; i++)
        {
            int drawY = startY + i * 20;
            TextOut(hDC, startX, drawY, lines[i], lineLens[i]);
        }

        // 커서 그리기 (현재 줄 끝에 세로선)
        SIZE sz;
        GetTextExtentPoint32(hDC, lines[curLine], lineLens[curLine], &sz);
        int cursorX = startX + sz.cx;
        int cursorY = startY + curLine * 20;

        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
        HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
        MoveToEx(hDC, cursorX, cursorY, NULL);
        LineTo(hDC, cursorX, cursorY + 16);
        SelectObject(hDC, hOldPen);
        DeleteObject(hPen);

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, iMessage, wParam, lParam);
}