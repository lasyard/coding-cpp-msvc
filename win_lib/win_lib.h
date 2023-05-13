#ifndef WIN_LIB_H
#define WIN_LIB_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

HWND createWindow(HINSTANCE hIns, LPWSTR name, LPWSTR title, long styles, WNDPROC wndProc);

int messageLoop();

int messageLoopIdle(HWND hWnd, void (*onIdle)(HWND));

#ifdef __cplusplus
}
#endif

#endif
