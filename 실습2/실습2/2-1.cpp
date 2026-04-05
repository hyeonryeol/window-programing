#include <windows.h> // 윈도우 API 기본 헤더
#include <tchar.h>   // TCHAR, wsprintf 등 문자열 처리
#include <time.h>    // time() - 랜덤 시드용
#include <stdlib.h>  // rand(), srand()

HINSTANCE g_hlnst;                          // 프로그램 인스턴스 저장
LPCTSTR IpszClass = L"Window Class Name";   // 윈도우 클래스 이름
LPCTSTR IpszWindowName = L"windows program 3"; // 창 제목

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessgae, WPARAM wParam, LPARAM IParam); // WndProc 함수 선언

int WINAPI WinMain(HINSTANCE hlnstance, HINSTANCE hPrevlnstance, LPSTR IpszCmdParam, int nCmdShow)
{
    srand(time(NULL)); // 랜덤 시드 초기화 (매번 다른 랜덤값)

    HWND hWND;         // 창 핸들
    MSG Message;       // 메시지 구조체
    WNDCLASSEX WndClass; // 윈도우 설계도
    g_hlnst = hlnstance;

    // ── 윈도우 설계도 작성 ───────────────────────────────────
    WndClass.cbSize = sizeof(WndClass);              // 구조체 크기
    WndClass.style = CS_HREDRAW | CS_VREDRAW;        // 크기 바뀌면 다시 그리기
    WndClass.lpfnWndProc = (WNDPROC)WndProc;         // 이벤트 처리 함수 지정
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hlnstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);   // 기본 아이콘
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
        100, 100,  // 창 시작 위치
        800, 600,  // 창 크기
        NULL, (HMENU)NULL, hlnstance, NULL
    );

    ShowWindow(hWND, nCmdShow);  // 창 보여주기
    UpdateWindow(hWND);          // 창 즉시 그리기

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

        // ── 중앙 기준점 (400, 300) 출력 ──────────────────────
        rect.left = 325;
        rect.top = 260;
        rect.right = 475;
        rect.bottom = 340;
        wsprintf(lpOut, L" %d:(400,300)", 0); // "0:(400,300)" 형식
        DrawText(hDC, lpOut, lstrlen(lpOut), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        // ── 겹치지 않는 사각형 15개 생성 ────────────────────
        RECT rects[15]; // 생성된 사각형 저장 배열

        for (int i = 0; i < 15; ++i)
        {
            RECT newRect;  // 새로 만들 사각형
            BOOL overlap;  // 겹침 여부

            do // 겹치지 않을 때까지 반복
            {
                overlap = false;

                // 랜덤 위치 생성
                int l = rand() % 600; // x좌표 0~599
                int t = rand() % 500; // y좌표 0~499

                // 사각형 크기 설정 (150x80)
                newRect.left = l;
                newRect.top = t;
                newRect.right = l + 150;
                newRect.bottom = t + 80;

                // 이미 생성된 사각형들과 겹치는지 확인
                for (int j = 0; j < i; ++j)
                {
                    RECT temp;
                    if (IntersectRect(&temp, &newRect, &rects[j])) // 겹치면 true
                    {
                        overlap = TRUE; // 겹침 발생
                        break;
                    }
                }
            } while (overlap); // 겹치면 다시 생성

            rects[i] = newRect; // 겹치지 않는 사각형 저장

            // 사각형 안에 번호와 좌표 출력
            TCHAR buf[100];
            wsprintf(buf, L" %d:(%d,%d)", i + 1, rects[i].left, rects[i].top);
            // 예: "1:(120,340)"
            DrawText(hDC, buf, lstrlen(buf), &rects[i], DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        }

        EndPaint(hWnd, &ps); // 그리기 끝
        break;
    }

    case WM_DESTROY: // 창 닫힐 때
        PostQuitMessage(0); // 프로그램 종료
        return 0;
    }

    return (DefWindowProc(hWnd, iMessage, wParam, IParam)); // 기본 처리
}