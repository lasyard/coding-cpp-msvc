#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <malloc.h>

#include "raw_mouse.h"

#define IS_SET(x, y) (((x) & (y)) == (y))

struct MouseInfo {
    HANDLE hDev;
    int visible;
    int x;
    int y;
    union RawMouseState state;
    int xHot;
    int yHot;
    HICON hIcon;
    HBITMAP hBitmap;
    int cx;
    int cy;
    HDC hDC;
};

static int g_num = 0;
static struct MouseInfo *g_mouse = NULL;

static void backupRawMouse(HDC hDC, int index)
{
    if (!g_mouse[index].visible) {
        return;
    }
    BitBlt(
        g_mouse[index].hDC,
        0,
        0,
        g_mouse[index].cx,
        g_mouse[index].cy,
        hDC,
        g_mouse[index].x - g_mouse[index].xHot,
        g_mouse[index].y - g_mouse[index].yHot,
        SRCCOPY
    );
}

static void backupAllRawMouse(HDC hDC)
{
    if (g_mouse == NULL) {
        return;
    }
    for (int i = 0; i < g_num; i++) {
        backupRawMouse(hDC, i);
    }
}

static void drawRawMouse(HDC hDC, int index)
{
    if (g_mouse[index].hDev == NULL || !g_mouse[index].visible) {
        return;
    }
    DrawIconEx(
        hDC,
        g_mouse[index].x - g_mouse[index].xHot,
        g_mouse[index].y - g_mouse[index].yHot,
        g_mouse[index].hIcon,
        g_mouse[index].cx,
        g_mouse[index].cy,
        0,
        NULL,
        DI_NORMAL | DI_COMPAT
    );
}

static void drawAllRawMouse(HDC hDC)
{
    if (g_mouse == NULL) {
        return;
    }
    for (int i = 0; i < g_num; i++) {
        drawRawMouse(hDC, i);
    }
}

static void eraseRawMouse(HDC hDC, int index)
{
    if (g_mouse[index].hDev == NULL || !g_mouse[index].visible) {
        return;
    }
    BitBlt(
        hDC,
        g_mouse[index].x - g_mouse[index].xHot,
        g_mouse[index].y - g_mouse[index].yHot,
        g_mouse[index].cx,
        g_mouse[index].cy,
        g_mouse[index].hDC,
        0,
        0,
        SRCCOPY
    );
}

static void eraseAllRawMouse(HDC hDC)
{
    if (g_mouse == NULL) {
        return;
    }
    for (int i = 0; i < g_num; i++) {
        eraseRawMouse(hDC, i);
    }
}

static void destroyRawMouse(int index)
{
    if (g_mouse[index].hIcon != NULL) {
        if (g_mouse[index].hDC != NULL) {
            DeleteDC(g_mouse[index].hDC);
            g_mouse[index].hDC = NULL;
        }
        if (g_mouse[index].hBitmap != NULL) {
            DeleteObject(g_mouse[index].hBitmap);
            g_mouse[index].hBitmap = NULL;
        }
    }
}

void setRawMouseIcon(HWND hWnd, int index, HICON hIcon)
{
    ICONINFO info;
    BITMAP bitmap;
    int cx, cy;
    if (g_mouse == NULL || index >= g_num) {
        return;
    }
    HDC hDC = GetDC(hWnd);
    /* Erase old icon */
    eraseAllRawMouse(hDC);
    destroyRawMouse(index);
    /* Set new icon */
    g_mouse[index].hIcon = hIcon;
    GetIconInfo(hIcon, &info);
    g_mouse[index].xHot = info.xHotspot;
    g_mouse[index].yHot = info.yHotspot;
    if (info.hbmColor != NULL) {
        GetObject(info.hbmColor, sizeof(bitmap), &bitmap);
        cx = bitmap.bmWidth;
        cy = bitmap.bmHeight;
    } else {
        GetObject(info.hbmMask, sizeof(bitmap), &bitmap);
        cx = bitmap.bmWidth;
        cy = bitmap.bmHeight / 2;
    }
    g_mouse[index].cx = cx;
    g_mouse[index].cy = cy;
    g_mouse[index].hBitmap = CreateCompatibleBitmap(hDC, cx, cy);
    g_mouse[index].hDC = CreateCompatibleDC(hDC);
    if (g_mouse[index].hDC != NULL) {
        SelectObject(g_mouse[index].hDC, g_mouse[index].hBitmap);
    }
    backupRawMouse(hDC, index);
    drawAllRawMouse(hDC);
    ReleaseDC(hWnd, hDC);
}

int openRawMouse(HWND hWnd, int num)
{
    RECT rect;
    PRAWINPUTDEVICELIST pdl;
    RAWINPUTDEVICE rid;
    /* Get raw input devices */
    UINT nDev = 0;
    if (GetRawInputDeviceList(NULL, &nDev, sizeof(RAWINPUTDEVICELIST)) != 0) {
        return -1;
    }
    pdl = (PRAWINPUTDEVICELIST)malloc(nDev * sizeof(RAWINPUTDEVICELIST));
    if (pdl == NULL) {
        SetLastError(ERROR_OUTOFMEMORY);
        return -1;
    }
    if (GetRawInputDeviceList(pdl, &nDev, sizeof(RAWINPUTDEVICELIST)) == -1) {
        free(pdl);
        return -1;
    }
    int count = 0;
    for (UINT i = 0; i < nDev; i++) {
#pragma warning(push)
#pragma warning(disable : 6385)
        if (pdl[i].dwType == RIM_TYPEMOUSE) {
#pragma warning(pop)
            count++;
        }
    }
    free(pdl);
    /* Create mouse info list */
    g_num = num;
    if (g_mouse != NULL) {
        closeRawMouse(hWnd);
    }
    g_mouse = malloc(g_num * sizeof(struct MouseInfo));
    if (g_mouse == NULL) {
        SetLastError(ERROR_OUTOFMEMORY);
        return -1;
    }
    memset(g_mouse, 0, sizeof(struct MouseInfo) * g_num);
    GetClientRect(hWnd, &rect);
    for (int i = 0; i < g_num; i++) {
#pragma warning(push)
#pragma warning(disable : 6386)
        g_mouse[i].x = (rect.left + rect.right) / 2;
        g_mouse[i].y = (rect.top + rect.bottom) / 2;
#pragma warning(pop)
    }
    /* Register raw mouse */
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    rid.dwFlags = RIDEV_NOLEGACY | RIDEV_CAPTUREMOUSE;
    rid.hwndTarget = hWnd;
    if (!RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
        return -1;
    }
    ShowCursor(FALSE);
    return count;
}

int isRawMouseOpen()
{
    return g_mouse != NULL;
}

void closeRawMouse(HWND hWnd)
{
    RAWINPUTDEVICE rid;
    hideAllRawMouse(hWnd);
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    rid.dwFlags = RIDEV_REMOVE;
    rid.hwndTarget = NULL;
    RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE));
    if (g_mouse != NULL) {
        for (int i = 0; i < g_num; i++)
            destroyRawMouse(i);
        free(g_mouse);
        g_mouse = NULL;
    }
    ShowCursor(TRUE);
}

void showRawMouse(HWND hWnd, int index)
{
    HDC hDC;
    if (g_mouse == NULL || index >= g_num) {
        return;
    }
    if (g_mouse[index].visible) {
        return;
    }
    hDC = GetDC(hWnd);
    eraseAllRawMouse(hDC);
    g_mouse[index].visible = 1;
    backupRawMouse(hDC, index);
    drawAllRawMouse(hDC);
    ReleaseDC(hWnd, hDC);
}

void hideRawMouse(HWND hWnd, int index)
{
    HDC hDC;
    if (g_mouse == NULL || index >= g_num) {
        return;
    }
    if (!g_mouse[index].visible) {
        return;
    }
    hDC = GetDC(hWnd);
    eraseRawMouse(hDC, index);
    g_mouse[index].visible = 0;
    drawAllRawMouse(hDC);
    ReleaseDC(hWnd, hDC);
}

void showAllRawMouse(HWND hWnd)
{
    HDC hDC;
    if (g_mouse == NULL) {
        return;
    }
    hDC = GetDC(hWnd);
    eraseAllRawMouse(hDC);
    for (int i = 0; i < g_num; i++) {
        g_mouse[i].visible = 1;
    }
    backupAllRawMouse(hDC);
    drawAllRawMouse(hDC);
    ReleaseDC(hWnd, hDC);
}

void hideAllRawMouse(HWND hWnd)
{
    HDC hDC;
    if (g_mouse == NULL) {
        return;
    }
    hDC = GetDC(hWnd);
    eraseAllRawMouse(hDC);
    for (int i = 0; i < g_num; i++) {
        g_mouse[i].visible = 0;
    }
    ReleaseDC(hWnd, hDC);
}

static int testRawMouseButton(HWND hWnd, int index, USHORT flag, USHORT state, UINT msg)
{
    if (IS_SET(flag, state)) {
        PostMessage(
            hWnd,
            msg,
            (WPARAM)((index << sz_RAW_MOUSE_STATE) | g_mouse[index].state.ch),
            (LPARAM)(g_mouse[index].y << 16 | g_mouse[index].x)
        );
        return 1;
    } else {
        return 0;
    }
}

static void updateRawMousePos(HWND hWnd, int index, int dx, int dy)
{
    HDC hDC;
    RECT rect;
    hDC = GetDC(hWnd);
    eraseAllRawMouse(hDC);
    g_mouse[index].x += dx;
    g_mouse[index].y += dy;
    GetClientRect(hWnd, &rect);
    if (g_mouse[index].x < rect.left) {
        g_mouse[index].x = rect.left;
    }
    if (g_mouse[index].x >= rect.right) {
        g_mouse[index].x = rect.right;
    }
    if (g_mouse[index].y < rect.top) {
        g_mouse[index].y = rect.top;
    }
    if (g_mouse[index].y >= rect.bottom) {
        g_mouse[index].y = rect.bottom;
    }
    backupRawMouse(hDC, index);
    drawAllRawMouse(hDC);
    ReleaseDC(hWnd, hDC);
    PostMessage(
        hWnd,
        RAW_MOUSE_MOVE,
        (WPARAM)((index << sz_RAW_MOUSE_STATE) | g_mouse[index].state.ch),
        (LPARAM)(g_mouse[index].y << 16 | g_mouse[index].x)
    );
}

LRESULT processRawMouse(HWND hWnd, HRAWINPUT hri)
{
    int i;
    LPBYTE lpb;
    RAWINPUT *raw;
    if (g_mouse == NULL) {
        return S_OK;
    }
    UINT size = 0;
    GetRawInputData(hri, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
    lpb = malloc(size);
    if (lpb == NULL) {
        return ERROR_OUTOFMEMORY;
    }
    if (GetRawInputData(hri, RID_INPUT, lpb, &size, sizeof(RAWINPUTHEADER)) != size) {
        free(lpb);
        return S_OK;
    }
    raw = (RAWINPUT *)lpb;
    if (raw->header.dwType != RIM_TYPEMOUSE) {
        DefRawInputProc(&raw, 1, sizeof(RAWINPUTHEADER));
        free(lpb);
        return 0;
    }
    for (i = 0; i < g_num && raw->header.hDevice != g_mouse[i].hDev; i++) {
        continue;
    }
    if (i == g_num) {
        for (i = 0; i < g_num; i++) {
            if (g_mouse[i].hDev == NULL) {
                g_mouse[i].hDev = raw->header.hDevice;
                break;
            }
        }
    } else {
        if (testRawMouseButton(
                hWnd,
                i,
                raw->data.mouse.usButtonFlags,
                RI_MOUSE_LEFT_BUTTON_DOWN,
                RAW_MOUSE_LEFT_DOWN
            )) {
            g_mouse[i].state.left = 1;
        }
        if (testRawMouseButton( //
                hWnd,
                i,
                raw->data.mouse.usButtonFlags,
                RI_MOUSE_LEFT_BUTTON_UP,
                RAW_MOUSE_LEFT_UP
            )) {
            g_mouse[i].state.left = 0;
        }
        if (testRawMouseButton(
                hWnd,
                i,
                raw->data.mouse.usButtonFlags,
                RI_MOUSE_MIDDLE_BUTTON_DOWN,
                RAW_MOUSE_MIDDLE_DOWN
            )) {
            g_mouse[i].state.mid = 1;
        }
        if (testRawMouseButton(
                hWnd,
                i,
                raw->data.mouse.usButtonFlags,
                RI_MOUSE_MIDDLE_BUTTON_UP,
                RAW_MOUSE_MIDDLE_UP
            )) {
            g_mouse[i].state.mid = 0;
        }
        if (testRawMouseButton(
                hWnd,
                i,
                raw->data.mouse.usButtonFlags,
                RI_MOUSE_RIGHT_BUTTON_DOWN,
                RAW_MOUSE_RIGHT_DOWN
            )) {
            g_mouse[i].state.right = 1;
        }
        if (testRawMouseButton( //
                hWnd,
                i,
                raw->data.mouse.usButtonFlags,
                RI_MOUSE_LEFT_BUTTON_UP,
                RAW_MOUSE_LEFT_UP
            )) {
            g_mouse[i].state.right = 0;
        }
        updateRawMousePos( //
            hWnd,
            i,
            raw->data.mouse.lLastX,
            raw->data.mouse.lLastY
        );
    }
    free(lpb);
    return S_OK;
}

void rawMouseBeforePaint(HDC hDC)
{
    eraseAllRawMouse(hDC);
}

void rawMouseAfterPaint(HDC hDC)
{
    backupAllRawMouse(hDC);
    drawAllRawMouse(hDC);
}
