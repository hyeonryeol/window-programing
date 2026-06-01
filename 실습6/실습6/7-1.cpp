#include <windows.h>
#include <windowsx.h> 

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCzmdLine, int nCmdShow)
{
	WNDCLASS wc = {};
	HWND hWnd;
	WNDCLASSEX wc;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	wc.lpszClassName = L“ParentClass”;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClass(&wc);

	wc.hCursor = LoadCursor(NULL, IDC_HELP);
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wc.lpszClassName = L"ChildClass"; // 차일드 윈도우 클래스 이름
	wc.lpfnWndProc = ChildProc; // 차일드 윈도우 프로시저 지정
	RegisterClassEx(&wc); // 자식 윈도우 클래스를 등록
	hWnd = CreateWindow(L“ParentClass", NULL, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);

	HWND hWnd = CreateWindow(
		TEXT("MyWindow"), TEXT("제목"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
		NULL, NULL, hInstance, NULL
	);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CREATE:
		// 초기화
		return 0;

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		// 그리기 코드

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_KEYDOWN:

	{
		// 키 입력 처리
		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		// 마우스 클릭 처리
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);
		return 0;
	}

	case WM_DESTROY:
	{

		PostQuitMessage(0);
		return 0;
	}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}