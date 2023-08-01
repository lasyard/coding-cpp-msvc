#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <GL/gl.h>
#include <math.h>

#include "gl_draw_logo.h"

#define WIDTH  2.5                       /* Size of square */
#define RADIUS 5.0                       /* Radius of logo */
#define DIAG   (sqrt(2.0) * WIDTH / 2.0) /* Half diagonal size of square */
#define SLICE  500                       /* Number of slices */
#define VISION 10.0                      /* Size of visual field */

static void vertexPair(double theta, double phi)
{
    glColor4d(
        (1.0 + sin(theta / 4.0)) / 2.0,
        (1.0 + sin(theta / 4.0 + 2.0 * PI / 3.0)) / 2.0,
        (1.0 + sin(theta / 4.0 + 4.0 * PI / 3.0)) / 2.0,
        1.0
    );
    glVertex3d(
        (RADIUS + DIAG * cos(phi + PI / 4.0)) * cos(theta),
        (RADIUS + DIAG * cos(phi + PI / 4.0)) * sin(theta),
        DIAG * sin(phi + PI / 4.0)
    );
    glVertex3d(
        (RADIUS + DIAG * cos(phi + 3.0 * PI / 4.0)) * cos(theta),
        (RADIUS + DIAG * cos(phi + 3.0 * PI / 4.0)) * sin(theta),
        DIAG * sin(phi + 3.0 * PI / 4.0)
    );
}

void glDrawLogo()
{
    int i;
    double theta, phi;
    glBegin(GL_QUAD_STRIP);
    phi = 0.0;
    for (i = 0; i < 4 * SLICE; i++) {
        theta = (double)i * 2.0 * PI / (double)SLICE + PI / 6.0;
        phi = (double)i * 1.5 * PI / (double)SLICE;
        vertexPair(theta, phi);
    }
    vertexPair(PI / 6.0, 0.0);
    glEnd();
}

void glInitLogo(int width, int height)
{
    glDisable(GL_FOG);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glCullFace(GL_FRONT);
    glShadeModel(GL_SMOOTH);
    /* Set projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
        -VISION * (GLdouble)width / (GLdouble)height,
        VISION * (GLdouble)width / (GLdouble)height,
        -VISION,
        VISION,
        -100,
        100
    );
}
