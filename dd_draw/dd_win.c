#include <tchar.h>

#include <win_lib.h> // Windows header must be put before dd_draw.

#include "dd_draw.h"

static HFONT g_hFont = NULL;

static void onCreate(HWND hWnd)
{
    HDC hDC;
    HFONT hFont;
    LOGFONT lft;
    hDC = GetDC(hWnd);
    hFont = GetStockObject(DEFAULT_GUI_FONT);
    GetObject(hFont, sizeof(LOGFONT), &lft);
    lft.lfHeight = 30;
    lft.lfWidth = 15;
    lft.lfWeight = FW_HEAVY;
    hFont = CreateFontIndirect(&lft);
    g_hFont = SelectObject(hDC, hFont);
    ReleaseDC(hWnd, hDC);
}

static void onDestroy(HWND hWnd)
{
    HDC hDC;
    HFONT hFont;
    hDC = GetDC(hWnd);
    hFont = SelectObject(hDC, g_hFont);
    DeleteObject(hFont);
    ReleaseDC(hWnd, hDC);
}

static void onKeyDown(HWND hWnd, int vkey)
{
    switch (vkey) {
    case 0x1B:
        DestroyWindow(hWnd);
        break;
    case 0x0D:
        switchSpin();
        break;
    default:
        break;
    }
}

static void onLButtonDown(HWND hWnd, int key, int x, int y)
{
    if (key == MK_LBUTTON) {
        speedUp();
    } else if (key == (MK_LBUTTON | MK_CONTROL)) {
        enlarge();
    }
}

static void onRButtonDown(HWND hWnd, int key, int x, int y)
{
    if (key == MK_RBUTTON) {
        speedDown();
    } else if (key == (MK_RBUTTON | MK_CONTROL)) {
        shrink();
    }
}

static void onIdle(HWND hWnd)
{
    ddDraw(hWnd);
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    LRESULT r = 0;
    switch (msg) {
    case WM_CREATE:
        onCreate(hWnd);
        break;
    case WM_DESTROY:
        onDestroy(hWnd);
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        onKeyDown(hWnd, (int)wp);
        break;
    case WM_LBUTTONDOWN:
        onLButtonDown(hWnd, (int)wp, LOWORD(lp), HIWORD(lp));
        break;
    case WM_RBUTTONDOWN:
        onRButtonDown(hWnd, (int)wp, LOWORD(lp), HIWORD(lp));
        break;
    default:
        r = DefWindowProc(hWnd, msg, wp, lp);
        break;
    }
    return r;
}

int APIENTRY wWinMain(_In_ HINSTANCE hIns, _In_opt_ HINSTANCE hPrevIns, _In_ LPWSTR cmd, _In_ int show)
{
    LPWSTR title = _T("DirectDraw Test");
    HWND hWnd = createWindow(hIns, _T("dd_draw"), title, WS_POPUP, wndProc);
    if (hWnd != NULL) {
        ShowWindow(hWnd, show);
        if (ddInit(hWnd) < 0) {
            MessageBox(hWnd, _T("Init DirectDraw Failed!"), title, MB_ICONERROR | MB_OK);
            DestroyWindow(hWnd);
            return -1;
        }
        int r = messageLoopIdle(hWnd, onIdle);
        ddRelease();
        return r;
    }
    return -1;
}
