#include <windows.h>
#include <cstdlib>   // srand(), rand()
#include <ctime>     // time()

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";
int wonpress = 0;
int R = rand() % 255;
int G = rand() % 255;
int B = rand() % 255;
HBRUSH boardbrush;
HBRUSH wonbrush[10];
HPEN wonpen = nullptr;
int g_wonlist[10];

int g_rect[900][4];
int g_won[900][4];
void makerectangle()
{
	int w = 30;
	int h = 30;
	for (int i = 0; i < 30; ++i)
	{
		for (int j = 0; j < 30; ++j)
		{
			int count = i * 30 + j;

			int left = i * w + 30;
			int top = j * h + 30;
			int right = left + w;
			int bottom = top + h;

			g_rect[count][0] = left;
			g_rect[count][1] = top;
			g_rect[count][2] = right;
			g_rect[count][3] = bottom;
		}
	}
}
int makewon()
{
	int w = 30;
	int h = 30;

	int i = rand() % 30;
	int j = rand() % 30;

	int count = i * 30 + j;

	int left = i * w + 30;
	int top = j * h + 30;
	int right = left + w;
	int bottom = top + h;

	g_won[count][0] = left;
	g_won[count][1] = top;
	g_won[count][2] = right;
	g_won[count][3] = bottom;

	return count;
}

struct won
{
	int wonwon[10];
	int R = rand() % 255;
	int G = rand() % 255;
	int B = rand() % 255;

};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{

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


	HWND hWnd = CreateWindowExW(
		0,
		CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1000, 1000,
		nullptr, nullptr, hInstance, nullptr
	);

	if (!hWnd) return 0;


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}


won wonstruct[10];
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CHAR:
	{
		if (wParam == 'q' || wParam == 'Q')
		{
			PostQuitMessage(0);
		}
		return 0;
	}
	case WM_KEYDOWN:
	{
		if (wParam == 'E')
		{
			if (wonpress < 10)
			{
				g_wonlist[wonpress] = makewon();  // 어느 칸인지 저장
				wonbrush[wonpress] = CreateSolidBrush(RGB(wonstruct[wonpress].R, wonstruct[wonpress].G, wonstruct[wonpress].B));
				wonpress++;
				InvalidateRect(hWnd, nullptr, TRUE);
			}
			
		}
		if (wParam == '1')
		{
			g_wonlist[0];
			wonpen = CreatePen(PS_SOLID, 2, (RGB(255, 0, 0)));

			InvalidateRect(hWnd, nullptr, TRUE);
		}
		if(wParam == )
		return 0;
	}
	case WM_CREATE:
	{
		srand((unsigned int)time(nullptr));  // 시드 설정 (프로그램 시작 시 1번만!)
		boardbrush = CreateSolidBrush(RGB(255, 255, 255));
		
		
		makerectangle();
		
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, boardbrush);
		for (int i = 0; i < 900; ++i)
		{
			Rectangle(hdc, g_rect[i][0], g_rect[i][1], g_rect[i][2], g_rect[i][3]);
		}
		SelectObject(hdc, oldbrush);
		
		
		
		for (int i = 0; i < wonpress; ++i)
		{
			
			int idx = g_wonlist[i];
			SelectObject(hdc, wonbrush[i]);


			
			Ellipse(hdc, g_won[idx][0], g_won[idx][1], g_won[idx][2], g_won[idx][3]);
		}
		SelectObject(hdc, oldbrush);

		if (wonpen != nullptr)
		{
		int idx = g_wonlist[0];
		HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
		SelectObject(hdc, wonbrush[0]);
		Ellipse(hdc, g_won[idx][0], g_won[idx][1], g_won[idx][2], g_won[idx][3]);
		SelectObject(hdc, oldpen);
		}

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		DeleteObject(wonpen);
		DeleteObject(wonbrush);
		DeleteObject(boardbrush);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}