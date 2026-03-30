#include <windows.h>
#include <tchar.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"메모장";

// 최대 10줄, 각 30글자
TCHAR lines[10][31];
int   lineLens[10];    // 각 줄의 글자 수
int   curLine = 0;     // 현재 줄 번호
int   curIdx = 0;     // 현재 줄의 글자 위치
int cursorX = 10;
int cursorY = 10;
bool overwrite = false;
int totalLines = 1;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdParam, int nCmdShow)
{
	HWND       hWnd;
	MSG        Message;
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
		100, 100, 800, 600,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}

	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage,
	WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC         hDC;

	switch (iMessage)
	{
	case WM_CREATE:
		// 줄 초기화
		for (int i = 0; i < 10; ++i)
		{
			lineLens[i] = 0;
			lines[i][0] = L'\0';
		}
		curLine = 0;
		curIdx = 0;

		// 캐럿(커서) 생성: 폭 2픽셀, 높이 20픽셀짜리 직사각형
		CreateCaret(hWnd, NULL, 2, 20);
		// 초기 위치 설정 (문자 폭 8 가정, 줄 간격 20 가정)
		SetCaretPos(cursorX, cursorY);
		// 캐럿 보이기
		ShowCaret(hWnd);
		break;

	case WM_CHAR:
	{
		TCHAR ch = (TCHAR)wParam;

		if (ch == 27) // ESC
		{
			for (int i = 0; i < 10; ++i)
			{
				lineLens[i] = 0;
				lines[i][0] = L'\0';
			}
			curLine = 0;
			curIdx = 0;
			cursorX = 10;
			cursorY = 10;
			overwrite = false;  
			int totalLines = 1;

			SetCaretPos(cursorX, cursorY);
			InvalidateRect(hWnd, NULL, TRUE);
			return 0;
		}

		// 엔터 → 다음 줄로 이동
		if (ch == '\r')
		{
			if (curLine < 9)
			{
				curLine++;
				curIdx = 0;
				if (curLine >= totalLines) totalLines = curLine + 1;
			}
			InvalidateRect(hWnd, NULL, TRUE);
			// 캐럿 위치 갱신
			HDC hDC = GetDC(hWnd);

			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);   // 글자 높이 가져오기

			SIZE sz;
			GetTextExtentPoint32(hDC, lines[curLine], curIdx, &sz); // 현재 줄에서 폭

			ReleaseDC(hWnd, hDC);

			cursorX = 10 + sz.cx;
			cursorY = 10 + curLine * tm.tmHeight;   // 여기서 tm.tmHeight 사용

			SetCaretPos(cursorX, cursorY);
			break;
		}

		// 출력 가능한 문자(알파벳/숫자/기호 등), 백스페이스는 제외
		if (isprint((unsigned char)ch) && ch != '\b')
		{
			if (!overwrite)
			{
				if (curIdx < 30)
				{
					lines[curLine][curIdx] = ch;
					curIdx++;
					lineLens[curLine] = curIdx;
					lines[curLine][curIdx] = L'\0';
					InvalidateRect(hWnd, NULL, TRUE);
				}
				else
				{
					if (curLine < 9)
					{
						curLine++;
						curIdx = 0;

						lines[curLine][curIdx] = ch;
						curIdx++;
						lineLens[curLine] = curIdx;
						lines[curLine][curIdx] = L'\0';

						if (curLine >= totalLines) totalLines = curLine + 1;
					}
					else
					{
						overwrite = true;

						curLine = 0;
						curIdx = 0;

						

						lines[curLine][curIdx] = ch;
						if (curIdx + 1 > lineLens[curLine])
							lineLens[curLine] = curIdx + 1;
						
						curIdx++;
						lines[curLine][curIdx] = L'\0';
					}
				}
			}
			else
			{
				lines[curLine][curIdx] = ch;

				if (curIdx + 1 > lineLens[curLine])
					lineLens[curLine] = curIdx + 1;
				curIdx++;

				if (curIdx >= 30)
				{
					curIdx = 0;
					curLine++;
					if (curLine > 9)
					{
						curLine = 0;
					}
				}
				lines[curLine][lineLens[curLine]] = L'\0';
			}
			
			InvalidateRect(hWnd, NULL, TRUE);

			// 캐럿 위치 갱신
			HDC hDC = GetDC(hWnd);

			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);   // 글자 높이

			SIZE sz;
			GetTextExtentPoint32(hDC, lines[curLine], curIdx, &sz);
			ReleaseDC(hWnd, hDC);

			cursorX = 10 + sz.cx;
			cursorY = 10 + curLine * tm.tmHeight; // tmHeight는 글자 높이
			SetCaretPos(cursorX, cursorY);
		}
	}
	break;

	case WM_KEYDOWN:
		if (wParam == VK_BACK)
		{
			if (curIdx > 0)
			{
				// 1) 현재 줄에서 한 글자 삭제 + 앞으로 당기기
				int delPos = curIdx - 1;

				for (int i = delPos; i < lineLens[curLine] - 1; ++i)
				{
					lines[curLine][i] = lines[curLine][i + 1];
				}

				lineLens[curLine]--;
				curIdx--;
				lines[curLine][lineLens[curLine]] = L'\0';
			}
			else if (curIdx == 0 && curLine > 0)
			{
				// 2) 줄 맨 앞에서 BKSP: 윗줄 끝과 합치고 아래 줄 당기기

				// 먼저 현재 줄을 윗줄 뒤에 이어붙이기
				int prevLine = curLine - 1;
				int prevLen = lineLens[prevLine];
				int thisLen = lineLens[curLine];

				// 현재 줄 글자를 prevLine 뒤에 이어붙임 (최대 30자까지)
				for (int i = 0; i < thisLen && prevLen + i < 30; ++i)
				{
					lines[prevLine][prevLen + i] = lines[curLine][i];
				}

				int newLen = prevLen + thisLen;
				if (newLen > 30) newLen = 30;

				lineLens[prevLine] = newLen;
				lines[prevLine][newLen] = L'\0';

				// 이제 현재 줄( curLine )은 "비워진 줄"이 된 상태.
				// 아래 줄들을 한 칸씩 위로 당긴다.
				for (int ln = curLine; ln < totalLines - 1; ++ln)
				{
					lstrcpy(lines[ln], lines[ln + 1]);
					lineLens[ln] = lineLens[ln + 1];
				}

				// 마지막 줄은 비우기
				lineLens[totalLines - 1] = 0;
				lines[totalLines - 1][0] = L'\0';

				totalLines--;
				curLine = prevLine;
				curIdx = lineLens[prevLine];
			}

			InvalidateRect(hWnd, NULL, TRUE);

			// 캐럿 위치 갱신
			HDC hDC = GetDC(hWnd);

			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);

			SIZE sz;
			GetTextExtentPoint32(hDC, lines[curLine], curIdx, &sz);
			ReleaseDC(hWnd, hDC);

			cursorX = 10 + sz.cx;
			cursorY = 10 + curLine * tm.tmHeight;
			SetCaretPos(cursorX, cursorY);
		}
		break;

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);

		TEXTMETRIC tm;
		GetTextMetrics(hDC, &tm);
		int charH = tm.tmHeight;

		for (int i = 0; i < totalLines; ++i)
		{
			TextOut(hDC, 10, 10 + i * charH, lines[i], lineLens[i]);
		}

		EndPaint(hWnd, &ps);
		break;
	}
		

	case WM_DESTROY:
		DestroyCaret();
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, iMessage, wParam, lParam);
	}

	return 0;
}