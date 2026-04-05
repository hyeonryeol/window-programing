#include <windows.h>   // Win32 API 기본 헤더
#include <tchar.h>     // TCHAR, LPCTSTR 등 문자 타입 매크로
#include <time.h>      // time()
#include <stdlib.h>    // rand(), srand()

HINSTANCE g_hInst;                                 // 프로그램 인스턴스 핸들(전역)
LPCTSTR lpszClass = L"Window Class Name";          // 윈도우 클래스 이름
LPCTSTR lpszWindowName = L"windows program 3";     // 윈도우 제목

// ── 전역 변수 ─────────────────────────────────────────────────
int startX, startY;         // 텍스트 출력 시작 위치(랜덤)
COLORREF textColor;         // 텍스트 색상(랜덤 RGB)
TCHAR lines[5][21];         // 최대 5줄, 각 줄 최대 20자 + 널문자('\0')
int lineLens[5];            // 각 줄의 현재 문자열 길이
int curLine = 0;            // 현재 입력 중인 줄 인덱스(0~4)
int totalLines = 1;         // 화면에 출력할 총 줄 수(최소 1)

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
    srand(time(NULL)); // 난수 시드 초기화(매 실행마다 다른 랜덤값)

    // 랜덤 시작 위치/색상 초기화
    startX = rand() % 600;       // X: 0~599
    startY = rand() % 500;       // Y: 0~499
    int r = rand() % 256;        // R: 0~255
    int g = rand() % 256;        // G: 0~255
    int b = rand() % 256;        // B: 0~255
    textColor = RGB(r, g, b);    // COLORREF 생성

    // 줄 버퍼 초기화
    for (int i = 0; i < 5; i++) {
        lineLens[i] = 0;         // 길이 0
        lines[i][0] = L'\0';     // 빈 문자열
    }

    HWND hWnd;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hInst = hInstance;         // 전역 인스턴스 저장

    // 윈도우 클래스 정보 설정
    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_HREDRAW | CS_VREDRAW;      // 크기 변경 시 다시 그리기
    WndClass.lpfnWndProc = WndProc;                // 메시지 처리 함수
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 흰 배경
    WndClass.lpszMenuName = NULL;                  // 메뉴 없음
    WndClass.lpszClassName = lpszClass;            // 클래스 이름
    WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&WndClass);                    // 클래스 등록

    // 윈도우 생성
    hWnd = CreateWindow(
        lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600, NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow); // 창 표시
    UpdateWindow(hWnd);         // WM_PAINT 유도

    // 메시지 루프
    while (GetMessage(&Message, 0, 0, 0)) {
        TranslateMessage(&Message); // 키 입력 변환
        DispatchMessage(&Message);  // WndProc로 전달
    }
    return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hDC;

    switch (iMessage)
    {
    case WM_CHAR: // 문자 입력 메시지 처리
    {
        TCHAR ch = (TCHAR)wParam; // 입력된 문자

        if (ch == 27) // ESC 키(ASCII 27) → 프로그램 종료
        {
            PostQuitMessage(0);
        }
        else if (ch == '\b') // 백스페이스
        {
            // 현재 줄에 문자가 있을 때만 마지막 문자 삭제
            if (lineLens[curLine] > 0)
            {
                lineLens[curLine]--;
                lines[curLine][lineLens[curLine]] = L'\0'; // 문자열 끝 갱신
                InvalidateRect(hWnd, NULL, TRUE);           // 화면 다시 그리기 요청
            }
            // 현재 줄이 비어 있으면 삭제하지 않음(아무 동작 없음)
        }
        else if (ch == '\r') // 엔터
        {
            if (curLine < 4) // 마지막 줄(4번)이 아니면 다음 줄로 이동
            {
                int nextLine = curLine + 1;

                // 현재 줄 내용을 다음 줄로 복사(널문자 포함)
                lineLens[nextLine] = lineLens[curLine];
                for (int i = 0; i <= lineLens[curLine]; i++)
                    lines[nextLine][i] = lines[curLine][i];

                // 현재 줄 비우기
                lineLens[curLine] = 0;
                lines[curLine][0] = L'\0';

                // 커서를 다음 줄로 이동
                curLine = nextLine;

                // 총 줄 수 갱신
                if (curLine >= totalLines) totalLines = curLine + 1;
            }
            // 마지막 줄이면 줄 이동 없이 유지
            InvalidateRect(hWnd, NULL, TRUE); // 다시 그리기
        }
        else if (isalpha(ch)) // 알파벳(A-Z, a-z)만 입력 허용
        {
            if (lineLens[curLine] < 20) // 현재 줄이 20자 미만이면 현재 줄에 추가
            {
                lines[curLine][lineLens[curLine]++] = ch;   // 문자 추가 후 길이 증가
                lines[curLine][lineLens[curLine]] = L'\0';  // 널 종료
            }
            else // 현재 줄이 20자면 다음 줄로 자동 줄바꿈
            {
                if (curLine < 4) // 다음 줄이 존재하면
                {
                    curLine++;
                    if (curLine >= totalLines) totalLines = curLine + 1;
                    lines[curLine][0] = ch;    // 새 줄 첫 글자로 입력 문자 저장
                    lineLens[curLine] = 1;
                    lines[curLine][1] = L'\0';
                }
                // 이미 5줄 가득 차 있으면 입력 무시
            }
            InvalidateRect(hWnd, NULL, TRUE); // 변경 내용 반영 위해 다시 그리기
        }
        return 0; // WM_CHAR 직접 처리 완료
    }

    case WM_PAINT:
    {
        hDC = BeginPaint(hWnd, &ps);          // 그리기 시작
        SetTextColor(hDC, textColor);         // 랜덤 텍스트 색상 적용
        SetBkMode(hDC, TRANSPARENT);          // 텍스트 배경 투명

        // 현재 totalLines만큼 줄을 세로로 출력
        for (int i = 0; i < totalLines; i++)
        {
            int drawY = startY + i * 20;      // 줄 간격 20픽셀
            TextOut(hDC, startX, drawY, lines[i], lineLens[i]);
        }

        // 커서 그리기: 현재 줄 문자열 끝 위치에 세로선 출력
        SIZE sz;
        GetTextExtentPoint32(hDC, lines[curLine], lineLens[curLine], &sz); // 현재 줄 너비 계산
        int cursorX = startX + sz.cx;         // 문자열 끝 X
        int cursorY = startY + curLine * 20;  // 현재 줄 Y

        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); // 검은색 2px 펜
        HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
        MoveToEx(hDC, cursorX, cursorY, NULL);             // 커서 시작점
        LineTo(hDC, cursorX, cursorY + 16);                // 세로선 길이 16
        SelectObject(hDC, hOldPen);                        // 원래 펜 복원
        DeleteObject(hPen);                                // GDI 리소스 해제

        EndPaint(hWnd, &ps); // 그리기 종료
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0); // 종료 메시지 보냄(메시지 루프 종료)
        return 0;
    }

    // 위에서 처리하지 않은 메시지는 기본 처리
    return DefWindowProc(hWnd, iMessage, wParam, lParam);
}