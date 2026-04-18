#include <windows.h>
#include <cstdlib>
#include <ctime>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";
bool super = false;
int speed1 = 500;
HBRUSH rectbrush;
HBRUSH playerbrush;
HBRUSH itembrush;
HBRUSH tailbrush;
HBRUSH obbrush;
int prevPlayerX = 0;
int prevPlayerY = 0; //기차놀이용
int tailprevX[20] = {};
int tailprevY[20] = {};
int trainOrder[20] = {};  // 기차에 합류한 순서
bool won = true;
static int mx, my;
int g_ob[1600][4] = {}; //장애물
int obindex[1600] = {};
int obcount = 0;

int position = 0;
int ip[20];
int r[20];
int g[20];
int b[20];
int tailr[20];
int tailg[20];
int tailb[20];
int tailip[20];
int g_rect[1600][4];
int g_player[1600][4];
int g_tail[1600][4];
int itemcount = 20;
int movetailwon = 0;

int tailx[20] = {};
int taildx[20];
int taily[20] = {};
int taildy[20];
int tailxy[20];
// 플레이어 이동 방향 (처음엔 오른쪽)
int pdx = 1;  // x 방향: -1 왼쪽, 1 오른쪽, 0 정지
int pdy = 0;  // y 방향: -1 위, 1 아래, 0 정지
int rectcount[20] = {};
int rectdirect[20] = {};
int rectsize[20] = {};
int tailsize[20] = {};
int tailplus[20] = {};
bool jumping = false;
int jumpStep = 0;  // 0: 안점프, 1: 올라가는중, 2: 내려가는중
void makeob()
{
	int w = 20, h = 20;
	for (int i = 0; i < 40; ++i)
		for (int j = 0; j < 40; ++j)
		{
			int count = i * 40 + j;
			g_ob[count][0] = i * w + 30;
			g_ob[count][1] = j * h + 30;
			g_ob[count][2] = g_ob[count][0] + w;
			g_ob[count][3] = g_ob[count][1] + h;
		}
}
void makerectangle()
{
	int w = 20, h = 20;
	for (int i = 0; i < 40; ++i)
		for (int j = 0; j < 40; ++j)
		{
			int count = i * 40 + j;
			g_rect[count][0] = i * w + 30;
			g_rect[count][1] = j * h + 30;
			g_rect[count][2] = g_rect[count][0] + w;
			g_rect[count][3] = g_rect[count][1] + h;
		}
}
void player()
{
	int w = 20, h = 20;
	for (int i = 0; i < 40; ++i)
		for (int j = 0; j < 40; ++j)
		{
			int count = i * 40 + j;
			g_player[count][0] = i * w + 30;
			g_player[count][1] = j * h + 30;
			g_player[count][2] = g_player[count][0] + w;
			g_player[count][3] = g_player[count][1] + h;
		}
}
void tailwon()
{
	int w = 20, h = 20;
	for (int i = 0; i < 40; ++i)
		for (int j = 0; j < 40; ++j)
		{
			int count = i * 40 + j;
			g_tail[count][0] = i * w + 30;
			g_tail[count][1] = j * h + 30;
			g_tail[count][2] = g_tail[count][0] + w;
			g_tail[count][3] = g_tail[count][1] + h;
		}
}

void CheckItemCollision(HWND hWnd)
{
	taildx[movetailwon] = 1;
	taildy[movetailwon] = 1;
	tailxy[movetailwon] = rand() % 5 + 1;
	rectcount[movetailwon] = 0;
	rectdirect[movetailwon] = 0;
	rectsize[movetailwon] = 5;
	for (int i = 0; i < itemcount; ++i)
	{
		if (g_player[position][0] == g_rect[ip[i]][0] && g_player[position][1] == g_rect[ip[i]][1])
		{
			DeleteObject(playerbrush);
			playerbrush = CreateSolidBrush(RGB(r[i], g[i], b[i]));
			tailr[movetailwon] = r[i];
			tailg[movetailwon] = g[i];
			tailb[movetailwon] = b[i];
			int tx = rand() % 30 + 5;  // 5~34
			int ty = rand() % 30 + 5;  // 5~34
			tailip[movetailwon] = tx * 40 + ty;
			tailx[movetailwon] = 0;
			taildx[movetailwon] = 1;

			for (int j = i; j < itemcount; ++j)
			{
				ip[j] = ip[j + 1];
				r[j] = r[j + 1];
				g[j] = g[j + 1];
				b[j] = b[j + 1];
			}
			movetailwon++;
			itemcount--;
			i = -1;
		}
	}

}

// 플레이어 자동이동 타이머
void CALLBACK PlayerTimerProc(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime)
{
	for (int i = 0; i < movetailwon; ++i)
	{
		tailprevX[i] = g_tail[tailip[i]][0] + tailx[i];
		tailprevY[i] = g_tail[tailip[i]][1] + taily[i];
	}
	prevPlayerX = g_player[position][0];
	prevPlayerY = g_player[position][1];
	g_player[position][0] += pdx * 20;
	g_player[position][2] += pdx * 20;
	g_player[position][1] += pdy * 20;
	g_player[position][3] += pdy * 20;

	for (int i = 0; i < movetailwon; ++i)
	{

		if (tailxy[i] == 6 || super == true)
		{// 현재 기차 길이 계산
			int trainLength = 0;
			for (int j = 0; j < movetailwon; ++j)
				if (tailxy[j] == 6) trainLength++;


			// 꼬리 0은 플레이어 이전 위치 따라가기
			if (i == 0)
			{
				tailx[0] = prevPlayerX - g_tail[tailip[0]][0];
				taily[0] = prevPlayerY - g_tail[tailip[0]][1];
			}
			else
			{
				// 꼬리 i는 앞 꼬리(i-1)의 이전 위치 따라가기
				tailx[i] = tailprevX[i - 1] - g_tail[tailip[i]][0];
				taily[i] = tailprevY[i - 1] - g_tail[tailip[i]][1];
			}
		}
	}
	// 벽 통과
   // 기존 벽 통과 코드 삭제하고 이걸로 교체
	if (g_player[position][0] >= 830 && pdx == 1)  // 오른쪽 끝 도달
	{
		g_player[position][0] = 810;
		g_player[position][2] = 830;
		g_player[position][1] += 20;  // 한 칸 아래로
		g_player[position][3] += 20;
		pdx = -1;  // 왼쪽으로 방향 전환
	}
	else if (g_player[position][0] <= 10 && pdx == -1)  // 왼쪽 끝 도달
	{
		g_player[position][0] = 30;
		g_player[position][2] = 50;
		g_player[position][1] += 20;  // 한 칸 아래로
		g_player[position][3] += 20;
		pdx = 1;  // 오른쪽으로 방향 전환
	}
	// 아래 끝 도달하면 위로 올라가기
	if (g_player[position][1] >= 830)
	{
		g_player[position][0] = 30;
		g_player[position][2] = 50;
		g_player[position][1] = 30;
		g_player[position][3] = 50;
	}
	
	if (obcount > 0 && super == false)
	{
		for (int k = 0; k < obcount; ++k)
		{
			if (g_player[position][0] == g_rect[obindex[k]][2])
			{
				pdx = 0; pdy = -1;
			}
			if ((g_player[position][1] == g_rect[obindex[k]][3]))
			{
				pdx = -1; pdy = 0;
			}
			if ((g_player[position][2] == g_rect[obindex[k]][0]))
			{
				pdx = 0; pdy = 1;
			}
			if ((g_player[position][3] == g_rect[obindex[k]][1]))
			{
				pdx = 1; pdy = 0;
			}

		}
	}
	if (jumping)
	{
		if (jumpStep == 1)
		{
			g_player[position][1] -= 20;
			g_player[position][3] -= 20;
			jumpStep = 2;
		}
		else if (jumpStep == 2)
		{
			g_player[position][1] += 20;
			g_player[position][3] += 20;
			jumpStep = 0;
			jumping = false;
		}
	}
	if (super == true && g_player[position][0] <= 30 && g_player[position][1] <= 30)
	{
		speed1 = 500;
		super = false;
		KillTimer(hWnd, 1);
		SetTimer(hWnd, 1, speed1, (TIMERPROC)PlayerTimerProc);
	}
	CheckItemCollision(hWnd);
	InvalidateRect(hWnd, NULL, TRUE);
}

// 꼬리 자동이동 타이머
void CALLBACK TailTimerProc(HWND hWnd, UINT iMsg, UINT idEvent, DWORD dwTime)
{
	for (int i = 0; i < movetailwon; ++i)
	{
		bool notailCollision =
			g_tail[tailip[i + 1]][0] + tailx[i + 1] + tailsize[i + 1] > g_tail[tailip[i]][2] + tailx[i] + tailsize[i] ||  // 플레이어가 오른쪽
			g_tail[tailip[i + 1]][2] + tailx[i + 1] - tailsize[i + 1] < g_tail[tailip[i]][0] + tailx[i] - tailsize[i] ||  // 플레이어가 왼쪽
			g_tail[tailip[i + 1]][1] + taily[i + 1] + tailsize[i + 1]> g_tail[tailip[i]][3] + taily[i] + tailsize[i] ||  // 플레이어가 아래
			g_tail[tailip[i + 1]][3] + taily[i + 1] - tailsize[i + 1] < g_tail[tailip[i]][1] + taily[i] - tailsize[i];    // 플레이어가 위

		bool noCollision =
			g_player[position][0] > g_tail[tailip[i]][2] + tailx[i] + tailsize[i] ||  // 플레이어가 오른쪽
			g_player[position][2] < g_tail[tailip[i]][0] + tailx[i] - tailsize[i] ||  // 플레이어가 왼쪽
			g_player[position][1] > g_tail[tailip[i]][3] + taily[i] + tailsize[i] ||  // 플레이어가 아래
			g_player[position][3] < g_tail[tailip[i]][1] + taily[i] - tailsize[i];    // 플레이어가 위
		// 꼬리끼리 충돌 체크
		for (int i = 0; i < movetailwon; ++i)
		{
			for (int j = i + 1; j < movetailwon; ++j)
			{
				if (tailxy[i] == 6 || tailxy[j] == 6) continue;  // 이미 기차면 스킵

				bool notailCollision =
					g_tail[tailip[i]][0] + tailx[i] - tailsize[i] > g_tail[tailip[j]][2] + tailx[j] + tailsize[j] ||
					g_tail[tailip[i]][2] + tailx[i] + tailsize[i] < g_tail[tailip[j]][0] + tailx[j] - tailsize[j] ||
					g_tail[tailip[i]][1] + taily[i] - tailsize[i] > g_tail[tailip[j]][3] + taily[j] + tailsize[j] ||
					g_tail[tailip[i]][3] + taily[i] + tailsize[i] < g_tail[tailip[j]][1] + taily[j] - tailsize[j];

				if (!notailCollision)
				{
					// 랜덤으로 이동 방법 하나 선택 (1~5)
					int newxy = rand() % 5 + 1;
					tailxy[i] = newxy;
					tailxy[j] = newxy;
					// 색상/크기는 유지, 위치만 같은 방식으로 이동
				}
			}
		}
		// TailTimerProc의 충돌 감지 부분에 추가
		if (!noCollision && super == false)
		{
			// 합류 순서 계산 (한 번만!)
			int trainLength = 0;
			for (int j = 0; j < movetailwon; ++j)
				if (tailxy[j] == 6) trainLength++;
			trainOrder[i] = trainLength;
			tailsize[i] = -(trainOrder[i] * 3);
			tailxy[i] = 6;
			tailx[i] = prevPlayerX - g_tail[tailip[i]][0];
			taily[i] = prevPlayerY - g_tail[tailip[i]][1];

		}
		if (tailxy[i] == 1 && super == false)
		{
			tailx[i] += taildx[i] * 20;
			int curLeft = g_tail[tailip[i]][0] + tailx[i];
			int curRight = g_tail[tailip[i]][2] + tailx[i];
			if (curRight >= 830) { tailx[i] = 830 - g_tail[tailip[i]][2]; taildx[i] = -1; }
			if (curLeft <= 30) { tailx[i] = 30 - g_tail[tailip[i]][0]; taildx[i] = 1; }
		}
		else if (tailxy[i] == 2 && super == false)
		{
			taily[i] += taildy[i] * 20;
			int curTop = g_tail[tailip[i]][1] + taily[i];
			int curBottom = g_tail[tailip[i]][3] + taily[i];
			if (curBottom >= 810) { taily[i] = 810 - g_tail[tailip[i]][3]; taildy[i] = -1; }
			if (curTop <= 30) { taily[i] = 30 - g_tail[tailip[i]][1]; taildy[i] = 1; }
		}
		else if (tailxy[i] == 3 && super == false)
		{

		}
		else if (tailxy[i] == 4 && super == false)
		{
			// 방향별 dx, dy
			int ddx[] = { 1, 0, -1, 0 };
			int ddy[] = { 0, 1,  0,-1 };

			int d = rectdirect[i];
			tailx[i] += ddx[d] * 20;
			taily[i] += ddy[d] * 20;
			rectcount[i]++;


			if (rectcount[i] >= rectsize[i])
			{
				rectcount[i] = 0;
				rectdirect[i] = (rectdirect[i] + 1) % 4;  // 다음 방향으로
			}
		}
		else if (tailxy[i] == 5 && super == false)
		{


			if (tailplus[i] == 0)
			{
				tailsize[i] += 20;
				tailplus[i] = 1;
			}
			else if (tailplus[i] == 1)
			{
				tailsize[i] -= 20;
				tailplus[i] = 0;
			}
		}


	}

	InvalidateRect(hWnd, NULL, TRUE);
}

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

	HWND hWnd = CreateWindowExW(0, CLASS_NAME, WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1000, 1000, nullptr, nullptr, hInstance, nullptr);
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static BOOL Selection;

	switch (msg)
	{
	case WM_CREATE:
	{
		srand((unsigned)time(NULL));
		position = rand() % 1600;
		for (int i = 0; i < itemcount; ++i)
			ip[i] = rand() % 1600;
		player();
		makerectangle();
		tailwon();
		for (int i = 0; i < itemcount; ++i)
		{
			r[i] = rand() % 255;
			g[i] = rand() % 255;
			b[i] = rand() % 255;
		}
		rectbrush = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
		playerbrush = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
		obbrush = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
		makeob();
		
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		int MX = LOWORD(lParam);
		int MY = HIWORD(lParam);
		int playerCenterX = g_player[position][0] + 10;
		int playerCenterY = g_player[position][1] + 10;
		int dx = MX - playerCenterX;
		int dy = MY - playerCenterY;

		if (abs(dx) < 10 && abs(dy) < 10)
		{
			movetailwon = 0;
			won = !won;
		}
		// WM_LBUTTONDOWN의 꼬리 클릭 부분
		bool tailClicked = false;
		for (int i = 0; i < movetailwon; ++i)
		{
			if (tailxy[i] != 6) continue;

			int tailcenterx = g_tail[tailip[i]][0] + tailx[i] + 10;
			int tailcentery = g_tail[tailip[i]][1] + taily[i] + 10;
			int tx = MX - tailcenterx;
			int ty = MY - tailcentery;
			int radius = 20;
			if (abs(tx) < radius && abs(ty) < radius)
			{
				// 클릭된 꼬리 포함 모든 따라다니는 꼬리 분리
				for (int j = 0; j < movetailwon; ++j)
				{
					if (tailxy[j] == 6)
						tailxy[j] = rand() % 5 + 1;
				}
				tailClicked = true;
				break;
			}
		}
		return 0;
	}
	case WM_RBUTTONDOWN:
	{


		mx = LOWORD(lParam);
		my = HIWORD(lParam);
		//--- 마우스 좌표값 저장: (mx, my)
		Selection = TRUE;
		int col = (mx - 30) / 20;  // 몇 번째 열
		int row = (my - 30) / 20;  // 몇 번째 행
		int index = col * 40 + row;
		bool isitem = false;
		for (int i = 0; i < itemcount; ++i)
		{
			if (ip[i] == index)
			{
				isitem = true;
				break;
			}
		}

		// 빈 보드에만 출력
		if (!isitem && obcount < 1600)
		{
			if (obcount < 20)
			{
			obindex[obcount] = index;
			obcount++;
			InvalidateRect(hWnd, nullptr, TRUE);

			}
		}
		return 0;
	}
	case WM_KEYDOWN:
	{
		// 방향만 바꾸기
		if (wParam == VK_LEFT) { pdx = -1; pdy = 0; }
		if (wParam == VK_RIGHT) { pdx = 1; pdy = 0; }
		if (wParam == VK_UP) { pdx = 0; pdy = -1; }
		if (wParam == VK_DOWN) { pdx = 0; pdy = 1; }
		if (wParam == 'S')
		{
		
			SetTimer(hWnd, 1, speed1, (TIMERPROC)PlayerTimerProc);  // 플레이어 자동이동
			SetTimer(hWnd, 2, 800, (TIMERPROC)TailTimerProc);    // 꼬리 자동이동
		}
		if (wParam == 'Q')
		{
			PostQuitMessage(0);
		}
		if (wParam == VK_OEM_PLUS)
		{
			if (speed1 < 100)
			{
			speed1 -= 100;
			}

			KillTimer(hWnd, 1);
			SetTimer(hWnd, 1, speed1, (TIMERPROC)PlayerTimerProc);
		}
		if (wParam == VK_OEM_MINUS)
		{
			speed1 += 100;
			KillTimer(hWnd, 1);
			SetTimer(hWnd, 1, speed1, (TIMERPROC)PlayerTimerProc);
		}
		if (wParam == 'J')
		{
			jumping = true;
			jumpStep = 1;
			

		}
		if (wParam == 'A')
		{
			if (super == false)
			{
			super = true;
			speed1 = 50;
			KillTimer(hWnd, 1);
			SetTimer(hWnd, 1, speed1, (TIMERPROC)PlayerTimerProc);

			}
			else 
			{
				if (super == true)
				{
					speed1 = 500;
					super = false;
					KillTimer(hWnd, 1);
					SetTimer(hWnd, 1, speed1, (TIMERPROC)PlayerTimerProc);
					
				}

			}
			
		}
		if (wParam == 'T' && movetailwon > 0)
		{
			// 플레이어 색 임시 저장
			int tempR = tailr[0];
			int tempG = tailg[0];
			int tempB = tailb[0];

			// 첫 번째 꼬리 색 → 플레이어 색으로
			DeleteObject(playerbrush);
			playerbrush = CreateSolidBrush(RGB(tempR, tempG, tempB));

			// 플레이어 색 → 마지막 꼬리 색으로
			tailr[movetailwon - 1] = tailr[0];
			tailg[movetailwon - 1] = tailg[0];
			tailb[movetailwon - 1] = tailb[0];

			// 첫 번째 꼬리 색을 플레이어 원래 색으로
			// (playerbrush의 RGB를 알 방법이 없으니 별도 변수 필요)
		}
			InvalidateRect(hWnd, nullptr, TRUE);
		return 0;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		HBRUSH oldbrush = (HBRUSH)SelectObject(hdc, rectbrush);
		for (int i = 0; i < 1600; ++i)
			Rectangle(hdc, g_rect[i][0], g_rect[i][1], g_rect[i][2], g_rect[i][3]);
		SelectObject(hdc, oldbrush);
		if (won == true)
		{
		oldbrush = (HBRUSH)SelectObject(hdc, playerbrush);
		Ellipse(hdc, g_player[position][0], g_player[position][1], g_player[position][2], g_player[position][3]);
		}
		else if (won == false)
		{
			// WM_PAINT의 else if (won == false) 부분
			POINT tri[3] = {
				{ g_player[position][0] + 10, g_player[position][1] },      // 위 꼭짓점
				{ g_player[position][0],      g_player[position][3] },      // 왼쪽 아래
				{ g_player[position][2],      g_player[position][3] }       // 오른쪽 아래
			};
			Polygon(hdc, tri, 3);
		}
		SelectObject(hdc, oldbrush);
		for (int i = 0; i < itemcount; ++i)
		{
			itembrush = (HBRUSH)CreateSolidBrush(RGB(r[i], g[i], b[i]));
			oldbrush = (HBRUSH)SelectObject(hdc, itembrush);
			Rectangle(hdc, g_rect[ip[i]][0] + 5, g_rect[ip[i]][1] + 5, g_rect[ip[i]][0] + 15, g_rect[ip[i]][1] + 15);
			DeleteObject(itembrush);
		}
		SelectObject(hdc, oldbrush);

		for (int i = 0; i < movetailwon; ++i)
		{
			tailbrush = (HBRUSH)CreateSolidBrush(RGB(tailr[i], tailg[i], tailb[i]));
			oldbrush = (HBRUSH)SelectObject(hdc, tailbrush);
			Ellipse(hdc,
				g_tail[tailip[i]][0] + tailx[i] - tailsize[i], g_tail[tailip[i]][1] + taily[i] - tailsize[i],
				g_tail[tailip[i]][2] + tailx[i] + tailsize[i], g_tail[tailip[i]][3] + taily[i] + tailsize[i]);
			DeleteObject(tailbrush);
		}
		SelectObject(hdc, oldbrush);
		oldbrush = (HBRUSH)SelectObject(hdc, obbrush);

		for (int i = 0; i < obcount; ++i)
		{
			Rectangle(hdc, g_rect[obindex[i]][0], g_rect[obindex[i]][1],
				g_rect[obindex[i]][2], g_rect[obindex[i]][3]);
		}

		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_DESTROY:
		DeleteObject(playerbrush);
		DeleteObject(rectbrush);
		DeleteObject(obbrush);
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}