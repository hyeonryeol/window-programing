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

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
    PAINTSTRUCT ps;
    HDC hDC;
    RECT rect;
    int x = 0, y = 0;
    TCHAR lpOut[100]; // 출력할 문자열 버퍼

    switch (iMessage)
    {
    case WM_PAINT: // 화면을 그릴 때 호출
    {
        hDC = BeginPaint(hWnd, &ps); // 그리기 시작

        RECT client;
        GetClientRect(hWnd, &client); // 창 크기 가져오기
        // client.right  = 창 너비
        // client.bottom = 창 높이

        TCHAR lpOut[100];

        // ── 1. 가로 등분 수 랜덤 결정 (2~10) ────────────────
        int garo = rand() % 9 + 2;

        // ── 2. 세로 절반 위치 ────────────────────────────────
        int sero = 600 / 2; // 위쪽 절반 / 아래쪽 절반 경계선 y좌표

        // ── 3. 셀 크기 계산 ──────────────────────────────────
        int cellWidth = 800 / garo;              // 셀 하나의 너비
        int cellHeight = (sero - client.top) / 9; // 셀 하나의 높이 (9행)

        int startDan = 2;               // 시작 단 (2단부터)
        int endDan = startDan + garo - 1; // 마지막 단

        // ── 위쪽 절반: 왼쪽부터 오름차순으로 구구단 출력 ────
        // 예: garo=3이면 2단, 3단, 4단 순서로
        for (int c = 0; c < garo; ++c)
        {
            int dan = startDan + c; // c번째 열의 단 번호

            for (int i = 1; i <= 9; ++i) // 1~9 행 반복
            {
                RECT r;
                r.left = client.left + c * cellWidth; // 열 시작 x
                r.right = r.left + cellWidth;
                r.top = client.top + (i - 1) * cellHeight; // 행 시작 y
                r.bottom = r.top + cellHeight;

                wsprintf(lpOut, L"%d x %d = %d", dan, i, dan * i);
                // 예: "2 x 3 = 6"
                DrawText(hDC, lpOut, lstrlen(lpOut), &r,
                    DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            }
        }

        // ── 아래쪽 절반: 왼쪽부터 내림차순으로 구구단 출력 ──
        // 예: garo=3이면 4단, 3단, 2단 순서로 (위쪽 반대)
        for (int c = 0; c < garo; ++c)
        {
            int dan = endDan - c; // c번째 열의 단 번호 (역순)

            for (int i = 1; i <= 9; ++i)
            {
                RECT r;
                r.left = client.left + c * cellWidth;
                r.right = r.left + cellWidth;
                r.top = sero + (i - 1) * cellHeight; // sero 아래쪽에 출력
                r.bottom = r.top + cellHeight;

                wsprintf(lpOut, L"%d x %d = %d", dan, i, dan * i);
                DrawText(hDC, lpOut, lstrlen(lpOut), &r,
                    DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            }
        }

        // ── 가로 등분 개수 상단에 출력 ───────────────────────
        RECT info = { 0, 0, client.right, 30 };
        wsprintf(lpOut, L"가로 등분: %d", garo);
        DrawText(hDC, lpOut, lstrlen(lpOut), &info,
            DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        EndPaint(hWnd, &ps); // 그리기 끝
    }
    break;

    case WM_DESTROY: // 창 닫힐 때
        PostQuitMessage(0); // 프로그램 종료
        return 0;
    }

    return (DefWindowProc(hWnd, iMessage, wParam, IParam)); // 기본 처리
}