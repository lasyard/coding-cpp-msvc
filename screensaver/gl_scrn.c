#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tchar.h>

#undef UNICODE
#include <scrnsave.h>

#include "resource.h"

#include "gl_draw.h"

#define MAX_SPEED 10
#define MIN_SPEED 1

static const TCHAR REG_MAIN_KEY[] = _T("SOFTWARE\\Lasyard\\LasyardScrn");
static const TCHAR REG_SPEED_KEY[] = _T("Spinning Speed");

static HFONT g_hFont = NULL;

static HGLRC g_hRC;

static LONG loadParasReg(int *speed)
{
    LONG r;
    HKEY hKey;
    DWORD type;
    DWORD value;
    DWORD len;
    *speed = 1;
    r = RegOpenKeyEx(HKEY_CURRENT_USER, REG_MAIN_KEY, 0, KEY_READ, &hKey);
    if (r != ERROR_SUCCESS) {
        return r;
    }
    len = sizeof(DWORD);
    r = RegQueryValueEx(hKey, REG_SPEED_KEY, 0, &type, (BYTE *)&value, &len);
    if (r == ERROR_SUCCESS) {
        *speed = (int)value;
    }
    RegCloseKey(hKey);
    return r;
}

static LONG saveParasReg(int speed)
{
    LONG r;
    HKEY hKey;
    DWORD value;
    r = RegCreateKeyEx(
        HKEY_CURRENT_USER,
        REG_MAIN_KEY,
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE,
        NULL,
        &hKey,
        &value
    );
    if (r != ERROR_SUCCESS) {
        return r;
    }
    r = RegSetValueEx(hKey, REG_SPEED_KEY, 0, REG_DWORD, (BYTE *)&speed, sizeof(DWORD));
    RegCloseKey(hKey);
    return r;
}

static void onCreate(HWND hWnd)
{
    HDC hDC;
    HFONT hFont;
    LOGFONT lft;
    RECT rect;
    PIXELFORMATDESCRIPTOR pfd;
    int speed;
    hDC = GetDC(hWnd);
    hFont = GetStockObject(DEFAULT_GUI_FONT);
    GetObject(hFont, sizeof(LOGFONT), &lft);
    lft.lfHeight = 30;
    lft.lfWidth = 15;
    lft.lfWeight = FW_HEAVY;
    hFont = CreateFontIndirect(&lft);
    g_hFont = SelectObject(hDC, hFont);

    /* Set pixel format and create opengl rendering context */
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
    g_hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, g_hRC);
    ReleaseDC(hWnd, hDC);

    /* Initialize opengl */
    GetClientRect(hWnd, &rect);
    glInit(rect.right - rect.left, rect.bottom - rect.top);
    loadParasReg(&speed);
    setSpinSpeed(speed);
}

static void onDestroy(HWND hWnd)
{
    HDC hDC;
    HFONT hFont;
    hDC = GetDC(hWnd);
    hFont = SelectObject(hDC, g_hFont);
    DeleteObject(hFont);
    ReleaseDC(hWnd, hDC);
    wglDeleteContext(g_hRC);
}

static void onTimer(HWND hWnd)
{
    HDC hDC;
    hDC = GetDC(hWnd);
    glDraw();
    SwapBuffers(hDC);
    ReleaseDC(hWnd, hDC);
}

LRESULT CALLBACK ScreenSaverProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    static UINT_PTR timer;
    LRESULT r = 0;
    switch (msg) {
    case WM_CREATE:
        onCreate(hWnd);
        timer = SetTimer(hWnd, 1, 10, NULL);
        break;
    case WM_DESTROY:
        KillTimer(hWnd, 1);
        onDestroy(hWnd);
        PostQuitMessage(0);
        break;
    case WM_TIMER:
        if (wp == 1) {
            onTimer(hWnd);
        } else {
            r = DefScreenSaverProc(hWnd, msg, wp, lp);
        }
        break;
    default:
        r = DefScreenSaverProc(hWnd, msg, wp, lp);
        break;
    }
    return r;
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    static int speed = 1;
    static HWND scrSpeed = NULL;
    switch (msg) {
    case WM_INITDIALOG:
        loadParasReg(&speed);
        scrSpeed = GetDlgItem(hDlg, IDC_SCROLL_SPEED);
        SetScrollRange(scrSpeed, SB_CTL, MIN_SPEED, MAX_SPEED, FALSE);
        SetScrollPos(scrSpeed, SB_CTL, speed, TRUE);
        return FALSE;
        break;
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case IDOK:
            saveParasReg(speed);
            EndDialog(hDlg, TRUE);
            break;
        case IDCANCEL:
            EndDialog(hDlg, FALSE);
            break;
        default:
            break;
        }
        break;
    case WM_HSCROLL:
        switch (LOWORD(wp)) {
        case SB_PAGEUP:
        case SB_LINEUP:
            speed--;
            if (speed < MIN_SPEED) {
                speed = MIN_SPEED;
            }
            break;
        case SB_PAGEDOWN:
        case SB_LINEDOWN:
            speed++;
            if (speed > MAX_SPEED) {
                speed = MAX_SPEED;
            }
            break;
        case SB_THUMBPOSITION:
            speed = HIWORD(wp);
            break;
        case SB_BOTTOM:
            speed = MIN_SPEED;
            break;
        case SB_TOP:
            speed = MAX_SPEED;
            break;
        case SB_THUMBTRACK:
        case SB_ENDSCROLL:
            break;
        default:
            break;
        }
        SetScrollPos(scrSpeed, SB_CTL, speed, TRUE);
    default:
        return FALSE;
        break;
    }
    return TRUE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hIns)
{
    return TRUE;
}
