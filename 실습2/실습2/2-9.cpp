#include <windows.h>   // Win32 API (창, 메시지, GDI)
#include <tchar.h>     // TCHAR, _istdigit, _toupper/_tolower 등
#include <time.h>
#include <stdlib.h>
#include <tchar.h> // 대소문자/문자분류 관련 (중복 include지만 동작에는 문제 없음)

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"메모장";

// 캐럿(커서) 화면 좌표
int cursorX = 10;
int cursorY = 10;

// 텍스트 버퍼: 최대 10줄, 줄당 최대 30자 + '\0'
TCHAR lines[10][31];
int lineLens[10];      // 각 줄의 실제 길이
int curLine = 0;       // 현재 줄 인덱스
int totalLines = 1;    // 현재 사용 중인 총 줄 수
int curidx = 0;        // 현재 줄 내 커서 문자 위치
bool overwriteMode = false; // 덮어쓰기 성격 플래그
bool insertmode = false;    // 삽입 모드 여부
bool uppermode = false;     // 대문자 입력 모드(F1)
bool gongback = false;      // F3에서 괄호 번갈아 넣기용 상태

// F5 토글(마스킹)용 백업 버퍼
bool f5Masked = false;
TCHAR backupLines[10][31];
int backupLens[10];

// 현재 curLine/curidx 기준으로 캐럿 좌표를 계산해 반영
void UpdateCaret(HWND hWnd)
{
	HDC hDC = GetDC(hWnd);
	TEXTMETRIC tm{};
	GetTextMetrics(hDC, &tm);

	// 인덱스 경계 보정
	if (curidx < 0) curidx = 0;
	if (curidx > lineLens[curLine]) curidx = lineLens[curLine];

	SIZE sz{ 0,0 };
	// 줄 시작~curidx까지의 폭을 구해 X 좌표 계산
	if (curidx > 0)
		GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);

	ReleaseDC(hWnd, hDC);

	cursorX = 10 + sz.cx;
	cursorY = 10 + curLine * tm.tmHeight;
	SetCaretPos(cursorX, cursorY);
}

// 현재 편집 상태를 백업
void BackupAllLines()
{
	for (int i = 0; i < 10; ++i)
	{
		lstrcpy(backupLines[i], lines[i]);
		backupLens[i] = lineLens[i];
	}
}

// 백업 상태를 복원
void RestoreAllLines()
{
	for (int i = 0; i < 10; ++i)
	{
		lstrcpy(lines[i], backupLines[i]);
		lineLens[i] = backupLens[i];
	}
}

// 각 줄에서 "가장 많이 나온 문자(공백 제외)"를 '@'로 치환
void MaskMostFrequentPerLine()
{
	for (int i = 0; i < totalLines; ++i)
	{
		int freq[65536] = { 0 }; // wchar 범위 대응 카운팅
		int bestCnt = 0;
		TCHAR bestCh = 0;

		// 문자 빈도 계산
		for (int j = 0; j < lineLens[i]; ++j)
		{
			TCHAR c = lines[i][j];
			if (c == L' ') continue; // 공백 제외
			freq[(unsigned short)c]++;
			if (freq[(unsigned short)c] > bestCnt)
			{
				bestCnt = freq[(unsigned short)c];
				bestCh = c;
			}
		}

		// 최빈문자 '@' 치환
		if (bestCnt > 0)
		{
			for (int j = 0; j < lineLens[i]; ++j)
			{
				if (lines[i][j] == bestCh)
					lines[i][j] = L'@';
			}
		}
	}
}

// Delete 키: 커서 위치 기준으로 "다음 단어" 삭제
void DeleteWordAtCaret(HWND hWnd)
{
	int len = lineLens[curLine];
	if (len == 0) return;

	// 인덱스 경계 보정
	if (curidx < 0) curidx = 0;
	if (curidx > len) curidx = len;

	int pos = curidx;

	// 1) 현재가 공백이면 공백을 건너뛰고
	while (pos < len && lines[curLine][pos] == L' ')
		pos++;

	// 삭제할 단어가 없으면 종료
	if (pos >= len) {
		if (curidx > len) curidx = len;
		UpdateCaret(hWnd);
		InvalidateRect(hWnd, NULL, TRUE);
		return;
	}

	// 2) 단어 구간 [start, end) 찾기
	int start = pos;
	int end = pos;
	while (end < len && lines[curLine][end] != L' ')
		end++;

	// 3) 삭제: 뒤 문자열을 앞으로 당김('\0' 포함)
	memmove(&lines[curLine][start],
		&lines[curLine][end],
		(len - end + 1) * sizeof(TCHAR));

	lineLens[curLine] -= (end - start);

	// 4) 커서 보정 및 갱신
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

	// 줄 버퍼 초기화
	for (int i = 0; i < 10; i++) {
		lineLens[i] = 0;
		lines[i][0] = L'\0';
	}

	// 윈도우 클래스 등록
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

	// 메인 창 생성
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
		// 캐럿 생성/표시/초기위치
		CreateCaret(hWnd, NULL, 2, 20);
		ShowCaret(hWnd);
		SetCaretPos(10, 10);
		break;

	case WM_CHAR:
	{
		TCHAR ch = (TCHAR)wParam;

		// F1 모드: 알파벳이면 대문자로 변환 후 입력
		if (uppermode == true)
		{
			if (isalpha(ch))
			{
				ch = _toupper(ch);

				if (curidx < 30)
				{
					// 현재 위치에 문자 기록
					lines[curLine][curidx] = ch;

					// overwriteMode가 아니면 길이/널 처리
					if (!overwriteMode)
					{
						lines[curLine][curidx + 1] = L'\0';
						if (curidx == lineLens[curLine])
						{
							lineLens[curLine]++;
							lines[curLine][curidx + 1] = L'\0';
						}
					}
					curidx++;

					// 캐럿 위치 갱신
					HDC hDC = GetDC(hWnd);
					SIZE sz;
					GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);
					ReleaseDC(hWnd, hDC);

					cursorX = 10 + sz.cx;
					SetCaretPos(cursorX, cursorY);

					InvalidateRect(hWnd, NULL, TRUE);
				}
				else // 줄 끝이면 다음 줄로
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
		else
		{
			// 일반 입력 모드(삽입모드가 아닐 때)
			if (insertmode == false)
			{
				if (32 <= ch)
				{
					if (curidx < 30)
					{
						lines[curLine][curidx] = ch;

						if (!overwriteMode)
						{
							lines[curLine][curidx + 1] = L'\0';
							if (curidx == lineLens[curLine])
							{
								lineLens[curLine]++;
								lines[curLine][curidx + 1] = L'\0';
							}
						}
						curidx++;

						HDC hDC = GetDC(hWnd);
						SIZE sz;
						GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);
						ReleaseDC(hWnd, hDC);

						cursorX = 10 + sz.cx;
						SetCaretPos(cursorX, cursorY);

						InvalidateRect(hWnd, NULL, TRUE);
					}
					else // 줄 넘침 -> 다음 줄
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
						else
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
		}

		// 삽입 모드: 현재 위치에 삽입(오른쪽으로 밀기)
		if (insertmode == true)
		{
			if (32 <= ch)
			{
				if (curidx < 30)
				{
					int len = lineLens[curLine];
					if (curidx < 0) curidx = 0;
					if (curidx > len) curidx = len;

					for (int i = len; i >= curidx; --i) {
						lines[curLine][i + 1] = lines[curLine][i];
					}

					lines[curLine][curidx] = ch;
					lineLens[curLine]++;
					// 필요하면 curidx++ 해도 됨
				}
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
		// 주의: 이 else-if는 바로 위 if(insertmode==true)에 결합됨
		else if (ch == '\b') // 백스페이스
		{
			if (overwriteMode)
			{
				if (curidx == 0) return 0;

				// 현재 줄에서 삭제
				for (int i = curidx - 1; i < lineLens[curLine] - 1; i++)
				{
					lines[curLine][i] = lines[curLine][i + 1];
				}

				lineLens[curLine]--;
				curidx--;
				lines[curLine][lineLens[curLine]] = L'\0';

				// 다음 줄 첫 글자 당겨오기
				if (curLine < totalLines - 1 && lineLens[curLine] < 30)
				{
					TCHAR nextChar = lines[curLine + 1][0];

					lines[curLine][lineLens[curLine]] = nextChar;
					lineLens[curLine]++;
					lines[curLine][lineLens[curLine]] = L'\0';

					for (int i = 0; i < lineLens[curLine + 1] - 1; i++)
					{
						lines[curLine + 1][i] = lines[curLine + 1][i + 1];
					}

					lineLens[curLine + 1]--;
					lines[curLine + 1][lineLens[curLine + 1]] = L'\0';

					// 빈 줄 정리
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
				if (lineLens[curLine] > 0)
				{
					TCHAR deleted = lines[curLine][lineLens[curLine] - 1];

					lineLens[curLine]--;
					curidx--;
					lines[curLine][lineLens[curLine]] = L'\0';
					curidx = lineLens[curLine];

					HDC hDC = GetDC(hWnd);
					SIZE sz;
					GetTextExtentPoint32(hDC, &deleted, 1, &sz);
					ReleaseDC(hWnd, hDC);

					cursorX -= sz.cx;
					SetCaretPos(cursorX, cursorY);

					InvalidateRect(hWnd, NULL, TRUE);
				}
				else if (curLine > 0)
				{
					curidx--;
					curLine--;

					for (int i = curidx; i < lineLens[curLine] - 1; i++)
					{
						lines[curLine][i] = lines[curLine][i + 1];
					}

					lineLens[curLine]--;
					lines[curLine][lineLens[curLine]] = L'\0';

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

		// 방향/기능키 처리
		TCHAR ch = (TCHAR)wParam;

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

			HDC hDC = GetDC(hWnd);
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			SIZE sz;
			GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);
			ReleaseDC(hWnd, hDC);

			cursorX = 10 + sz.cx;
			cursorY = 10 + curLine * tm.tmHeight;
			SetCaretPos(cursorX, cursorY);
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

			HDC hDC = GetDC(hWnd);
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			SIZE sz;
			GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);
			ReleaseDC(hWnd, hDC);

			cursorX = 10 + sz.cx;
			cursorY = 10 + curLine * tm.tmHeight;
			SetCaretPos(cursorX, cursorY);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_UP)
		{
			if (lines[curLine - 1][curidx] != L'\0')
			{
				if (curLine > 0) curLine--;
			}

			HDC hDC = GetDC(hWnd);
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			SIZE sz;
			GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);
			ReleaseDC(hWnd, hDC);

			cursorX = 10 + sz.cx;
			cursorY = 10 + curLine * tm.tmHeight;
			SetCaretPos(cursorX, cursorY);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_DOWN)
		{
			if (lines[curLine + 1][curidx] != L'\0')
			{
				if (curLine < 10) curLine++;
			}

			HDC hDC = GetDC(hWnd);
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			SIZE sz;
			GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);
			ReleaseDC(hWnd, hDC);

			cursorX = 10 + sz.cx;
			cursorY = 10 + curLine * tm.tmHeight;
			SetCaretPos(cursorX, cursorY);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_TAB)
		{
			// 공백 3개 삽입
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
				curLine++;
				if (curLine >= totalLines) totalLines = curLine + 1;
				curidx = 0;
			}

			HDC hDC = GetDC(hWnd);
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			SIZE sz;
			GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);
			ReleaseDC(hWnd, hDC);

			cursorX = 10 + sz.cx;
			cursorY = 10 + curLine * tm.tmHeight;
			SetCaretPos(cursorX, cursorY);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_HOME)
		{
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

		if (wParam == VK_END)
		{
			curidx = lineLens[curLine];

			HDC hDC = GetDC(hWnd);
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			SIZE sz;
			GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);
			ReleaseDC(hWnd, hDC);

			cursorX = 10 + sz.cx;
			cursorY = 10 + curLine * tm.tmHeight;
			SetCaretPos(cursorX, cursorY);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_INSERT)
		{
			// 삽입/덮어쓰기 토글
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
			DeleteWordAtCaret(hWnd);
			return 0;
		}

		if (wParam == VK_PRIOR) // PageUp: 3줄 위
		{
			if (curLine > 0)
			{
				for (int i = 0; i < 3; ++i) curLine--;
			}
			HDC hDC = GetDC(hWnd);
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			SIZE sz;
			GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);
			ReleaseDC(hWnd, hDC);

			cursorX = 10 + sz.cx;
			cursorY = 10 + curLine * tm.tmHeight;
			SetCaretPos(cursorX, cursorY);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_NEXT) // PageDown: 3줄 아래
		{
			if (curLine < 10)
			{
				for (int i = 0; i < 3; ++i) curLine++;
			}
			HDC hDC = GetDC(hWnd);
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			SIZE sz;
			GetTextExtentPoint32(hDC, lines[curLine], curidx, &sz);
			ReleaseDC(hWnd, hDC);

			cursorX = 10 + sz.cx;
			cursorY = 10 + curLine * tm.tmHeight;
			SetCaretPos(cursorX, cursorY);
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == VK_F1)
		{
			// 대문자 모드 토글
			uppermode = !uppermode;
		}

		if (wParam == VK_F2)
		{
			// 숫자 앞에 **** 삽입
			const int STAR_CNT = 4;

			for (int i = 0; i < totalLines; i++)
			{
				int j = 0;
				while (j < lineLens[i])
				{
					if (_istdigit(lines[i][j]) && (j == 0 || lines[i][j - 1] != L'*'))
					{
						// 오른쪽으로 4칸 밀기
						for (int k = lineLens[i]; k >= j; --k)
							lines[i][k + STAR_CNT] = lines[i][k];

						// **** 삽입
						for (int s = 0; s < STAR_CNT; ++s)
							lines[i][j + s] = L'*';

						lineLens[i] += STAR_CNT;
						j += STAR_CNT + 1;
					}
					else
					{
						j++;
					}
				}
			}

			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
			return 0;
		}

		if (wParam == VK_F3)
		{
			// 공백마다 괄호 삽입: '('와 ')' 번갈아
			const int STAR_CNT = 1;
			for (int i = 0; i < totalLines; i++)
			{
				int j = 0;
				while (j < lineLens[i])
				{
					if (lines[i][j] == L' ')
					{
						for (int k = lineLens[i]; k >= j; --k)
						{
							lines[i][k + STAR_CNT] = lines[i][k];
						}
						if (gongback == false)
						{
							lines[i][j] = L'(';
							gongback = true;
						}
						else
						{
							lines[i][j] = L')';
							gongback = false;
						}
						lineLens[i] += STAR_CNT;
						j += STAR_CNT + 1;
					}
					else
					{
						j++;
					}
				}
			}
			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
			return 0;
		}

		if (wParam == VK_F4)
		{
			// 1) 공백 제거
			for (int i = 0; i < totalLines; i++)
			{
				int j = 0;
				while (j < lineLens[i])
				{
					if (lines[i][j] == L' ')
					{
						for (int k = j; k < lineLens[i]; ++k)
							lines[i][k] = lines[i][k + 1];
						lineLens[i]--;
					}
					else
					{
						j++;
					}
				}
			}

			// 2) 대문자 -> 소문자
			for (int i = 0; i < totalLines; i++)
			{
				for (int j = 0; j < lineLens[i]; j++)
				{
					if ((isupper(lines[i][j])))
					{
						lines[i][j] = _tolower(lines[i][j]);
					}
				}
			}

			if (curidx > lineLens[curLine]) curidx = lineLens[curLine];
			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
			return 0;
		}

		if (wParam == VK_F5)
		{
			// 마스킹 토글
			if (!f5Masked)
			{
				BackupAllLines();
				MaskMostFrequentPerLine();
				f5Masked = true;
			}
			else
			{
				RestoreAllLines();
				f5Masked = false;
			}

			if (curidx > lineLens[curLine]) curidx = lineLens[curLine];
			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
			return 0;
		}

		if (wParam == VK_F6)
		{
			// 줄 순환: 첫 줄을 맨 아래로
			if (totalLines > 1)
			{
				TCHAR first[31];
				int firstLen = lineLens[0];
				lstrcpy(first, lines[0]);

				for (int i = 0; i < totalLines - 1; ++i)
				{
					lstrcpy(lines[i], lines[i + 1]);
					lineLens[i] = lineLens[i + 1];
				}

				lstrcpy(lines[totalLines - 1], first);
				lineLens[totalLines - 1] = firstLen;

				// 커서 줄도 함께 회전
				if (curLine == 0) curLine = totalLines - 1;
				else curLine--;

				if (curidx > lineLens[curLine]) curidx = lineLens[curLine];

				UpdateCaret(hWnd);
				InvalidateRect(hWnd, NULL, TRUE);
			}
			return 0;
		}

		if (wParam == VK_F7)
		{
			// 숫자 0~8을 +1
			for (int i = 0; i < totalLines; ++i)
			{
				for (int j = 0; j < lineLens[i]; ++j)
				{
					if (lines[i][j] >= L'0' && lines[i][j] <= L'8')
					{
						lines[i][j] = lines[i][j] + 1;
					}
				}
			}

			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
			return 0;
		}

		if (wParam == VK_F8)
		{
			// 숫자 1~9를 -1
			for (int i = 0; i < totalLines; ++i)
			{
				for (int j = 0; j < lineLens[i]; ++j)
				{
					if (lines[i][j] >= L'1' && lines[i][j] <= L'9')
					{
						lines[i][j] = lines[i][j] - 1;
					}
					else if (lines[i][j] == L'0')
					{
						// 0 처리 정책 없음(현재는 그대로)
					}
				}
			}

			UpdateCaret(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
			return 0;
		}
		break;
	}

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		SetBkMode(hDC, TRANSPARENT);

		// 줄 높이 구해 줄 간격으로 사용
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