#ifndef FILE_LIB_H
#define FILE_LIB_H

#include <direct.h>
#include <io.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool is_dir(const struct _finddata_t *file);
bool do_chdir(const char *name);

#ifdef __cplusplus
}
#endif

#endif
