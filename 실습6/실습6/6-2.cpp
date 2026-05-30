#include <windows.h>
#include <windowsx.h>
#include "resource.h"

// 대화상자 프로시저
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
    case WM_INITDIALOG:
        // 컨트롤 초기화 (라디오 버튼 기본 선택, 에디트 초기값 등)
        return TRUE;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hDlg, &ps);
        // 그리기 코드
        EndPaint(hDlg, &ps);
        return TRUE;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        // case IDC_BUTTON1:
        //     break;

        case IDOK:
            EndDialog(hDlg, IDOK);
            return TRUE;

        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        return TRUE;

    case WM_CLOSE:
        EndDialog(hDlg, 0);
        return TRUE;
    }
    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
    return 0;
}
