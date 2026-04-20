#include <windows.h>
#include <cstdlib>
#include <ctime>

// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 클래스 이름 / 창 제목
const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";
int mx[100] = {};
int my[100] = {};
int obcount = 60;
int crash = -1;
int g_rect[900][4];
int g_player[900][4];
int g_ob[900][4];
int erasecount = 0;
HBRUSH boardbrush;
HBRUSH playerbrush;
HBRUSH obbrush;
int randcolor[100] = {};
int obposition[100] = {};
int px = 0;
int py = 0;
int obx[100] = {};
int oby[100] = {};
int enterpress = 1;

void makeboard()
{
	int w = 20;
	int h = 20;
	for (int i = 0; i < 30; ++i)
	{
		for (int j = 0; j < 30; ++j)
		{
			int count = i * 30 + j;
			g_rect[count][0] = i * w + 20;
			g_rect[count][1] = j * h + 20;
			g_rect[count][2] = g_rect[count][0] + w;
			g_rect[count][3] = g_rect[count][1] + h;

		}
	}
}
void player()
{
	int w = 20;
	int h = 20;
	for (int i = 0; i < 30; ++i)
	{
		for (int j = 0; j < 30; ++j)
		{
			int count = i * 30 + j;
			g_player[count][0] = i * w + 20;
			g_player[count][1] = j * h + 20;
			g_player[count][2] = g_player[count][0] + w;
			g_player[count][3] = g_player[count][1] + h;

		}
	}
}
void ob()
{
	int w = 20;
	int h = 20;
	for (int i = 0; i < 28; ++i)
	{
		for (int j = 0; j < 25; ++j)
		{
			int count = i * 30 + j;
			g_ob[count][0] = i * w + 60;
			g_ob[count][1] = j * h + 60;
			g_ob[count][2] = g_ob[count][0] + w;
			g_ob[count][3] = g_ob[count][1] + h;

		}
	}
}
void CALLBACK PlayerTimerProc(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime)
{


	if (crash == -1)
	{
		for (int i = 0; i < 60; ++i)
		{
			if (g_ob[obposition[i]][3] + oby[i] == g_player[450][1] + py && g_player[450][0] + px == g_ob[obposition[i]][0] + obx[i])
			{
				crash = i;
				oby[crash] += 20;
			}
		}
	}
	else
	{
		oby[crash] += 20;
	}

	py += 20;
	if (g_player[450][3] + py > 620)
	{
		py = 0;
		crash = -1;
	}
	erasecount = 0;
	int lastrow[60] = {};
	for (int i = 0; i < 60; ++i)
	{
		if (g_ob[obposition[i]][3] + oby[i] +my[i] == 620)
		{
			lastrow[erasecount] = i;
			erasecount++;
		}
	}
	if (erasecount == 30)
	{
		bool sameColor = true;
		for (int i = 0; i < erasecount - 1; ++i)
		{
			if (randcolor[lastrow[i]] != randcolor[lastrow[i + 1]])
			{
				sameColor = false;
				break;
			}
		}
		if (sameColor)
		{
			// 삭제 처리
			for (int i = 0; i < erasecount; ++i)
			{
				oby[lastrow[i]] = -9999;  // 화면 밖으로 보내기
			}
		}
	}
	

		erasecount = 0;
	int lastrow2[100] = {};
	for (int i = 60; i < obcount; ++i)
	{
		if (my[i] + 30 == 620)
		{
			lastrow2[erasecount] = i;
			erasecount++;
		}
	}
	if (erasecount == 30)
	{
		bool sameColor = true;
		for (int i = 0; i < erasecount - 1; ++i)
		{
			if (randcolor[lastrow2[i]] != randcolor[lastrow2[i + 1]])
			{
				sameColor = false;
				break;
			}
		}
		if (sameColor)
		{
			for (int i = 0; i < erasecount; ++i)
			{
				my[lastrow2[i]] = -9999;
			}
		}
	}
	InvalidateRect(hWnd, NULL, TRUE);
}
// 프로그램 시작점
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	// 1) 윈도우 클래스 등록
	WNDCLASSEXW wc = {};
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;


	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = CLASS_NAME;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	RegisterClassExW(&wc);

	// 2) 윈도우 생성
	HWND hWnd = CreateWindowExW(
		0,
		CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1000, 1000,
		nullptr, nullptr, hInstance, nullptr
	);

	if (!hWnd) return 0;

	// 3) 창 표시
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 4) 메시지 루프
	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

// 윈도우 메시지 처리 함수
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static BOOL Selection;
	switch (msg)
	{
	case WM_CREATE:
	{
		srand((unsigned)time(NULL));
		for (int i = 0; i < 60; ++i)
		{
			randcolor[i] = rand() % 4 + 1;
			obposition[i] = rand() % 900;
			if (randcolor[i] == 1)
			{
				randcolor[i] = RGB(255, 0, 0);
			}
			if (randcolor[i] == 2)
			{
				randcolor[i] = RGB(0, 255, 0);

			}
			if (randcolor[i] == 3)
			{
				randcolor[i] = RGB(0, 0, 255);
			}
			if (randcolor[i] == 4)
			{
				randcolor[i] = RGB(0, 255, 255);

			}
		}
		playerbrush = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
		boardbrush = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
		makeboard();
		player();
		ob();
		SetTimer(hWnd, 1, 500, (TIMERPROC)PlayerTimerProc);

		return 0;

	}
	case WM_LBUTTONDOWN:
	{
		mx[obcount] = (LOWORD(lParam) / 20) * 20;
		my[obcount] = (HIWORD(lParam) / 20) * 20;
		if (mx[obcount] >= 20 && mx[obcount] < 620 && my[obcount] >= 20 && my[obcount] < 620)
		{
			randcolor[obcount] = RGB(255, 0, 0);
			obcount++;
			InvalidateRect(hWnd, nullptr, TRUE);
		}
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		int clickX = LOWORD(lParam);
		int clickY = HIWORD(lParam);

		// 기존 장애물 (0~59)
		for (int i = 0; i < 60; ++i)
		{
			if (clickX >= g_ob[obposition[i]][0] + obx[i] && clickX <= g_ob[obposition[i]][2] + obx[i]
				&& clickY >= g_ob[obposition[i]][1] + oby[i] && clickY <= g_ob[obposition[i]][3] + oby[i])
			{
				oby[i] = -9999;
			}
		}

		// 클릭 장애물 (60~)
		for (int i = 60; i < obcount; ++i)
		{
			if (clickX >= mx[i] && clickX <= mx[i] + 20
				&& clickY >= my[i] && clickY <= my[i] + 20)
			{
				my[i] = -9999;
			}
		}

		InvalidateRect(hWnd, nullptr, TRUE);
		return 0;
	}
	case WM_KEYDOWN:
	{
		if (wParam == VK_LEFT)
		{
			
			px -= 20;
			for (int i = 0; i < 60; ++i)
			{
				if (g_player[450][2] + px == g_ob[obposition[i]][0] + obx[i] && g_ob[obposition[i]][3] + oby[i] == g_player[450][1] + py)
				{
					obx[i] -= 20;
				}

			}
		}
		if (wParam == VK_RIGHT)
		{
			px += 20;
			for (int i = 0; i < 60; ++i)
			{
				if (g_player[450][0] + px == g_ob[obposition[i]][2] + obx[i] && g_ob[obposition[i]][3] + oby[i] == g_player[450][1] + py)
				{
					obx[i] += 20;
				}
			}
		}
		if (wParam == VK_RETURN)
		{
			if (enterpress == 1)
			{
			obx[crash] += 20;
			oby[crash] += 20;
			enterpress++;
			}
			else if (enterpress == 2)
			{
				obx[crash] -= 20;
				oby[crash] += 20;
				enterpress++;
			}
			else if (enterpress == 3)
			{
				obx[crash] -= 20;
				oby[crash] -= 20;
				enterpress++;
			}
			else if (enterpress == 4)
			{
				obx[crash] += 20;
				oby[crash] -= 20;
				enterpress =1;
			}
		}
		if (wParam == 'Q')
		{
			PostQuitMessage(0);
		}
		if (wParam == 'R')
		{
			crash = -1;
			obcount = 60;
			for (int i = 0; i < 60; ++i)
			{
				randcolor[i] = rand() % 4 + 1;
				obposition[i] = rand() % 900;
				if (randcolor[i] == 1)
				{
					randcolor[i] = RGB(255, 0, 0);
				}
				if (randcolor[i] == 2)
				{
					randcolor[i] = RGB(0, 255, 0);

				}
				if (randcolor[i] == 3)
				{
					randcolor[i] = RGB(0, 0, 255);
				}
				if (randcolor[i] == 4)
				{
					randcolor[i] = RGB(0, 255, 255);

				}
			}
			px = 0;
			py = 0;
		}
		InvalidateRect(hWnd, nullptr, TRUE);
		return 0;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, boardbrush);
		for (int i = 0;i < 900; ++i)
		{
			Rectangle(hdc, g_rect[i][0], g_rect[i][1], g_rect[i][2], g_rect[i][3]);
		}
		SelectObject(hdc, oldbrush);

		oldbrush = (HBRUSH)SelectObject(hdc, playerbrush);
		Ellipse(hdc, g_player[450][0] + px, g_player[450][1] + py, g_player[450][2] + px, g_player[450][3] + py);
		SelectObject(hdc, oldbrush);

		for (int i = 0; i < 60; ++i)
		{
			obbrush = (HBRUSH)CreateSolidBrush(randcolor[i]);
			oldbrush = (HBRUSH)SelectObject(hdc, obbrush);
			Rectangle(hdc, g_ob[obposition[i]][0] + obx[i] , g_ob[obposition[i]][1] + oby[i] , g_ob[obposition[i]][2] + obx[i] , g_ob[obposition[i]][3] + oby[i] );
			SelectObject(hdc, oldbrush);
		}
		for (int i = 60; i < obcount; ++i)
		{
			obbrush = (HBRUSH)CreateSolidBrush(randcolor[i]);
			oldbrush = (HBRUSH)SelectObject(hdc, obbrush);
			Rectangle(hdc, mx[i], my[i], mx[i] + 20, my[i] + 20);
			SelectObject(hdc, oldbrush);
		}
		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		DeleteObject(boardbrush);
		DeleteObject(playerbrush);
		DeleteObject(obbrush);
		KillTimer(hWnd, 1);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}