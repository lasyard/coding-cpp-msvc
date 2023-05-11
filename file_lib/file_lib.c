#include <stdio.h>
#include <stdlib.h>

#include "file_lib.h"

bool is_dir(const struct _finddata_t *file)
{
    return (file->attrib & _A_SUBDIR) == _A_SUBDIR;
}

bool do_chdir(const char *name)
{
    if (_chdir(name) == 0) {
        return true;
    }
    fprintf(stderr, "Cannot change directory to \"%s\", errno = %d.", name, errno);
    return false;
}
