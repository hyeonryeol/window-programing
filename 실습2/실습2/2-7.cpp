#include <windows.h>   // Win32 API
#include <tchar.h>     // TCHAR 계열
#include <time.h>
#include <stdlib.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"메모장";

// 캐럿(커서) 위치
int cursorX = 10;
int cursorY = 10;

// 텍스트 버퍼: 최대 10줄, 각 줄 최대 30자(+널문자)
TCHAR lines[10][31];
int lineLens[10];      // 각 줄의 현재 길이
int curLine = 0;       // 현재 줄 인덱스
int totalLines = 1;    // 실제 사용 중인 줄 수
int curidx = 0;        // 현재 줄 내 커서 문자 인덱스
bool overwriteMode = false; // true면 덮어쓰기 모드(끝까지 차면 순환처럼 동작)

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	// 모든 줄 버퍼 초기화
	for (int i = 0; i < 10; i++) {
		lineLens[i] = 0;
		lines[i][0] = L'\0';
	}

	// 윈도우 클래스 등록 정보 설정
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
	case WM_CREATE:
		// 캐럿 생성/표시/초기 위치 설정
		CreateCaret(hWnd, NULL, 2, 20); // 폭 2, 높이 20
		ShowCaret(hWnd);
		SetCaretPos(10, 10);
		break;

	case WM_CHAR:
	{
		TCHAR ch = (TCHAR)wParam;

		// 알파벳 입력 처리
		if (isalpha(ch))
		{
			// 현재 줄이 30자 미만이면 현재 줄에 입력
			if (curidx < 30)
			{
				// 현재 커서 위치에 문자 기록(덮어쓰기)
				lines[curLine][curidx] = ch;

				if (!overwriteMode)
				{
					// 일반 모드에서는 문자열 끝 유지
					lines[curLine][curidx + 1] = L'\0';

					// 커서가 줄 끝이었다면 길이 증가
					if (curidx == lineLens[curLine])
					{
						lineLens[curLine]++;
						lines[curLine][curidx + 1] = L'\0';
					}
				}

				curidx++; // 커서를 다음 문자 위치로 이동

				// 현재 줄 텍스트 폭을 측정해 캐럿 X 계산
				HDC hDC = GetDC(hWnd);
				SIZE sz;
				GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);
				ReleaseDC(hWnd, hDC);

				cursorX = 10 + sz.cx;
				SetCaretPos(cursorX, cursorY);

				InvalidateRect(hWnd, NULL, TRUE); // 다시 그리기
			}
			else // 현재 줄 30자 초과 시 다음 줄로 이동 시도
			{
				if (curLine < 9) // 다음 줄이 존재
				{
					curLine++;

					if (overwriteMode)
					{
						// 덮어쓰기 모드: 새 줄 현재 인덱스에 기록
						lines[curLine][curidx] = ch;
						curidx = 1;
					}
					else
					{
						// 일반 모드: 새 줄 맨 앞부터 시작
						if (curLine >= totalLines) totalLines = curLine + 1;
						lines[curLine][0] = ch;
						lineLens[curLine] = 1;
						lines[curLine][1] = L'\0';
						curidx = 1;
					}

					// 캐럿 위치(새 줄 첫 글자 뒤) 계산
					HDC hDC = GetDC(hWnd);
					TEXTMETRIC tm;
					GetTextMetrics(hDC, &tm);
					SIZE sz;
					GetTextExtentPoint32(hDC, &ch, 1, &sz);
					ReleaseDC(hWnd, hDC);

					cursorX = 10 + sz.cx;
					cursorY = 10 + curLine * tm.tmHeight;
					SetCaretPos(cursorX, cursorY);
				}
				else // 10줄이 모두 찬 경우: 첫 줄 처음으로 돌아가 덮어쓰기 시작
				{
					curLine = 0;
					curidx = 0;
					overwriteMode = true;
					lines[curLine][curidx] = ch;
					curidx++;

					// 캐럿 위치 갱신
					HDC hDC = GetDC(hWnd);
					SIZE sz;
					GetTextExtentPoint32(hDC, &ch, 1, &sz);
					ReleaseDC(hWnd, hDC);

					cursorX = 10 + sz.cx;
					cursorY = 10;
					SetCaretPos(cursorX, cursorY);
				}
			}

			InvalidateRect(hWnd, NULL, TRUE);
		}
		// 백스페이스 처리
		else if (ch == '\b')
		{
			if (overwriteMode)
			{
				// 줄 맨 앞이면 삭제 불가
				if (curidx == 0)
					return 0;

				// 현재 줄에서 (curidx-1) 위치 문자 삭제(뒤 문자 당김)
				for (int i = curidx - 1; i < lineLens[curLine] - 1; i++)
				{
					lines[curLine][i] = lines[curLine][i + 1];
				}

				lineLens[curLine]--;
				curidx--;
				lines[curLine][lineLens[curLine]] = L'\0';

				// 다음 줄의 첫 글자를 현재 줄 끝으로 당겨오기(줄 이어붙이기)
				if (curLine < totalLines - 1 && lineLens[curLine] < 30)
				{
					TCHAR nextChar = lines[curLine + 1][0];

					lines[curLine][lineLens[curLine]] = nextChar;
					lineLens[curLine]++;
					lines[curLine][lineLens[curLine]] = L'\0';

					// 다음 줄 앞당기기
					for (int i = 0; i < lineLens[curLine + 1] - 1; i++)
					{
						lines[curLine + 1][i] = lines[curLine + 1][i + 1];
					}

					lineLens[curLine + 1]--;
					lines[curLine + 1][lineLens[curLine + 1]] = L'\0';

					// 다음 줄이 비면 뒤 줄들을 위로 당겨 빈 줄 제거
					if (lineLens[curLine + 1] == 0)
					{
						for (int i = curLine + 1; i < totalLines - 1; i++)
						{
							lstrcpy(lines[i], lines[i + 1]);
							lineLens[i] = lineLens[i + 1];
						}
						totalLines--;
					}
				}

				// 캐럿 위치 재계산
				HDC hDC = GetDC(hWnd);
				SIZE sz;
				GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);
				ReleaseDC(hWnd, hDC);

				cursorX = 10 + sz.cx;
				SetCaretPos(cursorX, cursorY);

				InvalidateRect(hWnd, NULL, TRUE);
			}
			else
			{
				// 일반 모드: 현재 줄에 글자가 있으면 마지막 글자 삭제
				if (lineLens[curLine] > 0)
				{
					TCHAR deleted = lines[curLine][lineLens[curLine] - 1];

					lineLens[curLine]--;
					curidx--;
					lines[curLine][lineLens[curLine]] = L'\0';
					curidx = lineLens[curLine];

					// 삭제한 글자 너비만큼 캐럿 왼쪽 이동
					HDC hDC = GetDC(hWnd);
					SIZE sz;
					GetTextExtentPoint32(hDC, &deleted, 1, &sz);
					ReleaseDC(hWnd, hDC);

					cursorX -= sz.cx;
					SetCaretPos(cursorX, cursorY);

					InvalidateRect(hWnd, NULL, TRUE);
				}
				// 현재 줄이 비어 있고 이전 줄이 있으면 이전 줄로 이동 후 삭제 처리
				else if (curLine > 0)
				{
					curidx--;
					curLine--;

					// 이전 줄에서 curidx 위치 이후를 한 칸 당김
					for (int i = curidx; i < lineLens[curLine] - 1; i++)
					{
						lines[curLine][i] = lines[curLine][i + 1];
					}

					lineLens[curLine]--;
					lines[curLine][lineLens[curLine]] = L'\0';

					// 캐럿을 이전 줄 끝으로 이동
					HDC hDC = GetDC(hWnd);
					TEXTMETRIC tm;
					GetTextMetrics(hDC, &tm);
					SIZE sz;
					GetTextExtentPoint32(hDC, lines[curLine], lineLens[curLine], &sz);
					ReleaseDC(hWnd, hDC);

					cursorX = 10 + sz.cx;
					cursorY = 10 + curLine * tm.tmHeight;
					SetCaretPos(cursorX, cursorY);

					InvalidateRect(hWnd, NULL, TRUE);
				}
			}
		}
		// ESC: 전체 초기화
		else if (ch == 27)
		{
			for (int i = 0; i < 10; i++)
			{
				lineLens[i] = 0;
				lines[i][0] = L'\0';
			}
			curLine = 0;
			totalLines = 1;
			curidx = 0;
			cursorX = 10;
			cursorY = 10;
			SetCaretPos(cursorX, cursorY);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		return 0;
	}

	case WM_KEYDOWN:
	{
		// Enter: 다음 줄로 이동
		if (wParam == VK_RETURN)
		{
			if (curLine < 9)
			{
				curLine++;
				if (curLine >= totalLines) totalLines = curLine + 1;
				curidx = 0;

				// 새 줄 맨 앞으로 캐럿 이동
				HDC hDC = GetDC(hWnd);
				TEXTMETRIC tm;
				GetTextMetrics(hDC, &tm);
				ReleaseDC(hWnd, hDC);

				cursorX = 10;
				cursorY = 10 + curLine * tm.tmHeight;
				SetCaretPos(cursorX, cursorY);
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
		break;
	}

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		SetBkMode(hDC, TRANSPARENT);

		// 폰트 높이 구해서 줄 간격으로 사용
		TEXTMETRIC tm;
		GetTextMetrics(hDC, &tm);
		int charH = tm.tmHeight;

		// 현재까지의 모든 줄 출력
		for (int i = 0; i < totalLines; i++)
		{
			TextOut(hDC, 10, 10 + i * charH, lines[i], lineLens[i]);
		}

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	// 기본 처리
	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}