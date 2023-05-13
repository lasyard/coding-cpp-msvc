#ifndef _GL_DRAW_H_
#define _GL_DRAW_H_

#ifdef __cplusplus
extern "C" {
#endif

void glInit(int w, int h);
void glDraw();

void switchObject();
void switchRotate();
void zoomIn();
void zoomOut();
void zoomStop();
void shiftStart(int x, int y);
void shiftOn(int x, int y);
void rotateView(int x, int y);

#ifdef __cplusplus
}
#endif

#endif
