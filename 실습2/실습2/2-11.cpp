#include <windows.h>
#include <cstdlib> // srand, rand
#include <ctime>   // time

// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 윈도우 클래스/제목 문자열
const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

enum ShapeType { SHAPE_C, SHAPE_S, SHAPE_P, SHAPE_E, SHAPE_s, SHAPE_a };
ShapeType slotShape[4] = { SHAPE_S, SHAPE_P, SHAPE_C, SHAPE_E }; // 위,오,아,왼

bool cHeld = false;
bool sHeld = false;
bool pHeld = false;
bool eHeld = false;
bool cMoved = false;   // C를 눌러 안쪽으로 옮겼는지
bool sMoved = false;
bool pMoved = false;
bool eMoved = false;
// 도형별 브러시(색 채우기용)
HBRUSH triBrush;
HBRUSH pentaBrush;
HBRUSH wonBrush;
HBRUSH pieBrush;

COLORREF piecolor;
COLORREF pentacolor;
COLORREF tricolor;
COLORREF woncolor;
COLORREF tempColor;     // 누를 때 임시 색

POINT point[10] = { {350,20}, {400,20}, {375,70}, {400, 120}, {350, 120}, {375,70}, {350,20} };
POINT Spoint[10] = { {325,225},{325,275},{375,250},{425,275},{425,225},{375,250},{325,225} };

POINT p[10] = { {600,200}, {640,230}, {620,280}, {570, 280}, {550, 230} };
POINT sp[5] = { {340,200}, {380,220}, {365,265}, {315,265}, {300,220} };

void RotateCCW(ShapeType a[4]) { // 반시계
	ShapeType t = a[0];
	a[0] = a[1]; a[1] = a[2]; a[2] = a[3]; a[3] = t;
}
void RotateCW(ShapeType a[4]) { //  시계
	ShapeType t = a[3];
	a[3] = a[2]; a[2] = a[1]; a[1] = a[0]; a[0] = t;
}
void SwapUD(ShapeType a[4]) {   //  위아래 교환
	ShapeType t = a[0];
	a[0] = a[2]; a[2] = t;
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
		tricolor = RGB(rand() % 256, rand() % 256, rand() % 256);
		triBrush = CreateSolidBrush(tricolor); //모래시계

		pentacolor = RGB(rand() % 256, rand() % 256, rand() % 256);
		pentaBrush = CreateSolidBrush(pentacolor);
		
		woncolor = RGB(rand() % 256, rand() % 256, rand() % 256);
		wonBrush = CreateSolidBrush(woncolor);

		piecolor = RGB(rand() % 256, rand() % 256, rand() % 256);
		pieBrush = CreateSolidBrush(piecolor);
		
		return 0;

	case WM_KEYDOWN:
		// 키 입력 처리
		if (wParam == 'C' && !cHeld)
		{
			cHeld = true;
			cMoved = true;
			
			sMoved = false;
			pMoved = false;
			eMoved = false;
			tempColor = RGB(rand() % 256, rand() % 256, rand() % 256);
			DeleteObject(wonBrush);
			wonBrush = CreateSolidBrush(tempColor);

			

			InvalidateRect(hWnd, NULL, TRUE); // 다시 그리기 요청
		}
		if (wParam == 'S' && !sHeld)
		{
			sHeld = true;
			sMoved = true;
			cMoved = false;
			
			pMoved = false;
			eMoved = false;
			tempColor = RGB(rand() % 256, rand() % 256, rand() % 256);
			DeleteObject(triBrush);
			triBrush = CreateSolidBrush(tempColor);

			InvalidateRect(hWnd, NULL, TRUE); // 다시 그리기 요청
		}
		if (wParam == 'P' && !pHeld)
		{
			pHeld = true;
			pMoved = true;

			cMoved = false;
			sMoved = false;
			
			eMoved = false;
			tempColor = RGB(rand() % 256, rand() % 256, rand() % 256);
			DeleteObject(pentaBrush);
			pentaBrush = CreateSolidBrush(tempColor);

			InvalidateRect(hWnd, NULL, TRUE); // 다시 그리기 요청
		}
		if (wParam == 'E' && !eHeld)
		{
			eHeld = true;
			eMoved = true;

			cMoved = false;
			sMoved = false;
			pMoved = false;
			
			tempColor = RGB(rand() % 256, rand() % 256, rand() % 256);
			DeleteObject(pieBrush);
			pieBrush = CreateSolidBrush(tempColor);

			InvalidateRect(hWnd, NULL, TRUE); // 다시 그리기 요청
		}
		if (wParam == 'Q')
		{
			PostQuitMessage(0);
		}
		if (wParam == VK_RIGHT) { RotateCCW(slotShape); InvalidateRect(hWnd, NULL, TRUE); }
		if (wParam == VK_LEFT) { RotateCW(slotShape);  InvalidateRect(hWnd, NULL, TRUE); }
		if (wParam == VK_UP) { SwapUD(slotShape);    InvalidateRect(hWnd, NULL, TRUE); }
		return 0;
	case WM_KEYUP:
		if (wParam == 'C')
		{
			cHeld = false;

			DeleteObject(wonBrush);
			wonBrush = CreateSolidBrush(woncolor);

			InvalidateRect(hWnd, NULL, TRUE); // 다시 그리기 요청
		}
		if (wParam == 'S')
		{
			sHeld = false;

			DeleteObject(triBrush);
			triBrush = CreateSolidBrush(tricolor);

			InvalidateRect(hWnd, NULL, TRUE); // 다시 그리기 요청
		}
		if (wParam == 'P')
		{
			pHeld = false;

			DeleteObject(pentaBrush);
			pentaBrush = CreateSolidBrush(pentacolor);

			InvalidateRect(hWnd, NULL, TRUE); // 다시 그리기 요청
		}
		if (wParam == 'E')
		{
			eHeld = false;

			DeleteObject(pieBrush);
			pieBrush = CreateSolidBrush(piecolor);

			InvalidateRect(hWnd, NULL, TRUE); // 다시 그리기 요청
		}
		
		return 0;
	case WM_CHAR:
		// 문자 입력 메시지 (현재 사용 안 함)
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		Rectangle(hdc, 250, 150, 500, 350);  // 가운데

		// 외곽 슬롯(위,오,아,왼)
		RECT slotR[4] = {
			{325,  20, 425, 120}, // 위
			{550, 200, 650, 300}, // 오
			{340, 440, 460, 560}, // 아
			{100, 200, 200, 300}  // 왼
		};

		auto DrawShapeInRect = [&](ShapeType st, const RECT& r)
			{
				HBRUSH brush = wonBrush;
				switch (st) {
				case SHAPE_C: brush = wonBrush;   break;
				case SHAPE_S: brush = triBrush;   break;
				case SHAPE_P: brush = pentaBrush; break;
				case SHAPE_s: brush = triBrush;   break;
				case SHAPE_E: brush = pieBrush;   break;
				case SHAPE_a: brush = pieBrush;   break;
				}

				HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

				int l = r.left, t = r.top, rr = r.right, bb = r.bottom;
				int cx = (l + rr) / 2;
				int cy = (t + bb) / 2;
				int w = rr - l;
				int h = bb - t;

				switch (st)
				{
				case SHAPE_C:
					Ellipse(hdc, l, t, rr, bb);
					break;

				case SHAPE_S:
				{
					POINT hourglass[7] = {
						{l, t}, {rr, t}, {cx, cy}, {rr, bb}, {l, bb}, {cx, cy}, {l, t}
					};
					Polygon(hdc, hourglass, 7);
					break;
				}
				case SHAPE_s:
				{
					Polygon(hdc, Spoint, 7);
					break;
				}

				case SHAPE_P:
				{
					POINT pent[5] = {
						{cx, t},
						{rr - w / 8, t + h / 3},
						{rr - w / 4, bb},
						{l + w / 4, bb},
						{l + w / 8, t + h / 3}
					};
					Polygon(hdc, pent, 5);
					break;
				}
				case SHAPE_a:
				{
					Pie(hdc, 320, 180, 430, 290, 430, 235, 375, 180);
					break;
				}
				case SHAPE_E:
				
					Pie(hdc, l, t, rr, bb, cx, t, rr, cy);
					break;
				}


				SelectObject(hdc, oldBrush);
			};

		// 회전 결과 반영해서 외곽 4개 그리기
		for (int i = 0; i < 4; ++i) {
			DrawShapeInRect(slotShape[i], slotR[i]);
		}

		// ===== 기존 커맨드 spec 유지: 중앙 이동 표시 =====
		RECT centerR = { 320, 180, 430, 290 };
		if (cMoved) DrawShapeInRect(SHAPE_C, centerR);
		if (sMoved) DrawShapeInRect(SHAPE_s, centerR);
		if (pMoved) DrawShapeInRect(SHAPE_P, centerR);
		if (eMoved) DrawShapeInRect(SHAPE_a, centerR);

		EndPaint(hWnd, &ps);
		return 0;
	}
	

	case WM_DESTROY:
		// GDI 리소스 해제 후 종료
		DeleteObject(triBrush);
		DeleteObject(pentaBrush);
		DeleteObject(wonBrush);
		DeleteObject(pieBrush);
		PostQuitMessage(0);
		return 0;
	}

	// 기본 메시지 처리
	return DefWindowProc(hWnd, msg, wParam, lParam);
}