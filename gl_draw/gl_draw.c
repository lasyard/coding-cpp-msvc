#define WIN32_LEAN_AND_MEAN
#include <windows.h> // This must be put before GL headers

#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdbool.h>

#include "gl_draw.h"

#include "gl_draw_logo.h"

#define Z_NEAR 0.5
#define Z_FAR  100.0

/* Parameter of cube */
#define TEX_HEIGHT 64
#define TEX_WIDTH  64
#define CUBE_NUM   3
#define CUBE_LONG  2.0

struct Xyzi {
    int x, y, z;
};

struct Xyz {
    GLdouble x, y, z;
};

struct Polar {
    GLdouble longitude, latitude, altitude;
};

#define RAD(x) ((x)*PI / 180)

#define POLAR2XYZ(po, xyz)                                                                                             \
    do {                                                                                                               \
        (xyz).x = (po).altitude * cos(RAD((po).latitude)) * sin(RAD((po).longitude));                                  \
        (xyz).y = (po).altitude * sin(RAD((po).latitude));                                                             \
        (xyz).z = (po).altitude * cos(RAD((po).latitude)) * cos(RAD((po).longitude));                                  \
    } while (0)

/* Window */
static int g_width;
static int g_height;

/* Diplay list */
static GLuint g_list0;
static GLuint g_list1;

/* Texture */
static GLuint g_tex;
static GLubyte g_texImage[TEX_HEIGHT][TEX_WIDTH][4];

/* Control */
static bool g_isLogo = false;
static GLdouble g_rotate = 0.0;
static struct Xyz g_shift = {1E-6, 0.0, 0.0};
static struct Xyzi g_start = {0, 0, 0};

/* Camera position control */
static struct Xyz g_move = {0.0, 0.0, 0.0};

/*
 * Draw cubes
 */

static void makeTexImage()
{
    int i, j;
    GLubyte c;
    for (i = 0; i < TEX_HEIGHT; i++) {
        for (j = 0; j < TEX_WIDTH; j++) {
            c = (i ^ j) & 0x10 ? 0xFF : 0xAA;
            g_texImage[i][j][0] = c;
            g_texImage[i][j][1] = c;
            g_texImage[i][j][2] = c;
            g_texImage[i][j][3] = 0xFF;
        }
    }
}

static void glInitCube()
{
    glEnable(GL_FOG);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glCullFace(GL_BACK);
    glShadeModel(GL_FLAT);
    /* Set projection */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, (GLdouble)g_width / (GLdouble)g_height, Z_NEAR, Z_FAR);
}

static void glDrawSquare(GLdouble r, GLdouble g, GLdouble b)
{
    static GLdouble vData[] = {
        -CUBE_LONG / 2.0,
        -CUBE_LONG / 2.0,
        0,
        +CUBE_LONG / 2.0,
        -CUBE_LONG / 2.0,
        0,
        +CUBE_LONG / 2.0,
        +CUBE_LONG / 2.0,
        0,
        -CUBE_LONG / 2.0,
        +CUBE_LONG / 2.0,
        0,
    };
    static GLint pData[] = {0, 1, 2, 3};
    static GLdouble texSEq[] = {1.0, 0.0, 0.0, 0.0};
    static GLdouble texTEq[] = {0.0, 1.0, 0.0, 0.0};
    glVertexPointer(3, GL_DOUBLE, 0, vData);
    glColor4d(r, g, b, 1.0);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, pData);
    glTexGendv(GL_S, GL_OBJECT_PLANE, texSEq);
    glTexGendv(GL_T, GL_OBJECT_PLANE, texTEq);
}

static void glDrawCube(GLdouble x, GLdouble y, GLdouble z)
{
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glPushMatrix();
    glTranslated(x, y, z);
    /* Front plane */
    glPushMatrix();
    glTranslated(0.0, 0.0, CUBE_LONG / 2.0);
    glDrawSquare(1.0, 1.0, 1.0);
    glPopMatrix();
    /* Back plane */
    glPushMatrix();
    glTranslated(0.0, 0.0, -CUBE_LONG / 2.0);
    glRotated(180.0, 1.0, 0.0, 0.0);
    glDrawSquare(1.0, 1.0, 0.0);
    glPopMatrix();
    /* Left plane */
    glPushMatrix();
    glTranslated(-CUBE_LONG / 2.0, 0.0, 0.0);
    glRotated(-90.0, 0.0, 1.0, 0.0);
    glDrawSquare(1.0, 0.0, 0.0);
    glPopMatrix();
    /* Right plane */
    glPushMatrix();
    glTranslated(CUBE_LONG / 2.0, 0.0, 0.0);
    glRotated(90.0, 0.0, 1.0, 0.0);
    glDrawSquare(1.0, 0.5, 0.0);
    glPopMatrix();
    /* Top plane */
    glPushMatrix();
    glTranslated(0.0, CUBE_LONG / 2.0, 0.0);
    glRotated(-90.0, 1.0, 0.0, 0.0);
    glDrawSquare(0.0, 1.0, 0.0);
    glPopMatrix();
    /* Bottom plane */
    glPushMatrix();
    glTranslated(0.0, -CUBE_LONG / 2.0, 0.0);
    glRotated(90.0, 1.0, 0.0, 0.0);
    glDrawSquare(0.0, 0.0, 1.0);
    glPopMatrix();
    /* Restore matrix */
    glPopMatrix();
    glDisable(GL_COLOR_MATERIAL);
}

void glInit(int w, int h)
{
    static GLfloat lightModelAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    static GLfloat light0Ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    static GLfloat light0Diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    static GLfloat light1Ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    static GLfloat light1Diffuse[] = {0.3f, 0.3f, 0.3f, 1.0f};
    static GLfloat fogColor[] = {0.1f, 0.1f, 0.1f, 1.0f};
    g_width = w;
    g_height = h;
    /* Set fog */
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.1f);
    glHint(GL_FOG_HINT, GL_DONT_CARE);
    glFogf(GL_FOG_START, Z_NEAR);
    glFogf(GL_FOG_END, Z_FAR);
    /* Set lighting */
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightModelAmbient);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0Ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Diffuse);
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1Ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Diffuse);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    /* Set texturing */
    makeTexImage();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &g_tex);
    glBindTexture(GL_TEXTURE_2D, g_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, g_texImage);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glEnable(GL_TEXTURE_GEN_T);
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
    /* Set diplay list 0 (for cube) */
    g_list0 = glGenLists(1);
    glNewList(g_list0, GL_COMPILE);
    for (int i = 0; i < CUBE_NUM; i++) {
        for (int j = 0; j < CUBE_NUM; j++) {
            for (int k = 0; k < CUBE_NUM; k++) {
                if (i == 0 || i == CUBE_NUM - 1 || j == 0 || j == CUBE_NUM - 1 || k == 0 || k == CUBE_NUM - 1) {
                    glDrawCube(
                        ((GLdouble)i - (GLdouble)(CUBE_NUM - 1) / 2.0) * (CUBE_LONG + 0.1),
                        ((GLdouble)j - (GLdouble)(CUBE_NUM - 1) / 2.0) * (CUBE_LONG + 0.1),
                        ((GLdouble)k - (GLdouble)(CUBE_NUM - 1) / 2.0) * (CUBE_LONG + 0.1)
                    );
                }
            }
        }
    }
    glEndList();
    /* Set diplay list 1 (for logo) */
    g_list1 = glGenLists(1);
    glNewList(g_list1, GL_COMPILE);
    glDrawLogo();
    glEndList();
    if (g_isLogo) {
        glInitLogo(w, h);
    } else {
        glInitCube();
    }
}

void glDraw()
{
    static struct Polar cameraP = {0.0, 0.0, 20.0};
    static GLfloat light0Pos[] = {0.0f, 10.0f, 10.0f, 1.0f};
    static GLfloat light1Pos[] = {10.0f, 10.0f, 0.0f, 1.0f};
    static GLdouble angle = 0.0;
    struct Xyz camera;
    /* Clear background */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* Set model view matrix */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    LIMIT_MOVE(g_move.x, cameraP.longitude, 0.0, 359.0, 359.0, 0.0);
    LIMIT_MOVE(g_move.y, cameraP.latitude, -90.0, -90.0, 90.0, 90.0);
    LIMIT_MOVE(g_move.z, cameraP.altitude, Z_NEAR + 10.0, Z_NEAR + 10.0, Z_FAR / 2 + 10.0, Z_FAR / 2 + 10.0);
    POLAR2XYZ(cameraP, camera);
    gluLookAt(camera.x, camera.y, camera.z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
    glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
    LIMIT_MOVE(g_rotate, angle, 0.0, 359.0, 359.0, 0.0);
    glRotated(angle, -g_shift.y, g_shift.x, 1E-6);
    glTranslated(g_shift.x, g_shift.y, g_shift.z);
    glRotated(angle, 0.0, 0.0, 1.0);
    if (g_isLogo) {
        glCallList(g_list1);
    } else {
        glCallList(g_list0);
    }
    glFlush();
}

void switchObject()
{
    if (g_isLogo) {
        g_isLogo = false;
        glInitCube();
    } else {
        g_isLogo = true;
        glInitLogo(g_width, g_height);
    }
}

void switchRotate()
{
    g_rotate = ((g_rotate == 0.0) ? 0.2 : 0.0);
}

void zoomIn()
{
    g_move.z = 0.1;
}

void zoomOut()
{
    g_move.z = -0.1;
}

void zoomStop()
{
    g_move.z = 0.0;
}

void shiftStart(int x, int y)
{
    g_start.x = x;
    g_start.y = y;
}

void shiftOn(int x, int y)
{
    g_shift.x += 5.0 * (double)(x - g_start.x) / (double)g_width;
    g_shift.y += 5.0 * (double)(g_start.y - y) / (double)g_height;
    g_start.x = x;
    g_start.y = y;
}

void rotateView(int x, int y)
{
    if (x < 5) {
        g_move.x = -0.1;
    } else if (x > g_width - 5) {
        g_move.x = 0.1;
    } else {
        g_move.x = 0.0;
    }
    if (y < 5) {
        g_move.y = 0.1;
    } else if (y > g_height - 5) {
        g_move.y = -0.1;
    } else {
        g_move.y = 0.0;
    }
}
