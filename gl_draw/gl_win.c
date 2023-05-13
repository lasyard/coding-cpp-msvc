#include <tchar.h>

#include <win_lib.h>

#include "gl_draw.h"

static HFONT g_hFont = NULL;

static HGLRC g_hRC;

static void onCreate(HWND hWnd)
{
    HDC hDC;
    HFONT hFont;
    LOGFONT lft;
    RECT rect;
    PIXELFORMATDESCRIPTOR pfd;
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

static void onKeyDown(HWND hWnd, int vkey)
{
    switch (vkey) {
    case 0x1B:
        DestroyWindow(hWnd);
        break;
    case 0x0D:
        switchRotate();
        break;
    case 0x20:
        switchObject();
        break;
    default:
        break;
    }
}

static void onLButtonDown(HWND hWnd, int key, int x, int y)
{
    shiftStart(x, y);
}

static void onRButtonDown(HWND hWnd, int key, int x, int y)
{
    if (key == MK_RBUTTON) {
        zoomIn();
    } else if (key == (MK_RBUTTON | MK_CONTROL)) {
        zoomOut();
    }
}

static void onRButtonUp(HWND hWnd, int key, int x, int y)
{
    zoomStop();
}

static void onMouseMove(HWND hWnd, int key, int x, int y)
{
    if (key == MK_LBUTTON) {
        shiftOn(x, y);
    } else {
        rotateView(x, y);
    }
}

static void onIdle(HWND hWnd)
{
    HDC hDC;
    hDC = GetDC(hWnd);
    glDraw();
    SwapBuffers(hDC);
    ReleaseDC(hWnd, hDC);
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
    case WM_RBUTTONUP:
        onRButtonUp(hWnd, (int)wp, LOWORD(lp), HIWORD(lp));
        break;
    case WM_MOUSEMOVE:
        onMouseMove(hWnd, (int)wp, LOWORD(lp), HIWORD(lp));
        break;
    default:
        r = DefWindowProc(hWnd, msg, wp, lp);
        break;
    }
    return r;
}

int APIENTRY wWinMain(_In_ HINSTANCE hIns, _In_opt_ HINSTANCE hPrevIns, _In_ LPWSTR cmd, _In_ int show)
{
    HWND hWnd = createWindow(hIns, _T("gl_draw"), _T("OpenGL Test"), WS_POPUP, wndProc);
    if (hWnd != NULL) {
        ShowWindow(hWnd, show);
        return messageLoopIdle(hWnd, onIdle);
    }
    return -1;
}
