#ifndef _RAW_MOUSE_H_
#define _RAW_MOUSE_H_

#define RAW_MOUSE_LEFT_DOWN   (WM_USER + 1)
#define RAW_MOUSE_LEFT_UP     (WM_USER + 2)
#define RAW_MOUSE_MIDDLE_DOWN (WM_USER + 3)
#define RAW_MOUSE_MIDDLE_UP   (WM_USER + 4)
#define RAW_MOUSE_RIGHT_DOWN  (WM_USER + 5)
#define RAW_MOUSE_RIGHT_UP    (WM_USER + 6)
#define RAW_MOUSE_MOVE        (WM_USER + 7)

#define sz_RAW_MOUSE_STATE 8
#define GET_MOUSE_INDEX(x) ((int)((x) >> sz_RAW_MOUSE_STATE))
#define GET_MOUSE_STATE(x) ((unsigned char)((x)&0x000000FF))

union RawMouseState {
    struct {
        unsigned char left : 1;
        unsigned char mid : 1;
        unsigned char right : 1;
        unsigned char reserved : 5;
    };
    unsigned char ch;
};

#ifdef __cplusplus
extern "C" {
#endif

int openRawMouse(HWND hWnd, int num);
int isRawMouseOpen();
void closeRawMouse(HWND hWnd);
void hideRawMouse(HWND hWnd, int index);
void showRawMouse(HWND hWnd, int index);
void hideAllRawMouse(HWND hWnd);
void showAllRawMouse(HWND hWnd);

void setRawMouseIcon(HWND hWnd, int index, HICON hIcon);

LRESULT processRawMouse(HWND hWnd, HRAWINPUT hri);

void rawMouseBeforePaint(HDC hDC);
void rawMouseAfterPaint(HDC hDC);

#ifdef __cplusplus
}
#endif

#endif
