#include <windows.h>
#include <cstdlib> // srand, rand
#include <ctime>   // time

// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 윈도우 클래스/제목 문자열
const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

// 도형별 브러시(색 채우기용)
HBRUSH triBrush;
HBRUSH rectBrush;
HBRUSH wonBrush;

// -------------------- 삼각형 데이터 --------------------
int triangle = 0;       // 현재 그릴 삼각형 개수(1~3)
POINT g_tri[5][3];      // 최대 5개 삼각형, 각 삼각형은 꼭짓점 3개

void maketriangle()
{
	triangle = rand() % 3 + 1; // 1~3개 생성
	for (int i = 0; i < triangle; ++i)
	{
		int triW = 30; // 삼각형 폭
		int triH = 40; // 삼각형 높이

		// 왼쪽 영역(대략 x<250)에서 랜덤 위치 생성
		int left = 0 + rand() % 100 - triW + 40;
		int top = 0 + rand() % 400 - triH + 50;

		// 정삼각형에 가까운 3점 저장
		g_tri[i][0] = { left,            top + triH }; // 좌하
		g_tri[i][1] = { left + triW / 2, top };        // 상단
		g_tri[i][2] = { left + triW,     top + triH }; // 우하
	}
}

// -------------------- 사각형 데이터 --------------------
int rectangle = 0;      // 현재 그릴 사각형 개수(1~3)
int g_rect[5][4];       // [i][0..3] = left,top,right,bottom

void makerectangle()
{
	rectangle = rand() % 3 + 1; // 1~3개 생성
	for (int i = 0; i < rectangle; ++i)
	{
		int w = 40, h = 30; // 사각형 크기

		// 가운데 영역(251~500 사이)에 들어오도록 좌표 생성
		int left = 251 + rand() % (500 - 251 - w + 1); // 251~460
		int top = 0 + rand() % (550 - h + 1);          // 0~520

		int right = left + w;
		int bottom = top + h;

		// 좌표 저장
		g_rect[i][0] = { left };
		g_rect[i][1] = { top };
		g_rect[i][2] = { right };
		g_rect[i][3] = { bottom };
	}
}

// -------------------- 원 데이터 --------------------
int won = 0;            // 현재 그릴 원 개수(1~3)
int g_won[5][4];        // [i][0..3] = left,top,right,bottom (Ellipse 경계 사각형)

void makewon()
{
	won = rand() % 3 + 1; // 1~3개 생성
	for (int i = 0; i < won; ++i)
	{
		int w = 40, h = 30; // 원의 경계 사각형 크기

		// 오른쪽 영역(대략 500~700) 안에서 생성
		int left = 500 + rand() % (700 - 501 - w + 1);
		int top = 0 + rand() % (550 - h + 1);

		int right = left + w;
		int bottom = top + h;

		// 좌표 저장
		g_won[i][0] = { left };
		g_won[i][1] = { top };
		g_won[i][2] = { right };
		g_won[i][3] = { bottom };
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	// 1) 윈도우 클래스 등록
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;       // 가로/세로 변경 시 재그리기
	wc.lpfnWndProc = WndProc;                 // 메시지 처리 함수
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // 기본 배경
	wc.lpszClassName = CLASS_NAME;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	RegisterClassExW(&wc);

	// 2) 윈도우 생성
	HWND hWnd = CreateWindowExW(
		0,
		CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		nullptr, nullptr, hInstance, nullptr
	);

	if (!hWnd) return 0; // 생성 실패 시 종료

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 3) 메시지 루프
	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		// 프로그램 시작 시 초기화
		srand((unsigned)time(NULL)); // 난수 시드 초기화

		// 도형별 랜덤 색 브러시 생성
		triBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
		rectBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
		wonBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));

		// 초기 도형 데이터 생성
		maketriangle();
		makerectangle();
		makewon();
		return 0;

	case WM_KEYDOWN:
		// 키 입력 처리
		if (wParam == VK_RETURN)
		{
			// Enter: 모든 도형의 색/위치/개수 재랜덤
			DeleteObject(triBrush);
			DeleteObject(rectBrush);
			DeleteObject(wonBrush);

			triBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
			rectBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
			wonBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));

			maketriangle();
			makerectangle();
			makewon();

			InvalidateRect(hWnd, NULL, TRUE); // 다시 그리기 요청
		}

		if (wParam == '1')
		{
			// 1: 삼각형만 재랜덤
			DeleteObject(triBrush);
			triBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
			maketriangle();
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == '2')
		{
			// 2: 사각형만 재랜덤
			DeleteObject(rectBrush);
			rectBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
			makerectangle();
			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == '3')
		{
			// 3: 원만 재랜덤
			DeleteObject(wonBrush);
			wonBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
			makewon();
			InvalidateRect(hWnd, NULL, TRUE);
		}
		return 0;

	case WM_CHAR:
		// 문자 입력 메시지 (현재 사용 안 함)
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// 영역 구분선(3칸) 그리기
		Rectangle(hdc, 0, 0, 250, 550);    // 왼쪽
		Rectangle(hdc, 251, 0, 500, 550);  // 가운데
		Rectangle(hdc, 501, 0, 760, 550);  // 오른쪽

		// --- 삼각형 그리기 ---
		HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, triBrush);
		for (int i = 0; i < triangle; ++i)
			Polygon(hdc, g_tri[i], 3); // 저장 좌표로 삼각형 그림
		SelectObject(hdc, oldBrush);

		// --- 사각형 그리기 ---
		oldBrush = (HBRUSH)SelectObject(hdc, rectBrush);
		for (int i = 0; i < rectangle; ++i)
		{
			Rectangle(hdc, g_rect[i][0], g_rect[i][1], g_rect[i][2], g_rect[i][3]);
		}
		SelectObject(hdc, oldBrush);

		// --- 원 그리기 ---
		oldBrush = (HBRUSH)SelectObject(hdc, wonBrush);
		for (int i = 0; i < won; ++i)
		{
			Ellipse(hdc, g_won[i][0], g_won[i][1], g_won[i][2], g_won[i][3]);
		}
		SelectObject(hdc, oldBrush);

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		// GDI 리소스 해제 후 종료
		DeleteObject(triBrush);
		DeleteObject(rectBrush);
		DeleteObject(wonBrush);
		PostQuitMessage(0);
		return 0;
	}

	// 기본 메시지 처리
	return DefWindowProc(hWnd, msg, wParam, lParam);
}