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
// index % 5 에 따라 5가지 색상 중 하나 반환
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
    bool paint = false; // 그리기 여부 (항상 false → 항상 그림)
    RECT rect;
    int x = 0, y = 0;
    TCHAR lpOut[100]; // 출력할 문자열 버퍼

    // ── 단 결정 ───────────────────────────────────────────────
    // ⚠️ WndProc이 호출될 때마다 새로 선언되므로
    //    static 없이는 항상 새 랜덤값이 생성됨
    bool _danChosen = false;
    int  _chosenDan = 2;

    // _danChosen이 항상 false라서 매번 새 단이 결정됨
    if (!_danChosen) { _chosenDan = rand() % 8 + 2; _danChosen = true; }
    // _chosenDan: 2~9 중 랜덤으로 선택된 단

    int garo = _chosenDan; // 가로 열 수 = 선택된 단

    if (paint == false) // 항상 true (paint가 항상 false이므로)
    {
        int n = rand() % 10;          // 사용 안 됨
        int z = rand() % _chosenDan;  // 출력할 열 위치 (0 ~ 단-1 중 랜덤)

        switch (iMessage)
        {
        case WM_PAINT: // 화면을 그릴 때 호출
        {
            hDC = BeginPaint(hWnd, &ps); // 그리기 시작

            RECT client;
            GetClientRect(hWnd, &client); // 창 크기 가져오기

            // ── 상단에 선택 단 출력 ───────────────────────────
            RECT header = { client.left, client.top, client.right, client.top };
            wsprintf(lpOut, L"선택 단: %d", _chosenDan); // 예: "선택 단: 5"
            SetTextColor(hDC, RGB(0, 0, 0)); // 검정색
            DrawText(hDC, lpOut, lstrlen(lpOut), &header,
                DT_SINGLELINE | DT_CENTER | DT_VCENTER);

            // ── 셀 크기 계산 ──────────────────────────────────
            int areaTop = client.top;
            int areaHeight = client.bottom;
            int cellHeight = areaHeight / 9;  // 9행으로 나누기
            int cellWidth = 700 / garo;      // 열 수로 나누기
            int width = 200;             // 텍스트 박스 너비

            // ── 구구단 출력 (1~9) ─────────────────────────────
            for (int i = 1; i <= 9; ++i)
            {
                RECT r;
                // z = 랜덤 열 위치, i*10 = 줄마다 조금씩 오른쪽 이동 (계단식)
                r.left = (cellWidth * z) + client.left + (i * 10);
                r.right = r.left + width;
                r.top = areaTop + (i - 1) * cellHeight; // i번째 행 y위치
                r.bottom = r.top + cellHeight;

                COLORREF color = GetColor(rand() % 5); // 랜덤 색상
                SetTextColor(hDC, color);

                wsprintf(lpOut, L"%d x %d = %d", _chosenDan, i, _chosenDan * i);
                // 예: "5 x 3 = 15"
                DrawText(hDC, lpOut, lstrlen(lpOut), &r,
                    DT_SINGLELINE | DT_CENTER | DT_VCENTER);
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
}