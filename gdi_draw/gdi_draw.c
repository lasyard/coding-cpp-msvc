#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include <TCHAR.h>
#include <stdio.h>
#include <windows.h>

#define SX      64
#define SY      64
#define WX      (SX * 10)
#define WY      (SY * 15)
#define PX      SX
#define PY      SY
#define Y_START 65
#define Y_END   80

static HFONT g_hFont = NULL;
static HPEN g_hPen = NULL;
static HBRUSH g_hBrush = NULL;

static float x[] = {1970, 1971, 1972, 1973, 1974, 1975, 1976, 1977, 1978, 1979};
static float y[] = {69, 70, 72, 68, 73, 71, 75, 74, 78, 77};

static void draw(HDC hDC)
{
    int i;
    TCHAR buf[5];
    HPEN hPen;
    float m = 0, n = 0;
    float p = 0, q = 0;
    float a, b;
    int N = sizeof(x) / sizeof(float);
    for (i = 0; i < N; i++) {
        m += x[i] * x[i];
        n += x[i];
        p += x[i] * y[i];
        q += y[i];
    }
    a = (N * p - n * q) / (N * m - n * n);
    b = (m * q - n * p) / (N * m - n * n);
    for (i = 0; i < N; i++) {
        Rectangle(hDC, i * SX, 0, i * SX + SX - 1, (int)((y[i] - Y_START) * SY));
    }
    hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    hPen = SelectObject(hDC, hPen);
    MoveToEx(hDC, 0, WY, NULL);
    LineTo(hDC, 0, 0);
    LineTo(hDC, WX, 0);
    hPen = SelectObject(hDC, hPen);
    DeleteObject(hPen);
    for (i = Y_START; i <= Y_END; i += 5) {
        RECT rect = {-PX, (i - Y_START + 5) * SY, 0, (i - Y_START) * SY};
        _stprintf(buf, TEXT("%2d"), i);
        DrawText(hDC, buf, 2, &rect, DT_RIGHT | DT_SINGLELINE | DT_BOTTOM);
    }
    for (i = 0; i < 10; i++) {
        RECT rect = {i * SX, 0, i * SX + SX - 1, -SY};
        _stprintf(buf, TEXT("%4d"), i + 1970);
        DrawText(hDC, buf, 4, &rect, DT_CENTER | DT_SINGLELINE | DT_TOP);
    }
    hPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 0));
    hPen = SelectObject(hDC, hPen);
    MoveToEx(hDC, 0, (int)((a * (1970 - 0.5) + b - Y_START) * SY), NULL);
    LineTo(hDC, 10 * SX, (int)((a * (1979 + 0.5) + b - Y_START) * SY));
    hPen = SelectObject(hDC, hPen);
    DeleteObject(hPen);
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
    lft.lfWidth = 10;
    lft.lfWeight = FW_HEAVY;
    hFont = CreateFontIndirect(&lft);
    g_hFont = SelectObject(hDC, hFont);
    g_hPen = SelectObject(hDC, (HPEN)GetStockObject(WHITE_PEN));
    g_hBrush = SelectObject(hDC, (HBRUSH)GetStockObject(GRAY_BRUSH));
    ReleaseDC(hWnd, hDC);
}

static void onDestroy(HWND hWnd)
{
    HDC hDC;
    HFONT hFont;
    hDC = GetDC(hWnd);
    hFont = SelectObject(hDC, g_hFont);
    DeleteObject(hFont);
    SelectObject(hDC, g_hPen);
    SelectObject(hDC, g_hBrush);
    ReleaseDC(hWnd, hDC);
}

static void onPaint(HWND hWnd, HDC hDC)
{
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(255, 255, 255));
    draw(hDC);
}

static void onEraseBkGnd(HWND hWnd, HDC hDC)
{
    RECT rect = {-PX, -PY, WX + PX, WY + PY};
    FillRect(hDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

static void onKeyDown(HWND hWnd, int vkey)
{
    switch (vkey) {
    case 0x1B:
        DestroyWindow(hWnd);
        break;
    default:
        break;
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
    case WM_SIZE: {
        int width = LOWORD(lp);
        int height = HIWORD(lp);
        HDC hDC = GetDC(hWnd);
        int TX = WX + PX + PX;
        int TY = WY + PY + PY;
        SetMapMode(hDC, MM_ANISOTROPIC);
        SetWindowOrgEx(hDC, 0, 0, NULL);
        SetWindowExtEx(hDC, TX, TY, NULL);
        SetViewportOrgEx(hDC, PX * width / TX, height - PY * height / TY, NULL);
        SetViewportExtEx(hDC, width, -height, NULL);
        ReleaseDC(hWnd, hDC);
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hDC;
        hDC = BeginPaint(hWnd, &ps);
        onPaint(hWnd, hDC);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_ERASEBKGND: {
        HDC hDC = (HDC)wp;
        onEraseBkGnd(hWnd, hDC);
        break;
    }
    case WM_KEYDOWN:
        onKeyDown(hWnd, (int)wp);
        break;
    default:
        r = DefWindowProc(hWnd, msg, wp, lp);
        break;
    }
    return r;
}

int APIENTRY wWinMain(_In_ HINSTANCE hIns, _In_opt_ HINSTANCE hPrevIns, _In_ LPWSTR cmd, _In_ int show)
{
    TCHAR wndClassName[] = TEXT("GDI Draw");
    MSG msg;
    HWND hWnd;
    int r = 0;
    WNDCLASSEX wce;
    wce.cbSize = sizeof(WNDCLASSEX);
    wce.style = CS_OWNDC;
    wce.lpfnWndProc = (WNDPROC)wndProc;
    wce.cbClsExtra = 0;
    wce.cbWndExtra = 0;
    wce.hInstance = hIns;
    wce.hIcon = NULL;
    wce.hCursor = LoadCursor(NULL, IDC_ARROW);
    wce.hbrBackground = NULL;
    wce.lpszMenuName = NULL;
    wce.lpszClassName = wndClassName;
    wce.hIconSm = NULL;
    RegisterClassEx(&wce);
    hWnd = CreateWindow(
        wndClassName,
        TEXT("GDI Draw"),
        WS_TILEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hIns,
        0
    );
    if (hWnd != NULL) {
        ShowWindow(hWnd, show);
        for (; GetMessage(&msg, NULL, 0, 0);) {
            DispatchMessage(&msg);
        }
        r = (int)msg.wParam;
    }
    return r;
}
