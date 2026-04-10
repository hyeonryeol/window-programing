#include <windows.h>
#include <cstdlib> // srand, rand
#include <ctime>   // time

// 윈도우 프로시저 선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// 윈도우 클래스/제목 문자열
const wchar_t CLASS_NAME[] = L"MyWindowClass";
const wchar_t WINDOW_TITLE[] = L"Win32 Basic Template";

// 도형 타입
enum ShapeType {
	SHAPE_C,         // 원(외곽 기본)
	SHAPE_S,         // 모래시계(외곽 기본)
	SHAPE_P,         // 오각형(외곽 기본)
	SHAPE_E,         // 파이(외곽 기본)
	SHAPE_s,         // 중앙용 모래시계(기존 Spoint 사용)
	SHAPE_a,         // 중앙용 파이(기존 지정 좌표 사용)
	SHAPE_P_INV,     // 중앙용 뒤집힌 오각형
	SHAPE_C_ELLIPSE  // 중앙용 타원 (C 선택 시만 사용)
};

// 외곽 슬롯: 위,오,아,왼
ShapeType slotShape[4] = { SHAPE_S, SHAPE_P, SHAPE_C, SHAPE_E };

bool cHeld = false;
bool sHeld = false;
bool pHeld = false;
bool eHeld = false;
bool cMoved = false;   // C를 눌러 중앙으로 옮겼는지
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

POINT point[10] = { {350,20}, {400,20}, {375,70}, {400,120}, {350,120}, {375,70}, {350,20} };
POINT Spoint[10] = { {325,225},{325,275},{375,250},{425,275},{425,225},{375,250},{325,225} };

POINT p[10] = { {600,200}, {640,230}, {620,280}, {570,280}, {550,230} };
POINT sp[5] = { {340,200}, {380,220}, {365,265}, {315,265}, {300,220} };

// 회전/교환 함수
void RotateCCW(ShapeType a[4]) { // 반시계 (→)
	ShapeType t = a[0];
	a[0] = a[1]; a[1] = a[2]; a[2] = a[3]; a[3] = t;
}
void RotateCW(ShapeType a[4]) { // 시계 (←)
	ShapeType t = a[3];
	a[3] = a[2]; a[2] = a[1]; a[1] = a[0]; a[0] = t;
}
void SwapUD(ShapeType a[4]) {   // 위아래 교환 (↑)
	ShapeType t = a[0];
	a[0] = a[2]; a[2] = t;
}
void SwapLR(ShapeType a[4]) {   // 좌우 교환 (↓)
	ShapeType t = a[1];
	a[1] = a[3];
	a[3] = t;
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

	HWND hWnd = CreateWindowExW(
		0,
		CLASS_NAME,
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		srand((unsigned)time(NULL));

		tricolor = RGB(rand() % 256, rand() % 256, rand() % 256);
		triBrush = CreateSolidBrush(tricolor);

		pentacolor = RGB(rand() % 256, rand() % 256, rand() % 256);
		pentaBrush = CreateSolidBrush(pentacolor);

		woncolor = RGB(rand() % 256, rand() % 256, rand() % 256);
		wonBrush = CreateSolidBrush(woncolor);

		piecolor = RGB(rand() % 256, rand() % 256, rand() % 256);
		pieBrush = CreateSolidBrush(piecolor);

		return 0;

	case WM_KEYDOWN:
		if (wParam == 'C' && !cHeld)
		{
			cHeld = true; cMoved = true;
			sMoved = pMoved = eMoved = false;

			tempColor = RGB(rand() % 256, rand() % 256, rand() % 256);
			DeleteObject(wonBrush);
			wonBrush = CreateSolidBrush(tempColor);

			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == 'S' && !sHeld)
		{
			sHeld = true; sMoved = true;
			cMoved = pMoved = eMoved = false;

			tempColor = RGB(rand() % 256, rand() % 256, rand() % 256);
			DeleteObject(triBrush);
			triBrush = CreateSolidBrush(tempColor);

			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == 'P' && !pHeld)
		{
			pHeld = true; pMoved = true;
			cMoved = sMoved = eMoved = false;

			tempColor = RGB(rand() % 256, rand() % 256, rand() % 256);
			DeleteObject(pentaBrush);
			pentaBrush = CreateSolidBrush(tempColor);

			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == 'E' && !eHeld)
		{
			eHeld = true; eMoved = true;
			cMoved = sMoved = pMoved = false;

			tempColor = RGB(rand() % 256, rand() % 256, rand() % 256);
			DeleteObject(pieBrush);
			pieBrush = CreateSolidBrush(tempColor);

			InvalidateRect(hWnd, NULL, TRUE);
		}

		if (wParam == 'Q')
			PostQuitMessage(0);

		// 방향키 동작
		if (wParam == VK_LEFT) { RotateCCW(slotShape); InvalidateRect(hWnd, NULL, TRUE); } // → 반시계
		if (wParam == VK_RIGHT) { RotateCW(slotShape);  InvalidateRect(hWnd, NULL, TRUE); } // ← 시계
		if (wParam == VK_UP) { SwapUD(slotShape);    InvalidateRect(hWnd, NULL, TRUE); } // ↑ 위/아래
		if (wParam == VK_DOWN) { SwapLR(slotShape);    InvalidateRect(hWnd, NULL, TRUE); } // ↓ 좌/우

		return 0;

	case WM_KEYUP:
		if (wParam == 'C')
		{
			cHeld = false;
			DeleteObject(wonBrush);
			wonBrush = CreateSolidBrush(woncolor);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == 'S')
		{
			sHeld = false;
			DeleteObject(triBrush);
			triBrush = CreateSolidBrush(tricolor);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == 'P')
		{
			pHeld = false;
			DeleteObject(pentaBrush);
			pentaBrush = CreateSolidBrush(pentacolor);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == 'E')
		{
			eHeld = false;
			DeleteObject(pieBrush);
			pieBrush = CreateSolidBrush(piecolor);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		return 0;

	case WM_CHAR:
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		Rectangle(hdc, 250, 150, 500, 350);  // 가운데 박스

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
				case SHAPE_C:         brush = wonBrush;   break;
				case SHAPE_S:         brush = triBrush;   break;
				case SHAPE_P:         brush = pentaBrush; break;
				case SHAPE_s:         brush = triBrush;   break;
				case SHAPE_E:         brush = pieBrush;   break;
				case SHAPE_a:         brush = pieBrush;   break;
				case SHAPE_P_INV:     brush = pentaBrush; break;
				case SHAPE_C_ELLIPSE: brush = wonBrush;   break;
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
					// 기본 원
					Ellipse(hdc, l, t, rr, bb);
					break;

				case SHAPE_C_ELLIPSE:
					// 중앙 선택용 타원 (C 선택 시만 사용)
					Ellipse(hdc, l, t + h / 6, rr, bb - h / 6);
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
				case SHAPE_P_INV:
				{
					// 거꾸로 뒤집힌 오각형
					POINT pentInv[5] = {
						{cx, bb},
						{rr - w / 8, bb - h / 3},
						{rr - w / 4, t},
						{l + w / 4, t},
						{l + w / 8, bb - h / 3}
					};
					Polygon(hdc, pentInv, 5);
					break;
				}
				case SHAPE_a:
					// 중앙 전용 파이(기존 사용 좌표 유지)
					Pie(hdc, 320, 180, 430, 290, 430, 235, 375, 180);
					break;

				case SHAPE_E:
					Pie(hdc, l, t, rr, bb, cx, t, rr, cy);
					break;
				}

				SelectObject(hdc, oldBrush);
			};

		// 외곽 4개 그리기 (회전/교환 반영)
		for (int i = 0; i < 4; ++i) {
			DrawShapeInRect(slotShape[i], slotR[i]);
		}

		// 중앙 표시
		RECT centerR = { 320, 180, 430, 290 };
		if (cMoved) DrawShapeInRect(SHAPE_C_ELLIPSE, centerR); // C 선택 시만 타원
		if (sMoved) DrawShapeInRect(SHAPE_s, centerR);
		if (pMoved) DrawShapeInRect(SHAPE_P_INV, centerR);     // 뒤집힌 오각형
		if (eMoved) DrawShapeInRect(SHAPE_a, centerR);

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_DESTROY:
		DeleteObject(triBrush);
		DeleteObject(pentaBrush);
		DeleteObject(wonBrush);
		DeleteObject(pieBrush);
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}