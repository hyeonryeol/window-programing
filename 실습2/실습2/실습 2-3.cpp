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
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM IParam)
{
    PAINTSTRUCT ps;
    HDC hDC;

    RECT rect;
    int x = 0, y = 0;
    TCHAR lpOut[100];
    int n = rand() & 10;


    switch (iMessage) {
    case WM_PAINT:
    {
        
        hDC = BeginPaint(hWnd, &ps);

        RECT client;
        GetClientRect(hWnd, &client);

        TCHAR lpOut[100];
        if (n % 2 == 0)
        {
            // 1. 가로 분할 랜덤 
            int garo = rand() % 15 + 2;

            // 2. 세로 2등분
            int sero = 600 / 2;

            // 3. 셀 크기
            int cellWidth = 800 / garo; //한칸 너비
            int cellHeight = (sero - client.top) / 9; //한칸 높이

            int startDan = 2;
            int endDan = startDan + garo - 1;


            for (int c = 0; c < garo; ++c)
            {
                int dan = startDan + c;

                for (int i = 1; i <= 9; ++i)
                {
                    RECT r;

                    r.left = client.left + c * cellWidth; //각 칸 위치의 시작
                    r.right = r.left + cellWidth;

                    r.top = client.top + (i - 1) * cellHeight;
                    r.bottom = r.top + cellHeight;

                    wsprintf(lpOut, L"%d x %d = %d", dan, i, dan * i);
                    SetTextColor(hDC, RGB(255, 0, 0));
                    DrawText(hDC, lpOut, lstrlen(lpOut), &r,
                        DT_SINGLELINE | DT_CENTER | DT_VCENTER);
                }
            }


            for (int c = 0; c < garo; ++c)
            {
                int dan = endDan - c;

                for (int i = 1; i <= 9; ++i)
                {
                    RECT r;

                    r.left = client.left + c * cellWidth;
                    r.right = r.left + cellWidth;

                    r.top = sero + (i - 1) * cellHeight;
                    r.bottom = r.top + cellHeight;

                    wsprintf(lpOut, L"%d x %d = %d", dan, i, dan * i);

                    DrawText(hDC, lpOut, lstrlen(lpOut), &r,
                        DT_SINGLELINE | DT_CENTER | DT_VCENTER);
                }
            }

            // 등분 개수 출력
            RECT info = { 0, 0, client.right, 30 };
            wsprintf(lpOut, L"가로 등분: %d", garo);

            DrawText(hDC, lpOut, lstrlen(lpOut), &info,
                DT_SINGLELINE | DT_CENTER | DT_VCENTER);

            EndPaint(hWnd, &ps);
        break;
        }
        else
        {
            // 1. 가로 분할 랜덤 
            int garo = rand() % 15 + 2;

            // 2. 세로 2등분
            int sero = 600 / 2;

            // 3. 셀 크기
            int cellWidth = 800 / garo; //한칸 너비
            int cellHeight = (sero - client.top) / 9; //한칸 높이

            int startDan = 2;
            int endDan = startDan + garo - 1;


            for (int c = 0; c < garo; ++c)
            {
                int dan = startDan + c;

                for (int i = 1; i <= 9; ++i)
                {
                    RECT r;

                    r.left = client.left + c * cellWidth; //각 칸 위치의 시작
                    r.right = r.left + cellWidth;

                    r.top = client.top + (i - 1) * cellHeight;
                    r.bottom = r.top + cellHeight;

                    wsprintf(lpOut, L"%d x %d = %d", dan, i, dan * i);

                    DrawText(hDC, lpOut, lstrlen(lpOut), &r,
                        DT_SINGLELINE | DT_CENTER | DT_VCENTER);
                }
            }


            for (int c = 0; c < garo; ++c)
            {
                int dan = endDan - c;

                for (int i = 1; i <= 9; ++i)
                {
                    RECT r;

                    r.left = client.left + c * cellWidth;
                    r.right = r.left + cellWidth;

                    r.top = sero + (i - 1) * cellHeight;
                    r.bottom = r.top + cellHeight;

                    wsprintf(lpOut, L"%d x %d = %d", dan, i, dan * i);

                    DrawText(hDC, lpOut, lstrlen(lpOut), &r,
                        DT_SINGLELINE | DT_CENTER | DT_VCENTER);
                }
            }

            // 등분 개수 출력
            RECT info = { 0, 0, client.right, 30 };
            wsprintf(lpOut, L"가로 등분: %d", garo);

            DrawText(hDC, lpOut, lstrlen(lpOut), &info,
                DT_SINGLELINE | DT_CENTER | DT_VCENTER);

            EndPaint(hWnd, &ps);
            break;
        }
        }
    
       

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    }
    return (DefWindowProc(hWnd, iMessage, wParam, IParam));

}
