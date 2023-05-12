#ifndef _DD_DRAW_H_
#define _DD_DRAW_H_

#ifdef __cplusplus
extern "C" {
#endif

int ddInit(HWND hWnd);
void ddDraw(HWND hWnd);
void ddRelease();

void switchSpin();
void speedDown();
void speedUp();
void enlarge();
void shrink();

#ifdef __cplusplus
}
#endif

#endif
