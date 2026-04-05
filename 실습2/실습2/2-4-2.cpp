#include <windows.h> // 윈도우 API 기본 헤더 (창 생성, 메시지 처리 등)
#include <tchar.h>   // TCHAR, wsprintf 등 문자열 처리
#include <time.h>    // time() - 랜덤 시드용
#include <stdlib.h>  // rand(), srand() - 랜덤 함수

HINSTANCE g_hInst;                          // 프로그램 인스턴스 저장
LPCTSTR lpszClass = L"WindowClassName"; // 윈도우 클래스 이름
LPCTSTR lpszWindowName = L"랜덤사각형그리기"; // 창 제목

// ── 전역 변수 ─────────────────────────────────────────────────
int    rectX, rectY;   // 사각형 위치 (랜덤으로 결정)
int    rectW, rectH;   // 사각형 크기 (랜덤으로 결정)
TCHAR  garoStr[100];   // 가로선에 출력할 문자열
TCHAR  seroStr[100];   // 세로선에 출력할 문자열
int    garoLen;        // 가로 문자열 길이 (사각형 너비 기준)
int    seroLen;        // 세로 문자열 길이 (사각형 높이 기준)
COLORREF garoColor;    // 가로선 색상 (랜덤)
COLORREF seroColor;    // 세로선 색상 (랜덤)

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

// ── 랜덤 알파벳 문자열 생성 ───────────────────────────────────
// str: 저장할 배열, len: 글자 수
void makeRandStr(TCHAR* str, int len)
{
	for (int i = 0; i < len; i++)
		str[i] = L'A' + rand() % 26; // 'A'~'Z' 중 랜덤 글자
	str[len] = L'\0';                 // 문자열 끝 표시
}

// ── 랜덤 색상 생성 ────────────────────────────────────────────
COLORREF randColor()
{
	return RGB(rand() % 256, rand() % 256, rand() % 256); // R, G, B 각각 0~255
}

// ── 사각형/문자열 값 초기화 ───────────────────────────────────
// 창이 처음 만들어지거나 크기가 바뀔 때마다 호출
void initValues(HWND hWnd)
{
	RECT client;
	GetClientRect(hWnd, &client); // 창 크기 가져오기
	int gw = client.right;        // 창 너비
	int sh = client.bottom;       // 창 높이

	// 창이 너무 작으면 그리지 않음
	if (gw < 220 || sh < 220)
	{
		rectW = rectH = 0;
		return;
	}

	// 사각형 크기: 100 ~ 창너비/2 사이 랜덤
	rectW = 100 + rand() % (gw / 2 - 100);
	rectH = 100 + rand() % (sh / 2 - 100);

	// 사각형 위치: 1 ~ 창너비/2 사이 랜덤
	rectX = 1 + rand() % (gw / 2);
	rectY = 1 + rand() % (sh / 2);

	// 가로 문자열 길이 결정 (글자 하나 ≈ 8픽셀 기준)
	garoLen = rectW / 8;
	if (garoLen < 1)  garoLen = 1;  // 최소 1글자
	if (garoLen > 99) garoLen = 99; // 배열 크기 초과 방지
	makeRandStr(garoStr, garoLen);  // 랜덤 문자열 생성

	// 세로 문자열 길이 결정 (글자 하나 ≈ 16픽셀 기준)
	seroLen = rectH / 16;
	if (seroLen < 1)  seroLen = 1;
	if (seroLen > 99) seroLen = 99;
	makeRandStr(seroStr, seroLen);

	garoColor = randColor(); // 가로선 색상 랜덤
	seroColor = randColor(); // 세로선 색상 랜덤
}

// ── 프로그램 시작점 ───────────────────────────────────────────
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	srand(time(NULL)); // 랜덤 시드 초기화 (매번 다른 랜덤값)

	HWND hWnd;           // 창 핸들
	MSG Message;         // 메시지 구조체
	WNDCLASSEX WndClass; // 윈도우 설계도
	g_hInst = hInstance;

	// ── 윈도우 설계도 작성 ───────────────────────────────────
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;        // 크기 바뀌면 다시 그리기
	WndClass.lpfnWndProc = WndProc;                        // 이벤트 처리 함수 지정
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); // 기본 아이콘
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);     // 기본 화살표 커서
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // 배경 흰색
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass); // 설계도 등록

	// ── 실제 창 생성 ─────────────────────────────────────────
	hWnd = CreateWindow(
		lpszClass, lpszWindowName,
		WS_OVERLAPPEDWINDOW,
		100, 100, // 창 시작 위치
		800, 600, // 창 크기
		NULL, NULL, hInstance, NULL
	);

	ShowWindow(hWnd, nCmdShow); // 창 보여주기
	UpdateWindow(hWnd);         // 창 즉시 그리기

	// ── 메시지 루프 (프로그램이 살아있는 동안 반복) ──────────
	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message); // 키 입력 → WM_CHAR 변환
		DispatchMessage(&Message);  // WndProc으로 메시지 전달
	}
	return Message.wParam;
}

// ── 이벤트 처리 함수 ──────────────────────────────────────────
// 키보드, 마우스, 화면 그리기 등 모든 이벤트를 처리
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;

	switch (iMessage)
	{
	case WM_CREATE: // 창이 처음 만들어질 때 한 번 호출
		initValues(hWnd); // 랜덤값 초기화
		break;

	case WM_SIZE: // 창 크기가 바뀔 때 호출
		initValues(hWnd);              // 새로운 랜덤값으로 초기화
		InvalidateRect(hWnd, NULL, TRUE); // 화면 다시 그리기 요청 → WM_PAINT 발생
		break;

	case WM_PAINT: // 화면을 그릴 때 호출
	{
		// 사각형 크기가 0 이하면 그리지 않음 (창이 너무 작을 때)
		if (rectW <= 0 || rectH <= 0)
		{
			EndPaint(hWnd, &ps);
			return 0;
		}
		if (rectW > 101 && rectH > 101)
		{
			hDC = BeginPaint(hWnd, &ps); // 그리기 시작
			SetBkMode(hDC, TRANSPARENT); // 글자 배경 투명하게

			// 글자 높이 측정
			TEXTMETRIC tm;
			GetTextMetrics(hDC, &tm);
			int charH = tm.tmHeight; // 글자 높이 (세로선 간격용)

			// 가로 문자열 실제 너비 측정
			SIZE sz;
			GetTextExtentPoint32(hDC, garoStr, garoLen, &sz);
			int strW = sz.cx;          // 가로 문자열 전체 너비
			int charW = strW / garoLen; // 글자 하나 너비

			// 위쪽 가로선 출력
			SetTextColor(hDC, garoColor);
			TextOut(hDC, rectX, rectY, garoStr, garoLen);

			// 아래쪽 가로선 출력 (rectH만큼 아래)
			TextOut(hDC, rectX, rectY + rectH, garoStr, garoLen);

			// 왼쪽 세로선 출력 (한 글자씩 아래로)
			SetTextColor(hDC, seroColor);
			for (int i = 0; i < seroLen; i++)
			{
				TCHAR ch[2] = { seroStr[i], L'\0' }; // 글자 하나씩
				TextOut(hDC, rectX, rectY + i * charH, ch, 1);
			}

			// 오른쪽 세로선 출력 (가로선 마지막 글자 위치에 맞춤)
			int rightX = rectX + strW - charW; // 마지막 글자 시작 x위치
			for (int i = 0; i < seroLen; i++)
			{
				TCHAR ch[2] = { seroStr[i], L'\0' };
				TextOut(hDC, rightX, rectY + i * charH, ch, 1);
			}

			EndPaint(hWnd, &ps); // 그리기 끝
			return 0;
		}
	}

	case WM_DESTROY: // 창 닫힐 때 호출
		PostQuitMessage(0); // 메시지 루프 종료 → 프로그램 종료
		return 0;
	}

	return DefWindowProc(hWnd, iMessage, wParam, lParam); // 기본 이벤트 처리
}