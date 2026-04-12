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
bool won1 = false;
bool won2 = false;
bool won3 = false;
bool won4 = false;
bool won5 = false;
bool won6 = false;
bool won7 = false;
bool won8 = false;
bool won9 = false;
bool won10 = false;

int g_wonlist[10];
int g_selected = -1;
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
			if (won1 == false)
			{
				g_wonlist[0];
				wonpen = CreatePen(PS_SOLID, 3, (RGB(255, 0, 0)));
				g_selected = 0;
				won1 = true;
			}
			else
			{
				won1 = false;
				g_selected = -1;
			}
			InvalidateRect(hWnd, nullptr, TRUE);


		}
		if (wParam == '2')
		{
			if (won2 == false)
			{
				g_wonlist[1];
				wonpen = CreatePen(PS_SOLID, 3, (RGB(255, 0, 0)));
				g_selected = 1;
				won2 = true;

			}
			else
			{
				won2 = false;
				g_selected = -1;
			}
			InvalidateRect(hWnd, nullptr, TRUE);
		}
		if (wParam == '3')
		{
			if (won3 == false)
			{
				g_wonlist[2];
				wonpen = CreatePen(PS_SOLID, 3, (RGB(255, 0, 0)));
				g_selected = 2;
				won3 = true;

			}
			else
			{
				won3 = false;
				g_selected = -1;
			}
			InvalidateRect(hWnd, nullptr, TRUE);
		}
		if (wParam == '4')
		{
			if (won4 == false)
			{
				g_wonlist[3];
				wonpen = CreatePen(PS_SOLID, 3, (RGB(255, 0, 0)));
				g_selected = 3;
				won4 = true;

			}
			else
			{
				won4 = false;
				g_selected = -1;
			}
			InvalidateRect(hWnd, nullptr, TRUE);
		}
		if (wParam == '5')
		{
			if (won5 == false)
			{
				g_wonlist[4];
				wonpen = CreatePen(PS_SOLID, 3, (RGB(255, 0, 0)));
				g_selected = 4;
				won5 = true;

			}
			else
			{
				won5 = false;
				g_selected = -1;
			}
			InvalidateRect(hWnd, nullptr, TRUE);
		}
		if (wParam == '6')
		{
			if (won6 == false)
			{
				g_wonlist[5];
				wonpen = CreatePen(PS_SOLID, 3, (RGB(255, 0, 0)));
				g_selected = 5;
				won6 = true;

			}
			else
			{
				won6 = false;
				g_selected = -1;
			}
			InvalidateRect(hWnd, nullptr, TRUE);
		}
		if (wParam == '7')
		{
			if (won7 == false)
			{
				g_wonlist[6];
				wonpen = CreatePen(PS_SOLID, 3, (RGB(255, 0, 0)));
				g_selected = 6;
				won7 = true;

			}
			else
			{
				won7 = false;
				g_selected = -1;
			}
			InvalidateRect(hWnd, nullptr, TRUE);
		}
		if (wParam == '8')
		{
			if (won8 == false)
			{
				g_wonlist[7];
				wonpen = CreatePen(PS_SOLID, 3, (RGB(255, 0, 0)));
				g_selected = 7;
				won8 = true;

			}
			else
			{
				won8 = false;
				g_selected = -1;
			}
			InvalidateRect(hWnd, nullptr, TRUE);
		}
		if (wParam == '9')
		{
			if (won9 == false)
			{
				g_wonlist[8];
				wonpen = CreatePen(PS_SOLID, 3, (RGB(255, 0, 0)));
				g_selected = 8;
				won9 = true;

			}
			else
			{
				won9 = false;
				g_selected = -1;
			}
			InvalidateRect(hWnd, nullptr, TRUE);
		}
		if (wParam == '0')
		{
			if (won10 == false)
			{
				g_wonlist[9];
				wonpen = CreatePen(PS_SOLID, 3, (RGB(255, 0, 0)));
				g_selected = 9;
				won10 = true;

			}
			else
			{
				won10 = false;
				g_selected = -1;
			}
			InvalidateRect(hWnd, nullptr, TRUE);
		}

		if (wParam == VK_RIGHT && (won1 == true || won2 == true || won3 == true || won4 == true || won5 == true || won6 == true || won7 == true || won8 == true || won9 == true || won10 == (true)))
		{
			if (g_selected == 0 || 1 || 2 || 3 || 4 || 5 || 6 || 7 || 8 || 9)
			{
				int idx = g_wonlist[g_selected];
				g_won[idx][0] += 30;
				g_won[idx][2] += 30;
				InvalidateRect(hWnd, nullptr, TRUE);
			}

		}
		if (wParam == VK_LEFT && (won1 == true || won2 == true || won3 == true || won4 == true || won5 == true || won6 == true || won7 == true || won8 == true || won9 == true || won10 == (true)))
		{
			if (g_selected == 0 || 1 || 2 || 3 || 4 || 5 || 6 || 7 || 8 || 9)
			{
				int idx = g_wonlist[g_selected];
				g_won[idx][0] -= 30;
				g_won[idx][2] -= 30;
				InvalidateRect(hWnd, nullptr, TRUE);
			}
		}
		if (wParam == VK_UP && (won1 == true || won2 == true || won3 == true || won4 == true || won5 == true || won6 == true || won7 == true || won8 == true || won9 == true || won10 == (true)))
		{
			if (g_selected == 0 || 1 || 2 || 3 || 4 || 5 || 6 || 7 || 8 || 9)
			{
				int idx = g_wonlist[g_selected];
				g_won[idx][1] -= 30;
				g_won[idx][3] -= 30;
				InvalidateRect(hWnd, nullptr, TRUE);
			}
		}
		if (wParam == VK_DOWN && (won1 == true || won2 == true || won3 == true || won4 == true || won5 == true || won6 == true || won7 == true || won8 == true || won9 == true || won10 == (true)))
		{
			if (g_selected == 0 || 1 || 2 || 3 || 4 || 5 || 6 || 7 || 8 || 9)
			{
				int idx = g_wonlist[g_selected];
				g_won[idx][1] += 30;
				g_won[idx][3] += 30;
				InvalidateRect(hWnd, nullptr, TRUE);
			}
		}
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

		if (won1 == true)
		{
			int idx = g_wonlist[0];
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			SelectObject(hdc, wonbrush[0]);
			Ellipse(hdc, g_won[idx][0], g_won[idx][1], g_won[idx][2], g_won[idx][3]);
			SelectObject(hdc, oldpen);
		}
		if (won2 == true)
		{
			int idx = g_wonlist[1];
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			SelectObject(hdc, wonbrush[1]);
			Ellipse(hdc, g_won[idx][0], g_won[idx][1], g_won[idx][2], g_won[idx][3]);
			SelectObject(hdc, oldpen);
		}
		if (won3 == true)
		{
			int idx = g_wonlist[2];
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			SelectObject(hdc, wonbrush[2]);
			Ellipse(hdc, g_won[idx][0], g_won[idx][1], g_won[idx][2], g_won[idx][3]);
			SelectObject(hdc, oldpen);
		}
		if (won4 == true)
		{
			int idx = g_wonlist[3];
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			SelectObject(hdc, wonbrush[3]);
			Ellipse(hdc, g_won[idx][0], g_won[idx][1], g_won[idx][2], g_won[idx][3]);
			SelectObject(hdc, oldpen);
		}
		if (won5 == true)
		{
			int idx = g_wonlist[4];
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			SelectObject(hdc, wonbrush[4]);
			Ellipse(hdc, g_won[idx][0], g_won[idx][1], g_won[idx][2], g_won[idx][3]);
			SelectObject(hdc, oldpen);
		}
		if (won6 == true)
		{
			int idx = g_wonlist[5];
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			SelectObject(hdc, wonbrush[5]);
			Ellipse(hdc, g_won[idx][0], g_won[idx][1], g_won[idx][2], g_won[idx][3]);
			SelectObject(hdc, oldpen);
		}
		if (won7 == true)
		{
			int idx = g_wonlist[6];
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			SelectObject(hdc, wonbrush[6]);
			Ellipse(hdc, g_won[idx][0], g_won[idx][1], g_won[idx][2], g_won[idx][3]);
			SelectObject(hdc, oldpen);
		}
		if (won8 == true)
		{
			int idx = g_wonlist[7];
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			SelectObject(hdc, wonbrush[7]);
			Ellipse(hdc, g_won[idx][0], g_won[idx][1], g_won[idx][2], g_won[idx][3]);
			SelectObject(hdc, oldpen);
		}
		if (won9 == true)
		{
			int idx = g_wonlist[8];
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			SelectObject(hdc, wonbrush[8]);
			Ellipse(hdc, g_won[idx][0], g_won[idx][1], g_won[idx][2], g_won[idx][3]);
			SelectObject(hdc, oldpen);
		}
		if (won10 == true)
		{
			int idx = g_wonlist[9];
			HPEN oldpen = (HPEN)SelectObject(hdc, wonpen);
			SelectObject(hdc, wonbrush[9]);
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