#include <windows.h>   // Win32 API (창, 메시지, GDI)
#include <tchar.h>     // TCHAR, LPCTSTR
#include <time.h>
#include <stdlib.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"메모장";

// 캐럿(커서) 실제 화면 좌표
int cursorX = 10;
int cursorY = 10;

// 텍스트 버퍼: 최대 10줄, 줄당 최대 30자(+널 문자)
TCHAR lines[10][31];
int lineLens[10];      // 각 줄 길이
int curLine = 0;       // 현재 줄 인덱스
int totalLines = 1;    // 현재 사용 중인 총 줄 수
int curidx = 0;        // 현재 줄 내부 문자 인덱스(커서 열)
bool overwriteMode = false; // 덮어쓰기 관련 플래그
bool insertmode = false;    // true: 삽입 모드, false: 기본(덮어쓰기 성격)

// 현재 curLine/curidx를 기준으로 캐럿 위치 재계산
void UpdateCaret(HWND hWnd)
{
	HDC hDC = GetDC(hWnd);
	TEXTMETRIC tm{};
	GetTextMetrics(hDC, &tm);

	// 인덱스 경계 보정
	if (curidx < 0) curidx = 0;
	if (curidx > lineLens[curLine]) curidx = lineLens[curLine];

	SIZE sz{ 0,0 };
	// 커서 앞 문자 폭 계산
	if (curidx > 0)
		GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);

	ReleaseDC(hWnd, hDC);

	// 캐럿 좌표 반영
	cursorX = 10 + sz.cx;
	cursorY = 10 + curLine * tm.tmHeight;
	SetCaretPos(cursorX, cursorY);
}

// Delete 키 동작: "커서 위치 기준 단어 삭제"
void DeleteWordAtCaret(HWND hWnd)
{
	int len = lineLens[curLine];
	if (len == 0) return; // 빈 줄이면 종료

	// 인덱스 경계 보정
	if (curidx < 0) curidx = 0;
	if (curidx > len) curidx = len;

	int pos = curidx;

	// 1) 커서가 공백이면 공백 구간 건너뛰고 다음 단어부터 삭제
	while (pos < len && lines[curLine][pos] == L' ')
		pos++;

	// 삭제할 단어가 없으면 캐럿만 갱신 후 종료
	if (pos >= len) {
		if (curidx > len) curidx = len;
		UpdateCaret(hWnd);
		InvalidateRect(hWnd, NULL, TRUE);
		return;
	}

	// 2) 단어 시작/끝 찾기 ([start, end))
	int start = pos;
	int end = pos;
	while (end < len && lines[curLine][end] != L' ')
		end++;

	// 3) 해당 구간 삭제: 뒤 문자열을 앞으로 당김(널문자 포함 복사)
	memmove(&lines[curLine][start],
		&lines[curLine][end],
		(len - end + 1) * sizeof(TCHAR));

	// 길이 갱신
	lineLens[curLine] -= (end - start);

	// 4) 캐럿 보정
	if (curidx > lineLens[curLine])
		curidx = lineLens[curLine];

	UpdateCaret(hWnd);
	InvalidateRect(hWnd, NULL, TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	// 버퍼 초기화
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
		// 캐럿 생성 및 초기 표시
		CreateCaret(hWnd, NULL, 2, 20);
		ShowCaret(hWnd);
		SetCaretPos(10, 10);
		break;

	case WM_CHAR:
	{
		TCHAR ch = (TCHAR)wParam;

		// ===== 기본 모드(insertmode == false): 덮어쓰기 성격 =====
		if (insertmode == false)
		{
			// 출력 가능한 문자(스페이스)
			if (32 <= ch)
			{
				if (curidx < 30)
				{
					// 현재 위치에 문자 배치
					lines[curLine][curidx] = ch;

					// overwriteMode가 아니면 문자열 끝/길이 관리
					if (!overwriteMode)
					{
						lines[curLine][curidx + 1] = L'\0';
						if (curidx == lineLens[curLine])
						{
							lineLens[curLine]++;
							lines[curLine][curidx + 1] = L'\0';
						}
					}
					curidx++; // 커서 오른쪽 이동

					// 캐럿 X 재계산
					HDC hDC = GetDC(hWnd);
					SIZE sz;
					GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);
					ReleaseDC(hWnd, hDC);

					cursorX = 10 + sz.cx;
					SetCaretPos(cursorX, cursorY);

					InvalidateRect(hWnd, NULL, TRUE);
				}
				else // 30자를 넘으면 다음 줄로 이동 시도
				{
					if (curLine < 9)
					{
						curLine++;
						if (overwriteMode)
						{
							lines[curLine][curidx] = ch;
							curidx = 1;
						}
						else
						{
							if (curLine >= totalLines) totalLines = curLine + 1;
							lines[curLine][0] = ch;
							lineLens[curLine] = 1;
							lines[curLine][1] = L'\0';
							curidx = 1;
						}

						// 새 줄 캐럿 위치 계산
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
					else // 10줄 꽉 차면 첫 줄부터 덮어쓰기
					{
						curLine = 0;
						curidx = 0;
						overwriteMode = true;
						lines[curLine][curidx] = ch;
						curidx++;

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
		}

		// ===== 삽입 모드(insertmode == true): 뒤 문자 밀고 삽입 =====
		if (insertmode == true)
		{
			if (32 <= ch)
			{
				if (curidx < 30)
				{
					int len = lineLens[curLine];

					// 인덱스 보정
					if (curidx < 0) curidx = 0;
					if (curidx > len) curidx = len;

					// 뒤에서부터 한 칸씩 밀기(널문자까지)
					for (int i = len; i >= curidx; --i) {
						lines[curLine][i + 1] = lines[curLine][i];
					}

					// 현재 위치에 삽입
					lines[curLine][curidx] = ch;

					// 길이 증가 (필요시 curidx++ 추가 가능)
					lineLens[curLine]++;
				}
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}

		// 주의: 현재 코드 구조상 이 else-if는 바로 위 if(insertmode==true)에 결합됨
		// 즉 insertmode==true이고 32<=ch가 아닐 때만 백스페이스로 들어감
		else if (ch == '\b')
		{
			if (overwriteMode)
			{
				// 줄 맨 앞이면 삭제 불가
				if (curidx == 0)
					return 0;

				// 현재 줄에서 한 글자 삭제(좌측으로 당김)
				for (int i = curidx - 1; i < lineLens[curLine] - 1; i++)
				{
					lines[curLine][i] = lines[curLine][i + 1];
				}

				lineLens[curLine]--;
				curidx--;
				lines[curLine][lineLens[curLine]] = L'\0';

				// 다음 줄 첫 글자 당겨와 현재 줄 끝에 붙이기
				if (curLine < totalLines - 1 && lineLens[curLine] < 30)
				{
					TCHAR nextChar = lines[curLine + 1][0];

					lines[curLine][lineLens[curLine]] = nextChar;
					lineLens[curLine]++;
					lines[curLine][lineLens[curLine]] = L'\0';

					// 다음 줄 당김
					for (int i = 0; i < lineLens[curLine + 1] - 1; i++)
					{
						lines[curLine + 1][i] = lines[curLine + 1][i + 1];
					}

					lineLens[curLine + 1]--;
					lines[curLine + 1][lineLens[curLine + 1]] = L'\0';

					// 빈 줄이면 뒤 줄을 위로 당겨 정리
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
				// 일반 삭제: 현재 줄 끝 글자 삭제
				if (lineLens[curLine] > 0)
				{
					TCHAR deleted = lines[curLine][lineLens[curLine] - 1];

					lineLens[curLine]--;
					curidx--;
					lines[curLine][lineLens[curLine]] = L'\0';
					curidx = lineLens[curLine];

					// 삭제 문자 폭만큼 캐럿 이동
					HDC hDC = GetDC(hWnd);
					SIZE sz;
					GetTextExtentPoint32(hDC, &deleted, 1, &sz);
					ReleaseDC(hWnd, hDC);

					cursorX -= sz.cx;
					SetCaretPos(cursorX, cursorY);

					InvalidateRect(hWnd, NULL, TRUE);
				}
				else if (curLine > 0) // 현재 줄 비어 있고 이전 줄 존재
				{
					curidx--;
					curLine--;

					// 이전 줄에서 문자 당기기
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
		else if (ch == 27) // ESC: 전체 초기화
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

		// 방향키/기능키 처리
		if (wParam == VK_LEFT)
		{
			if (lines[curLine][curidx - 1] != L'\0')
			{
				if (curidx > 0) curidx--;
			}
			if (curidx == 0 && curLine > 0)
			{
				curLine--;
				curidx = lineLens[curLine];
			}
			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_RIGHT)
		{
			if (lines[curLine][curidx] != L'\0')
			{
				if (curidx < 30) curidx++;
			}
			if (curidx == 30 && curLine < 10)
			{
				curLine++;
				curidx = 0;
			}
			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_UP)
		{
			if (lines[curLine - 1][curidx] != L'\0')
			{
				if (curLine > 0) curLine--;
			}
			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_DOWN)
		{
			if (lines[curLine + 1][curidx] != L'\0')
			{
				if (curLine < 10) curLine++;
			}
			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_TAB)
		{
			// 현재 줄에 공백 3칸 삽입
			if (curidx < 28)
			{
				for (int i = 0; i < 3; ++i)
				{
					lines[curLine][curidx] = L' ';
					if (curidx >= lineLens[curLine])
						lineLens[curLine]++;
					curidx++;
				}
				lines[curLine][lineLens[curLine]] = L'\0';
			}
			else if (curLine < 9)
			{
				// 공간 부족하면 다음 줄로
				curLine++;
				if (curLine >= totalLines) totalLines = curLine + 1;
				curidx = 0;
			}
			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_HOME)
		{
			// 줄 맨 앞으로 이동
			curidx = 0;
			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_END)
		{
			// 줄 맨 끝으로 이동
			curidx = lineLens[curLine];
			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_INSERT)
		{
			// Insert 모드 토글
			if (insertmode == true)
			{
				insertmode = false;
				overwriteMode = false;
			}
			else
			{
				insertmode = true;
				overwriteMode = true;
			}
		}

		if (wParam == VK_DELETE)
		{
			// 단어 삭제 함수 호출
			DeleteWordAtCaret(hWnd);
			return 0;
		}

		if (wParam == VK_PRIOR) // PageUp: 3줄 위로
		{
			if (curLine > 0)
			{
				for (int i = 0; i < 3; ++i) curLine--;
			}
			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_NEXT) // PageDown: 3줄 아래로
		{
			if (curLine < 10)
			{
				for (int i = 0; i < 3; ++i) curLine++;
			}
			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		break;
	}

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		SetBkMode(hDC, TRANSPARENT);

		// 현재 폰트 줄 높이
		TEXTMETRIC tm;
		GetTextMetrics(hDC, &tm);
		int charH = tm.tmHeight;

		// 전체 줄 출력
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

	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}