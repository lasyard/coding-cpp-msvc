#include <tchar.h>
#include <win_lib.h>

#include "raw_mouse.h"

#define MAX_MOUSE_NUM 3

static HFONT g_hFont = NULL;

static initRawMouse(HWND hWnd)
{
    int i;
    openRawMouse(hWnd, MAX_MOUSE_NUM);
    for (i = 0; i < MAX_MOUSE_NUM; i++) {
        setRawMouseIcon(hWnd, i, LoadCursor(NULL, IDC_CROSS));
    }
    showAllRawMouse(hWnd);
}

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
    initRawMouse(hWnd);
}

static void onDestroy(HWND hWnd)
{
    HDC hDC;
    HFONT hFont;
    closeRawMouse(hWnd);
    hDC = GetDC(hWnd);
    hFont = SelectObject(hDC, g_hFont);
    DeleteObject(hFont);
    ReleaseDC(hWnd, hDC);
}

static void onPaint(HWND hWnd, HDC hDC)
{
    static TCHAR text[] = TEXT("ESC = switch mode, Enter = clear window");
    RECT rect;
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(255, 0, 0));
    GetClientRect(hWnd, &rect);
    DrawText(hDC, text, sizeof(text) / sizeof(TCHAR) - 1, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

static void onEraseBkGnd(HWND hWnd, HDC hDC)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    FillRect(hDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

static void onKeyDown(HWND hWnd, int vkey)
{
    switch (vkey) {
    case 0x1B:
        if (isRawMouseOpen()) {
            closeRawMouse(hWnd);
        } else {
            initRawMouse(hWnd);
        }
        break;
    case 0x0D:
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    default:
        break;
    }
}

static void onLButtonDown(HWND hWnd, int x, int y)
{
    HDC hDC;
    hDC = GetDC(hWnd);
    MoveToEx(hDC, x, y, NULL);
    ReleaseDC(hWnd, hDC);
}

static void onMouseMove(HWND hWnd, int button, int x, int y)
{
    if (button == MK_LBUTTON) {
        HDC hDC;
        HPEN hPen;
        hDC = GetDC(hWnd);
        hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        hPen = SelectObject(hDC, hPen);
        LineTo(hDC, x, y);
        hPen = SelectObject(hDC, hPen);
        DeleteObject(hPen);
        ReleaseDC(hWnd, hDC);
    }
}

static void onRawMouseLeftDown(HWND hWnd, int index, unsigned char state, int x, int y)
{
    HDC hDC;
    hDC = GetDC(hWnd);
    MoveToEx(hDC, x, y, NULL);
    ReleaseDC(hWnd, hDC);
}

static void onRawMouseMove(HWND hWnd, int index, unsigned char state, int x, int y)
{
    static COLORREF color[] = {RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255)};
    union RawMouseState st;
    st.ch = state;
    if (st.left == 1) {
        HDC hDC;
        HPEN hPen;
        hDC = GetDC(hWnd);
        rawMouseBeforePaint(hDC);
        hPen = CreatePen(PS_SOLID, 5, color[index]);
        hPen = SelectObject(hDC, hPen);
        LineTo(hDC, x, y);
        hPen = SelectObject(hDC, hPen);
        DeleteObject(hPen);
        rawMouseAfterPaint(hDC);
        ReleaseDC(hWnd, hDC);
    }
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
    case WM_SIZE:
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hDC;
        hDC = BeginPaint(hWnd, &ps);
        rawMouseBeforePaint(hDC);
        onPaint(hWnd, hDC);
        rawMouseAfterPaint(hDC);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_ERASEBKGND: {
        HDC hDC = (HDC)wp;
        rawMouseBeforePaint(hDC);
        onEraseBkGnd(hWnd, hDC);
        rawMouseAfterPaint(hDC);
        break;
    }
    case WM_INPUT:
        r = processRawMouse(hWnd, (HRAWINPUT)lp);
        break;
    case WM_KEYDOWN:
        onKeyDown(hWnd, (int)wp);
        break;
    case WM_LBUTTONDOWN:
        onLButtonDown(hWnd, LOWORD(lp), HIWORD(lp));
        break;
    case WM_MOUSEMOVE:
        onMouseMove(hWnd, (int)wp, LOWORD(lp), HIWORD(lp));
        break;
    case RAW_MOUSE_LEFT_DOWN:
        onRawMouseLeftDown(hWnd, GET_MOUSE_INDEX(wp), GET_MOUSE_STATE(wp), LOWORD(lp), HIWORD(lp));
        break;
    case RAW_MOUSE_MOVE:
        onRawMouseMove(hWnd, GET_MOUSE_INDEX(wp), GET_MOUSE_STATE(wp), LOWORD(lp), HIWORD(lp));
        break;
    default:
        r = DefWindowProc(hWnd, msg, wp, lp);
        break;
    }
    return r;
}

int APIENTRY wWinMain(_In_ HINSTANCE hIns, _In_opt_ HINSTANCE hPrevIns, _In_ LPWSTR cmd, _In_ int show)
{
    HWND hWnd = createWindow(hIns, _T("raw_mouse"), _T("Raw Mouse Test"), WS_TILEDWINDOW, wndProc);
    if (hWnd != NULL) {
        ShowWindow(hWnd, show);
        return messageLoop();
    }
    return -1;
}
