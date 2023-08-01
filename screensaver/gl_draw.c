#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include "gl_draw.h"

#include "gl_draw_logo.h"

/* Window */
static int g_width;
static int g_height;

/* Diplay list */
static GLuint g_list1;

/* Spinning Speed */
static int g_speed = 1;

void glInit(int w, int h)
{
    g_width = w;
    g_height = h;
    /* Set clear color and depth */
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearDepth(1.0);
    /* Enable depth test */
    glEnable(GL_DEPTH_TEST);
    /* Set viewport */
    glViewport(0, 0, (GLint)w, (GLint)h);
    /* Set draw properties */
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    /* Enable vertex array */
    glEnableClientState(GL_VERTEX_ARRAY);
    /* Set diplay list 1 (for logo) */
    g_list1 = glGenLists(1);
    glNewList(g_list1, GL_COMPILE);
    glDrawLogo();
    glEndList();
    glInitLogo(w, h);
}

void glDraw()
{
    static GLdouble angle = 0.0;
    /* Clear background */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* Set model view matrix */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 20.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    LIMIT_MOVE(g_speed, angle, 0.0, 359.0, 359.0, 0.0);
    glRotated(angle, 0.0, 0.0, 1.0);
    glCallList(g_list1);
    glFlush();
}

void setSpinSpeed(int v)
{
    g_speed = v;
}
