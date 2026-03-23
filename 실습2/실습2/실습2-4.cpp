#include <windows.h> //윈도우 좌표값, 창크기, 색, 이름
#include <tchar.h>
#include<time.h>
#include<stdlib.h>

HINSTANCE g_hlnst;
LPCTSTR IpszClass = L"Window Class Name";
LPCTSTR IpszWindowName = L"windows program 3";

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessgae, WPARAM wParam, LPARAM IParam);

int WINAPI WinMain(HINSTANCE hlnstance, HINSTANCE hPrevlnstance, LPSTR IpszCmdParam, int nCmdShow)
{
    srand(time(NULL)); // 랜덤 시드 초기화
    
    HWND hWND;
    MSG Message;
    WNDCLASSEX WndClass;
    g_hlnst = hlnstance;

    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc = (WNDPROC)WndProc;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = hlnstance;
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClass.lpszMenuName = NULL;
    WndClass.lpszClassName = IpszClass;
    WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&WndClass);

    srand(time(NULL)); // 랜덤 시드 초기화
    hWND = CreateWindow(IpszClass, IpszWindowName, WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_HSCROLL | WS_VSCROLL | WS_MAXIMIZEBOX | WS_MINIMIZEBOX, 100, 100, 800, 600, NULL, (HMENU)NULL, hlnstance, NULL);

    ShowWindow(hWND, nCmdShow);
    UpdateWindow(hWND);

    while (GetMessage(&Message, 0, 0, 0))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);

    }
    return Message.wParam;
}
COLORREF GetColor(int index)
{
    switch (index % 5)
    {
    case 0: return RGB(255, 0, 0);   // 빨강
    case 1: return RGB(0, 255, 0);   // 초록
    case 2: return RGB(0, 0, 255);   // 파랑
    case 3: return RGB(255, 0, 255); // 보라
    case 4: return RGB(255, 165, 0); // 주황
    }
    return RGB(0, 0, 0);
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
    PAINTSTRUCT ps;
    HDC hDC;
    bool paint = false;
    RECT rect;
    int x = 0, y = 0;
    TCHAR lpOut[100];
    
    
    bool _danChosen = false;
    int _chosenDan = 2;
    
    if (!_danChosen) { _chosenDan = rand() % 8 + 2; _danChosen = true; }
    int garo = _chosenDan;
    if (paint == false)
    {
        int n = rand() % 10;
        int z = rand() % _chosenDan;

        switch (iMessage) {
        case WM_PAINT:
        {
            hDC = BeginPaint(hWnd, &ps);

            RECT client;
            GetClientRect(hWnd, &client);



            RECT header = { client.left, client.top, client.right, client.top };

            wsprintf(lpOut, L"선택 단: %d", _chosenDan);
            SetTextColor(hDC, RGB(0, 0, 0));
            DrawText(hDC, lpOut, lstrlen(lpOut), &header, DT_SINGLELINE | DT_CENTER | DT_VCENTER);


            int areaTop = client.top;
            int areaHeight = client.bottom;
            int cellHeight = areaHeight / 9;
            int cellWidth = 400 / garo;
            int width = 200; // 텍스트 박스 너비

            for (int i = 1; i <= 9; ++i)
            {
                RECT r;

                r.left = (cellWidth * z) + client.left + (i * 10);
                r.right = r.left + width; 

                r.top = areaTop + (i - 1) * cellHeight;
                r.bottom = r.top + cellHeight;

                COLORREF color = GetColor(rand() % 5);
                SetTextColor(hDC, color);

                wsprintf(lpOut, L"%d x %d = %d", _chosenDan, i, _chosenDan * i);

                DrawText(hDC, lpOut, lstrlen(lpOut), &r, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
            }

            EndPaint(hWnd, &ps);
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        }
        return (DefWindowProc(hWnd, iMessage, wParam, IParam));
    }
}
