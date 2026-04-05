#include <windows.h>   // Win32 API 기본 기능(창, 메시지, GDI)
#include <tchar.h>     // TCHAR, LPCTSTR 등 문자 타입
#include <time.h>      // time()
#include <stdlib.h>    // rand(), srand()

HINSTANCE g_hInst;                                 // 인스턴스 핸들(전역)
LPCTSTR lpszClass = L"Window Class Name";          // 윈도우 클래스 이름
LPCTSTR lpszWindowName = L"windows program";       // 윈도우 제목

// 한 번의 출력 정보를 담는 구조체
struct DrawInfo
{
    int x, y;           // 출력 시작 위치
    int n;              // 반복해서 출력할 숫자
    int count;          // 가로/세로 반복 횟수(정사각 형태)
    COLORREF textColor; // 글자색
    COLORREF bkColor;   // 배경색
};

// 최대 10개의 출력 기록 저장
DrawInfo history[10];
int historyCount = 0;   // 현재 저장된 기록 개수
bool showAll = false;   // true면 누적 출력(a), false면 최신 1개만 출력

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

// 랜덤 DrawInfo 1개 생성 함수
DrawInfo makeRandom()
{
    DrawInfo info;
    info.x = rand() % 600;   // x 위치 랜덤
    info.y = rand() % 400;   // y 위치 랜덤
    info.n = rand() % 200;   // 숫자 0~199
    info.count = rand() % 16 + 5; // 반복 횟수 5~20
    info.textColor = RGB(rand() % 256, rand() % 256, rand() % 256); // 랜덤 글자색
    info.bkColor = RGB(rand() % 256, rand() % 256, rand() % 256);   // 랜덤 배경색
    return info;
}

// DrawInfo 1개를 화면에 실제로 그리는 함수
void drawOne(HDC hDC, DrawInfo& info)
{
    // 숫자를 문자열로 변환 (예: 123)
    TCHAR numStr[20];
    wsprintf(numStr, L"%d", info.n);
    int numLen = lstrlen(numStr);

    // 숫자 문자열의 픽셀 크기 측정
    SIZE sz;
    GetTextExtentPoint32(hDC, numStr, numLen, &sz);
    int charW = sz.cx; // 숫자 문자열 1개 너비
    int charH = sz.cy; // 숫자 문자열 1개 높이

    // 한 줄 문자열 생성: 숫자 문자열을 count번 이어붙임
    // 예) n=12, count=3 -> "121212"
    TCHAR rowStr[500];
    rowStr[0] = L'\0';
    for (int i = 0; i < info.count; i++)
        lstrcat(rowStr, numStr);
    int rowLen = lstrlen(rowStr);

    // 배경 사각형 크기 계산 (가로 count개, 세로 count줄)
    int rectW = charW * info.count;
    int rectH = charH * info.count;

    // 배경 사각형 채우기
    HBRUSH hBrush = CreateSolidBrush(info.bkColor);
    RECT r = { info.x, info.y, info.x + rectW, info.y + rectH };
    FillRect(hDC, &r, hBrush);
    DeleteObject(hBrush); // 브러시 리소스 해제

    // 텍스트 속성 설정 후 count줄 출력
    SetTextColor(hDC, info.textColor);
    SetBkMode(hDC, TRANSPARENT); // 글자 배경 투명
    for (int i = 0; i < info.count; i++)
        TextOut(hDC, info.x, info.y + i * charH, rowStr, rowLen);

    // 배경 아래쪽에 정보 문자열 출력: x y n count
    TCHAR info_str[100];
    wsprintf(info_str, L"%d %d %d %d", info.x, info.y, info.n, info.count);
    SetTextColor(hDC, RGB(0, 0, 0)); // 정보 문자열은 검은색
    TextOut(hDC, info.x, info.y + rectH + 2, info_str, lstrlen(info_str));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    srand(time(NULL)); // 난수 시드 초기화

    // 시작 시 첫 랜덤 출력 1개를 history에 저장
    history[historyCount++] = makeRandom();

    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInst = hInstance;

    // 윈도우 클래스 설정
    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_HREDRAW | CS_VREDRAW; // 창 크기 변경 시 다시 그림
    WndClass.lpfnWndProc = WndProc;           // 메시지 처리 함수
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 흰 배경
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = lpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&WndClass);

    // 메인 윈도우 생성
    hWnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 메시지 루프
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
    case WM_CHAR: // 문자 키 입력 처리
    {
        TCHAR ch = (TCHAR)wParam;

        if (ch == 'q') // q: 프로그램 종료
        {
            PostQuitMessage(0);
        }
        else if (ch == 'r') // r: 리셋(기록 지우고 새 랜덤 1개만)
        {
            showAll = false;               // 최신 1개 모드
            historyCount = 0;              // 기록 ���기화
            history[historyCount++] = makeRandom(); // 새 기록 1개 생성
            InvalidateRect(hWnd, NULL, TRUE);       // 다시 그리기 요청
        }
        else if (ch == 'a') // a: 지금까지의 기록 모두 출력
        {
            showAll = true;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        return 0; // WM_CHAR 직접 처리 완료
    }

    case WM_KEYDOWN: // 기능키/특수키 처리
    {
        if (wParam == VK_RETURN) // Enter: 새 랜덤 출력 추가
        {
            if (historyCount < 10)                  // 최대 10개까지만 저장
                history[historyCount++] = makeRandom();
            showAll = false;                        // Enter는 최신 1개만 보이게
            InvalidateRect(hWnd, NULL, TRUE);       // 다시 그리기 요청
        }
        break;
    }

    case WM_PAINT: // 화면 갱신
    {
        hDC = BeginPaint(hWnd, &ps);

        if (showAll) // a키 상태: 누적된 모든 기록 출력
        {
            for (int i = 0; i < historyCount; i++)
                drawOne(hDC, history[i]);
        }
        else // 기본/Enter 상태: 최신 기록 1개만 출력
        {
            drawOne(hDC, history[historyCount - 1]);
        }

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0); // 종료 메시지 전송
        return 0;
    }

    // 미처리 메시지는 기본 처리
    return DefWindowProc(hWnd, iMessage, wParam, lParam);
}