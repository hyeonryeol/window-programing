#include <windows.h> // 윈도우 API 기본 헤더
#include <tchar.h>   // TCHAR, wsprintf 등 문자열 처리
#include <time.h>    // time() - 랜덤 시드용
#include <stdlib.h>  // rand(), srand()

HINSTANCE g_hlnst;
LPCTSTR IpszClass = L"Window Class Name";
LPCTSTR IpszWindowName = L"windows program 3";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessgae, WPARAM wParam, LPARAM IParam);

int WINAPI WinMain(HINSTANCE hlnstance, HINSTANCE hPrevlnstance, LPSTR IpszCmdParam, int nCmdShow)
{
    srand(time(NULL)); // 랜덤 시드 초기화 (매번 다른 랜덤값)

    HWND hWND;           // 창 핸들
    MSG Message;         // 메시지 구조체
    WNDCLASSEX WndClass; // 윈도우 설계도
    g_hlnst = hlnstance;

    // ── 윈도우 설계도 작성 ───────────────────────────────────
    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_HREDRAW | CS_VREDRAW;        // 크기 바뀌면 다시 그리기
    WndClass.lpfnWndProc = (WNDPROC)WndProc;               // 이벤트 처리 함수 지정
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hlnstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); // 기본 아이콘
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);     // 기본 화살표 커서
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 배경 흰색
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = IpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&WndClass); // 설계도 등록

    // ── 실제 창 생성 ─────────────────────────────────────────
    hWND = CreateWindow(
        IpszClass, IpszWindowName,
        WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_HSCROLL | WS_VSCROLL | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
        100, 100, // 창 시작 위치
        800, 600, // 창 크기
        NULL, (HMENU)NULL, hlnstance, NULL
    );

    ShowWindow(hWND, nCmdShow); // 창 보여주기
    UpdateWindow(hWND);         // 창 즉시 그리기

    // ── 메시지 루프 (프로그램이 살아있는 동안 반복) ──────────
    while (GetMessage(&Message, 0, 0, 0))
    {
        TranslateMessage(&Message); // 키 입력 → WM_CHAR 변환
        DispatchMessage(&Message);  // WndProc으로 메시지 전달
    }
    return Message.wParam;
}

// ── 색상 반환 함수 ────────────────────────────────────────────
// index % 5 에 따라 5가지 색상 중 하나를 반환
COLORREF GetColor(int index)
{
    switch (index % 5)
    {
    case 0: return RGB(255, 0, 0);   // 빨강
    case 1: return RGB(0, 255, 0);   // 초록
    case 2: return RGB(0, 0, 255);   // 파랑
    case 3: return RGB(255, 0, 255); // 보라
    case 4: return RGB(255, 165, 0); // 주황
    }
    return RGB(0, 0, 0); // 기본 검정
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
    PAINTSTRUCT ps;
    HDC hDC;
    RECT rect;
    int x = 0, y = 0;
    TCHAR lpOut[100];

    int n = rand() % 10; // 0~9 랜덤값 → 홀수/짝수로 색상 모드 결정

    switch (iMessage)
    {
    case WM_PAINT:
    {
        hDC = BeginPaint(hWnd, &ps); // 그리기 시작

        RECT client;
        GetClientRect(hWnd, &client); // 창 크기 가져오기

        TCHAR lpOut[100];

        if (n % 2 == 1) // ── 홀수: 단(열) 기준으로 색상 변경 ──────────
        {
            int garo = rand() % 5 + 2; // 가로 등분 수 랜덤 (2~6)
            int sero = 600 / 2;        // 세로 절반 y좌표
            int cellWidth = 800 / garo;              // 셀 너비
            int cellHeight = (sero - client.top) / 9; // 셀 높이
            int startDan = 2;              // 시작 단
            int endDan = startDan + garo - 1; // 마지막 단

            // 위쪽 절반: 오름차순 단 출력 (2단, 3단, ...)
            for (int c = 0; c < garo; ++c)
            {
                int dan = startDan + c;
                COLORREF color = GetColor(c); // 열(단) 기준 색상
                SetTextColor(hDC, color);

                for (int i = 1; i <= 9; ++i) // 1~9 행
                {
                    RECT r;
                    r.left = client.left + c * cellWidth;
                    r.right = r.left + cellWidth;
                    r.top = client.top + (i - 1) * cellHeight;
                    r.bottom = r.top + cellHeight;

                    wsprintf(lpOut, L"%d x %d = %d", dan, i, dan * i);
                    DrawText(hDC, lpOut, lstrlen(lpOut), &r,
                        DT_SINGLELINE | DT_CENTER | DT_VCENTER);
                }
            }

            // 아래쪽 절반: 내림차순 단 출력 (..., 3단, 2단)
            for (int c = 0; c < garo; ++c)
            {
                int dan = endDan - c; // 역순
                COLORREF color = GetColor(c); // 열(단) 기준 색상
                SetTextColor(hDC, color);

                for (int i = 1; i <= 9; ++i)
                {
                    RECT r;
                    r.left = client.left + c * cellWidth;
                    r.right = r.left + cellWidth;
                    r.top = sero + (i - 1) * cellHeight; // sero 아래부터
                    r.bottom = r.top + cellHeight;

                    wsprintf(lpOut, L"%d x %d = %d", dan, i, dan * i);
                    DrawText(hDC, lpOut, lstrlen(lpOut), &r,
                        DT_SINGLELINE | DT_CENTER | DT_VCENTER);
                }
            }

            // 가로 등분 수 상단 출력
            RECT info = { 0, 0, client.right, 30 };
            wsprintf(lpOut, L"가로 등분: %d", garo);
            DrawText(hDC, lpOut, lstrlen(lpOut), &info,
                DT_SINGLELINE | DT_CENTER | DT_VCENTER);

            EndPaint(hWnd, &ps);
            break;
        }
        else if (n % 2 == 0) // ── 짝수: 행(줄) 기준으로 색상 변경 ────────
        {
            int garo = rand() % 5 + 2;
            int sero = 600 / 2;
            int cellWidth = 800 / garo;
            int cellHeight = (sero - client.top) / 9;
            int startDan = 2;
            int endDan = startDan + garo - 1;

            // 위쪽 절반
            for (int c = 0; c < garo; ++c)
            {
                int dan = startDan + c;

                for (int i = 1; i <= 9; ++i)
                {
                    RECT r;
                    r.left = client.left + c * cellWidth;
                    r.right = r.left + cellWidth;
                    r.top = client.top + (i - 1) * cellHeight;
                    r.bottom = r.top + cellHeight;

                    COLORREF color = GetColor(i); // 행(줄) 기준 색상
                    SetTextColor(hDC, color);

                    wsprintf(lpOut, L"%d x %d = %d", dan, i, dan * i);
                    DrawText(hDC, lpOut, lstrlen(lpOut), &r,
                        DT_SINGLELINE | DT_CENTER | DT_VCENTER);
                }
            }

            // 아래쪽 절반
            for (int c = 0; c < garo; ++c)
            {
                int dan = endDan - c;

                for (int i = 1; i <= 9; ++i)
                {
                    RECT r;
                    r.left = client.left + c * cellWidth;
                    r.right = r.left + cellWidth;
                    r.top = sero + (i - 1) * cellHeight;
                    r.bottom = r.top + cellHeight;

                    COLORREF color = GetColor(i); // 행(줄) 기준 색상
                    SetTextColor(hDC, color);

                    wsprintf(lpOut, L"%d x %d = %d", dan, i, dan * i);
                    DrawText(hDC, lpOut, lstrlen(lpOut), &r,
                        DT_SINGLELINE | DT_CENTER | DT_VCENTER);
                }
            }

            // 가로 등분 수 상단 출력
            RECT info = { 0, 0, client.right, 30 };
            wsprintf(lpOut, L"가로 등분: %d", garo);
            DrawText(hDC, lpOut, lstrlen(lpOut), &info,
                DT_SINGLELINE | DT_CENTER | DT_VCENTER);

            EndPaint(hWnd, &ps);
            break;
        }
    }

    case WM_DESTROY: // 창 닫힐 때
        PostQuitMessage(0); // 프로그램 종료
        return 0;
    }

    return (DefWindowProc(hWnd, iMessage, wParam, IParam)); // 기본 처리
}