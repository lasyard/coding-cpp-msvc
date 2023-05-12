#define _CRT_SECURE_NO_WARNINGS

#include <ddraw.h>
#include <math.h>
#include <stdio.h>
#include <tchar.h>
#include <time.h>

#include "dd_draw.h"

#define PI 3.1415926

static LPDIRECTDRAW lpDD = NULL;
static LPDIRECTDRAWSURFACE lpDDSPrimary = NULL;
static LPDIRECTDRAWSURFACE lpDDSBack = NULL;

static int g_radius = 100;
static int g_speed = 32;
static int g_spin = 0;

static void drawOneSixthPie(HDC hDC, int x, int y, int radius, double angle, COLORREF color)
{
    SetDCBrushColor(hDC, color);
    SetDCPenColor(hDC, color);
    Pie(hDC,
        x - radius,
        y - radius,
        x + radius,
        y + radius,
        x + (int)(radius * cos(angle)),
        y - (int)(radius * sin(angle)),
        x + (int)(radius * cos(angle + PI / 3.0)),
        y - (int)(radius * sin(angle + PI / 3.0)));
}

static void drawPie(HDC hDC, int x, int y, int radius, double angle)
{
    HBRUSH brush = (HBRUSH)SelectObject(hDC, GetStockObject(DC_BRUSH));
    HPEN pen = (HPEN)SelectObject(hDC, GetStockObject(DC_PEN));
    drawOneSixthPie(hDC, x, y, radius, angle + 0.0 * PI / 3.0, RGB(255, 0, 0));
    drawOneSixthPie(hDC, x, y, radius, angle + 1.0 * PI / 3.0, RGB(255, 255, 0));
    drawOneSixthPie(hDC, x, y, radius, angle + 2.0 * PI / 3.0, RGB(0, 255, 0));
    drawOneSixthPie(hDC, x, y, radius, angle + 3.0 * PI / 3.0, RGB(0, 255, 255));
    drawOneSixthPie(hDC, x, y, radius, angle + 4.0 * PI / 3.0, RGB(0, 0, 255));
    drawOneSixthPie(hDC, x, y, radius, angle + 5.0 * PI / 3.0, RGB(255, 0, 255));
    SelectObject(hDC, brush);
    SelectObject(hDC, pen);
}

void ddRelease()
{
    if (lpDD != NULL) {
        if (lpDDSPrimary != NULL) {
            if (lpDDSBack != NULL) {
                lpDDSBack->Release();
                lpDDSBack = NULL;
            }
            lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
        }
        lpDD->Release();
        lpDD = NULL;
    }
}

int ddInit(HWND hWnd)
{
    DDSURFACEDESC ddsd;
    if (DirectDrawCreate(NULL, &lpDD, NULL) != DD_OK) {
        return -1;
    }
    if (lpDD->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) != DD_OK) {
        ddRelease();
        return -1;
    }
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
    ddsd.dwBackBufferCount = 1;
    if (lpDD->CreateSurface(&ddsd, &lpDDSPrimary, NULL) != DD_OK) {
        ddRelease();
        return -1;
    }
    ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
    if (lpDDSPrimary->GetAttachedSurface(&ddsd.ddsCaps, &lpDDSBack) != DD_OK) {
        ddRelease();
        return -1;
    }
    return 0;
}

void ddDraw(HWND hWnd)
{
    static time_t tm;
    static int count;
    static TCHAR text[] = TEXT("DirectDraw Practice\n"
                               "Enter = spin, ESC = quit\n"
                               "Click = speed up, right click = speed down\n"
                               "Ctrl+click = enlarge, Ctrl+right click = shrink");
    static RECT upLeft = {10, 10, 210, 60};
    static int index = 0;
    static TCHAR s[4];
    HDC hDC;
    HRESULT r;
    if (lpDDSBack->GetDC(&hDC) == DD_OK) {
        time_t tm1;
        RECT rect;
        GetClientRect(hWnd, &rect);
        FillRect(hDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
        drawPie(
            hDC,
            (rect.right - rect.left) / 2,
            (rect.bottom - rect.top) / 2,
            g_radius,
            (double)index * 2.0 * PI / (double)g_speed
        );
        SetBkMode(hDC, TRANSPARENT);
        SetTextColor(hDC, RGB(0, 255, 0));
        DrawText(hDC, text, sizeof(text) / sizeof(TCHAR) - 1, &rect, DT_CENTER | /* DT_SINGLELINE |*/ DT_BOTTOM);
        count++;
        tm1 = time(NULL);
        if (tm1 >= tm + 1) {
            _stprintf(s, TEXT("%3d"), count);
            tm = tm1;
            count = 0;
        }
        DrawText(hDC, s, sizeof(s) / sizeof(TCHAR) - 1, &upLeft, DT_LEFT);
        lpDDSBack->ReleaseDC(hDC);
    }
    if (g_spin) {
        index++;
        index %= g_speed;
    }
    for (;;) {
        r = lpDDSPrimary->Flip(NULL, 0);
        if (r == DD_OK) {
            break;
        }
        if (r == DDERR_SURFACELOST) {
            r = lpDDSPrimary->Restore();
            if (r != DD_OK) {
                break;
            }
        }
        if (r != DDERR_WASSTILLDRAWING) {
            break;
        }
    }
}

void switchSpin()
{
    g_spin = 1 - g_spin;
}

void speedDown()
{
    if (g_spin) {
        g_speed *= 2;
        if (g_speed > 1024) {
            g_speed = 1024;
        }
    }
}

void speedUp()
{
    if (g_spin) {
        g_speed /= 2;
        if (g_speed < 1) {
            g_speed = 1;
        }
    }
}

void enlarge()
{
    g_radius += 10;
    if (g_radius > 600) {
        g_radius = 600;
    }
}

void shrink()
{
    g_radius -= 10;
    if (g_radius < 10) {
        g_radius = 10;
    }
}
