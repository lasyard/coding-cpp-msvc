#include <stdio.h>

#include "win_lib.h"

HWND createWindow(HINSTANCE hIns, LPWSTR name, LPWSTR title, long styles, WNDPROC wndProc)
{
    WNDCLASSEX wce;
    wce.cbSize = sizeof(WNDCLASSEX);
    wce.style = CS_OWNDC;
    wce.lpfnWndProc = wndProc;
    wce.cbClsExtra = 0;
    wce.cbWndExtra = 0;
    wce.hInstance = hIns;
    wce.hIcon = NULL;
    wce.hCursor = LoadCursor(NULL, IDC_ARROW);
    wce.hbrBackground = NULL;
    wce.lpszMenuName = NULL;
    wce.lpszClassName = name;
    wce.hIconSm = NULL;
    RegisterClassEx(&wce);
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    int width = CW_USEDEFAULT;
    int height = CW_USEDEFAULT;
    if ((styles & WS_POPUP) == WS_POPUP) {
        x = 0;
        y = 0;
        width = GetSystemMetrics(SM_CXSCREEN);
        height = GetSystemMetrics(SM_CYSCREEN);
    }
    HWND hWnd = CreateWindow(name, title, styles, x, y, width, height, NULL, NULL, hIns, 0);
    if (hWnd == NULL) {
        fprintf(stderr, "Create window failed.");
    }
    return hWnd;
}

int messageLoop()
{
    MSG msg;
    for (; GetMessage(&msg, NULL, 0, 0);) {
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

int messageLoopIdle(HWND hWnd, void (*onIdle)(HWND))
{
    MSG msg;
    for (;;) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }
            DispatchMessage(&msg);
        }
        onIdle(hWnd);
    }
    return (int)msg.wParam;
}
