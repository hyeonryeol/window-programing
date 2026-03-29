#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"메모장";

int cursorX = 10;
int cursorY = 10;
TCHAR lines[10][31];         // 최대 10줄, 각 30자
int lineLens[10];            // 각 줄 길이
int curLine = 0;            // 현재 줄
int totalLines = 1;         // 총 줄 수

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;

	// 줄 초기화
	for (int i = 0; i < 10; i++) {
		lineLens[i] = 0;
		lines[i][0] = L'\0';
	}
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
		100, 100, 800, 600, NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

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

		CreateCaret(hWnd, NULL, 2, 20); // 너비2, 높이20 캐럿 생성
		ShowCaret(hWnd);                // 캐럿 표시
		SetCaretPos(10, 10);            // 초기 위치
		break;

	case WM_CHAR:
	{
		TCHAR ch = (TCHAR)wParam;
		if (isalpha(ch))
		{
			if (lineLens[curLine] < 30) // 20자 미만이면 현재 줄에 추가
			{
				lines[curLine][lineLens[curLine]++] = ch;
				lines[curLine][lineLens[curLine]] = L'\0';
				// 글자 크기 측정
				HDC hDC = GetDC(hWnd);
				SIZE sz;
				GetTextExtentPoint32(hDC, &ch, 1, &sz);
				ReleaseDC(hWnd, hDC);

				// 캐럿 오른쪽으로 이동
				cursorX += sz.cx;
				SetCaretPos(cursorX, cursorY);

				InvalidateRect(hWnd, NULL, TRUE);

			}
			else // 30자 넘으면 다음 줄로 자동 이동
			{
				if (curLine < 9)
				{
					curLine++;
					if (curLine >= totalLines) totalLines = curLine + 1;
					lines[curLine][0] = ch;
					lineLens[curLine] = 1;
					lines[curLine][1] = L'\0';


					HDC hDC = GetDC(hWnd);
					TEXTMETRIC tm;
					GetTextMetrics(hDC, &tm);
					SIZE sz;
					GetTextExtentPoint32(hDC, &ch, 1, &sz);
					ReleaseDC(hWnd, hDC);

					cursorX = 10 + sz.cx;           // x는 첫 글자 다음으로
					cursorY = 10 + curLine * tm.tmHeight; // y는 현재 줄 위치로
					SetCaretPos(cursorX, cursorY);

				}
				else //  10줄 꽉 찼으면 첫 줄 첫 칸으로
				{
					curLine = 0;

					// 첫 줄 덮어쓰기
					lines[curLine][0] = ch;
					lineLens[curLine] = 1;
					lines[curLine][1] = L'\0';

					HDC hDC = GetDC(hWnd);
					SIZE sz;
					GetTextExtentPoint32(hDC, &ch, 1, &sz);
					ReleaseDC(hWnd, hDC);

					cursorX = 10 + sz.cx; // 첫 글자 다음
					cursorY = 10;          // 첫 줄 y위치
					SetCaretPos(cursorX, cursorY);
				}
				
			}
			InvalidateRect(hWnd, NULL, TRUE);
		
		}
	
		
		
		else if (ch == '\b') // 백스페이스 → 마지막 문자 삭제
		{
			if (lineLens[curLine] > 0)
			{
				TCHAR deleted = lines[curLine][lineLens[curLine] - 1];


				lineLens[curLine]--;
				lines[curLine][lineLens[curLine]] = L'\0';

				//  캐럿 왼쪽으로 이동
				HDC hDC = GetDC(hWnd);
				SIZE sz;
				
				GetTextExtentPoint32(hDC, &deleted, 1, &sz);
				ReleaseDC(hWnd, hDC);

				cursorX -= sz.cx;  // 왼쪽으로
				SetCaretPos(cursorX, cursorY);

				InvalidateRect(hWnd, NULL, TRUE);
			}
			else if (curLine > 0) //  현재 줄이 비어있고 이전 줄이 있으면
			{
				curLine--;
				totalLines--;

				// 캐럿을 이전 줄 끝으로 이동
				HDC hDC = GetDC(hWnd);
				TEXTMETRIC tm;
				GetTextMetrics(hDC, &tm);
				SIZE sz;
				GetTextExtentPoint32(hDC, lines[curLine], lineLens[curLine], &sz);
				ReleaseDC(hWnd, hDC);

				cursorX = 10 + sz.cx;              // 이전 줄 마지막 글자 다음
				cursorY = 10 + curLine * tm.tmHeight; // 이전 줄 y위치
				SetCaretPos(cursorX, cursorY);
				InvalidateRect(hWnd, NULL, TRUE);
			}
			
		}
		else if (ch == 27) // ESC → 종료
		{
			// 모든 줄 초기화
			for (int i = 0; i < 10; i++)
			{
				lineLens[i] = 0;
				lines[i][0] = L'\0';
			}
			curLine = 0;
			totalLines = 1;
			cursorX = 10;  // 캐럿 초기 위치로
			cursorY = 10;
			SetCaretPos(cursorX, cursorY);
			InvalidateRect(hWnd, NULL, TRUE);
		}
	}
		

	case WM_KEYDOWN:
	{

		break;
	}

	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);
		SetBkMode(hDC, TRANSPARENT);

		// 글자 높이 측정
		TEXTMETRIC tm;
		GetTextMetrics(hDC, &tm);
		int charH = tm.tmHeight;

		// 모든 줄 출력
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