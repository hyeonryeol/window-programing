#include <windows.h>
#include <string>
#include <vector>
#include <cmath>
#include <cwctype>
#include "resource.h"



HINSTANCE g_hInst;
HWND      g_hDlg;
bool g_fresh   = true;    // 결과 직후  다음 숫자 입력 시 새로 시작
bool g_binMode = false;   // 2진수 표시 상태

//에디트 박스 읽기/쓰기 
std::wstring GetDisp()
{
    wchar_t buf[256] = {};
    GetDlgItemText(g_hDlg, IDC_EDIT_DISP, buf, 256);
    return buf;
}
void SetDisp(const std::wstring& s)
{
    SetDlgItemText(g_hDlg, IDC_EDIT_DISP, s.c_str());
}

bool IsOp(wchar_t c) { return c == L'+' || c == L'-' || c == L'*' || c == L'/'; }
int  Prec(wchar_t c) { return (c == L'*' || c == L'/') ? 2 : 1; }

// --- 수식 계산 (+ - * / , 우선순위/실수 지원) ---
bool Eval(const std::wstring& s, double& out)
{
    std::vector<double>  nums;
    std::vector<wchar_t> ops;

    auto apply = [&]() -> bool {
        if (nums.size() < 2 || ops.empty()) return false;
        double b = nums.back(); nums.pop_back();
        double a = nums.back(); nums.pop_back();
        wchar_t op = ops.back(); ops.pop_back();
        double r = 0;
        switch (op) {
        case L'+': r = a + b; break;
        case L'-': r = a - b; break;
        case L'*': r = a * b; break;
        case L'/': if (b == 0) return false; r = a / b; break;
        default: return false;
        }
        nums.push_back(r);
        return true;
    };

    size_t i = 0;
    bool expectNum = true;
    while (i < s.size()) {
        wchar_t c = s[i];
        if (c == L' ') { i++; continue; }
        if (expectNum) {
            std::wstring num;
            if (c == L'+' || c == L'-') { num += c; i++; }   // 부호
            bool hasDigit = false, hasDot = false;
            while (i < s.size() && (iswdigit(s[i]) || (s[i] == L'.' && !hasDot))) {
                if (s[i] == L'.') hasDot = true; else hasDigit = true;
                num += s[i]; i++;
            }
            if (!hasDigit) return false;
            out = 0;
            nums.push_back(_wtof(num.c_str()));
            expectNum = false;
        } else {
            if (!IsOp(c)) return false;
            while (!ops.empty() && Prec(ops.back()) >= Prec(c))
                if (!apply()) return false;
            ops.push_back(c); i++; expectNum = true;
        }
    }
    if (expectNum) return false;           // 끝이 연산자
    while (!ops.empty()) if (!apply()) return false;
    if (nums.size() != 1) return false;
    out = nums[0];
    return true;
}

// --- 결과 문자열 포맷 (정수면 정수, 아니면 실수) ---
std::wstring FormatNum(double v)
{
    wchar_t buf[64];
    if (std::fabs(v - std::floor(v + 0.5)) < 1e-9 && std::fabs(v) < 9e15)
        swprintf(buf, 64, L"%lld", (long long)std::floor(v + 0.5));
    else
        swprintf(buf, 64, L"%g", v);
    return buf;
}

// --- 현재 표시값을 수로 (실패 시 false) ---
bool CurrentValue(double& v)
{
    return Eval(GetDisp(), v);
}

// --- 2진수 변환 ---
std::wstring ToBinary(long long v)
{
    if (v == 0) return L"0";
    bool neg = v < 0;
    unsigned long long u = neg ? (unsigned long long)(-v) : (unsigned long long)v;
    std::wstring s;
    while (u) { s += (u & 1) ? L'1' : L'0'; u >>= 1; }
    if (neg) s += L'-';
    std::wstring r(s.rbegin(), s.rend());
    return r;
}
long long FromBinary(const std::wstring& s)
{
    long long v = 0; bool neg = false; size_t i = 0;
    if (!s.empty() && s[0] == L'-') { neg = true; i = 1; }
    for (; i < s.size(); i++)
        if (s[i] == L'0' || s[i] == L'1') v = v * 2 + (s[i] - L'0');
    return neg ? -v : v;
}

// --- R : 각 숫자의 자릿수를 뒤집음 (12+34 -> 21+43) ---
std::wstring ReverseDigits(const std::wstring& s)
{
    std::wstring out, num;
    auto flush = [&]() {
        if (!num.empty()) { out.append(num.rbegin(), num.rend()); num.clear(); }
    };
    for (wchar_t c : s) {
        if (iswdigit(c)) num += c;
        else { flush(); out += c; }
    }
    flush();
    return out;
}

// --- CE : 마지막으로 입력한 숫자를 지움 (123+2+3 -> 123+2+) ---
std::wstring RemoveLastNumber(std::wstring s)
{
    while (!s.empty() && s.back() == L' ') s.pop_back();
    while (!s.empty() && (iswdigit(s.back()) || s.back() == L'.')) s.pop_back();
    return s;
}

// --- 숫자 버튼 ---
void OnDigit(wchar_t d)
{
    std::wstring t = GetDisp();
    if (g_fresh) { t.clear(); g_fresh = false; }
    if (t == L"0") t.clear();          // 앞자리 0 방지
    t += d;
    SetDisp(t);
}

// --- 소수점 버튼 ---
void OnDot()
{
    std::wstring t = GetDisp();
    if (g_fresh) { t.clear(); g_fresh = false; }
    size_t p = t.find_last_of(L"+-*/");
    std::wstring cur = (p == std::wstring::npos) ? t : t.substr(p + 1);
    if (cur.find(L'.') == std::wstring::npos) {
        if (cur.empty()) t += L'0';
        t += L'.';
        SetDisp(t);
    }
}

// --- 연산자 버튼 ---
void OnOperator(wchar_t op)
{
    std::wstring t = GetDisp();
    g_fresh = false;
    if (t.empty()) {
        if (op == L'-') SetDisp(L"-");  // 음수 시작 허용
        return;
    }
    if (IsOp(t.back())) t.back() = op;  // 연산자 중복이면 교체
    else                t += op;
    SetDisp(t);
}

// --- = 버튼 ---
void OnEquals()
{
    double v;
    if (CurrentValue(v)) { SetDisp(FormatNum(v)); g_fresh = true; }
    else                   SetDisp(L"Error");
}

// 단항 연산 공통 처리
void Unary(double (*fn)(double))
{
    double v;
    if (!CurrentValue(v)) { SetDisp(L"Error"); return; }
    SetDisp(FormatNum(fn(v)));
    g_fresh = true;
}

double f_half(double v) { return v / 2.0; }
double f_mul10(double v) { return v * 10.0; }
double f_square(double v) { return v * v; }

// --- 대화상자 프로시저 (모덜리스) ---
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
    case WM_INITDIALOG:
        SetDisp(L"");
        return TRUE;

    case WM_COMMAND:
    {
        WORD id = LOWORD(wParam);

        // 2진수 버튼이 아니면 2진수 표시 상태 해제
        if (id != IDC_BTN_BIN) g_binMode = false;

        switch (id)
        {
        case IDC_BTN_0: OnDigit(L'0'); break;
        case IDC_BTN_1: OnDigit(L'1'); break;
        case IDC_BTN_2: OnDigit(L'2'); break;
        case IDC_BTN_3: OnDigit(L'3'); break;
        case IDC_BTN_4: OnDigit(L'4'); break;
        case IDC_BTN_5: OnDigit(L'5'); break;
        case IDC_BTN_6: OnDigit(L'6'); break;
        case IDC_BTN_7: OnDigit(L'7'); break;
        case IDC_BTN_8: OnDigit(L'8'); break;
        case IDC_BTN_9: OnDigit(L'9'); break;
        case IDC_BTN_DOT: OnDot(); break;

        case IDC_BTN_ADD: OnOperator(L'+'); break;
        case IDC_BTN_SUB: OnOperator(L'-'); break;
        case IDC_BTN_MUL: OnOperator(L'*'); break;
        case IDC_BTN_DIV: OnOperator(L'/'); break;
        case IDC_BTN_EQ:  OnEquals(); break;

        case IDC_BTN_R:   SetDisp(ReverseDigits(GetDisp())); break;   // 자릿수 뒤집기
        case IDC_BTN_CE:  SetDisp(RemoveLastNumber(GetDisp())); break; // 마지막 숫자 삭제
        case IDC_BTN_C:   SetDisp(L""); g_fresh = true; break;         // 전체 삭제

        case IDC_BTN_BACK: {                                          // ← 한 자리 삭제
            std::wstring t = GetDisp();
            if (!t.empty()) t.pop_back();
            if (t.empty()) t = L"0";
            SetDisp(t);
            break;
        }

        case IDC_BTN_BIN: {                                          // 2진수 토글
            if (!g_binMode) {
                double v;
                if (CurrentValue(v)) {
                    SetDisp(ToBinary((long long)std::floor(v + 0.5)));
                    g_binMode = true; g_fresh = true;
                } else SetDisp(L"Error");
            } else {
                SetDisp(FormatNum((double)FromBinary(GetDisp())));
                g_binMode = false; g_fresh = true;
            }
            break;
        }

        case IDC_BTN_HALF:  Unary(f_half);   break;   // 1/2
        case IDC_BTN_MUL10: Unary(f_mul10);  break;   // *10
        case IDC_BTN_SQ:    Unary(f_square); break;   // x²
        case IDC_BTN_ROOT: {                          // √
            double v;
            if (!CurrentValue(v) || v < 0) SetDisp(L"Error");
            else { SetDisp(FormatNum(std::sqrt(v))); g_fresh = true; }
            break;
        }

        case IDC_BTN_FINISH:                          // 종료
        case IDCANCEL:
            DestroyWindow(hDlg);
            break;
        }
        return TRUE;
    }

    case WM_CLOSE:
        DestroyWindow(hDlg);
        return TRUE;

    case WM_DESTROY:
        PostQuitMessage(0);
        return TRUE;
    }
    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    g_hInst = hInstance;

    g_hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG3), NULL, DlgProc);
    ShowWindow(g_hDlg, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!IsDialogMessage(g_hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}
