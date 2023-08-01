#ifndef GL_DRAW_LOGO_H
#define GL_DRAW_LOGO_H

#ifndef PI
#define PI 3.1415926
#endif

#define LIMIT_MOVE(m, var, negLimit, negNew, posLimit, posNew)                                                         \
    do {                                                                                                               \
        if (m) {                                                                                                       \
            (var) += (GLfloat)(m);                                                                                     \
            if ((var) < (negLimit)) {                                                                                  \
                (var) = (negNew);                                                                                      \
            } else if ((var) > (posLimit)) {                                                                           \
                (var) = (posNew);                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#ifdef __cplusplus
extern "C" {
#endif

void glDrawLogo();
void glInitLogo(int width, int height);

#ifdef __cplusplus
}
#endif

#endif
