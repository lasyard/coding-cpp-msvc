#define _CRT_SECURE_NO_WARNINGS

#include <direct.h>
#include <io.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILE_COUNT_BLOCK 256
#define TIME_BUF_LEN     64
#define TIME_FORMAT      "%y-%m-%d"

/* options */
static int g_show_file = 0;
static int g_show_size = 0;
static int g_show_time = 0;

static int g_depth = 0;

static void search_file();

static void show_help()
{
    printf("Display tree of the specified path.\n"
           "Usage: tree [-f] [-s] [-t] [path]\n"
           "-f show files\n"
           "-s show files with file size\n"
           "-t show files with modification time\n"
           "\n");
}

static bool is_dir(const struct _finddata_t *file)
{
    return (file->attrib & _A_SUBDIR) == _A_SUBDIR;
}

static void safe_chdir(const char *name)
{
    if (_chdir(name) == -1) {
        fprintf(stderr, "Cannot change directory to \"%s\", errno = %d.", name, errno);
        exit(-1);
    }
}

static struct _finddata_t *safe_malloc_finddata(int count)
{
    struct _finddata_t *p_new = malloc(count * sizeof(struct _finddata_t));
    if (p_new != NULL) {
        return p_new;
    }
    fprintf(stderr, "Not enough memory!\n");
    exit(-1);
}

static int compare_file(const struct _finddata_t *e1, const struct _finddata_t *e2)
{
    if (is_dir(e1)) {
        if (is_dir(e2)) {
            return _stricmp(e1->name, e2->name);
        } else {
            return -1;
        }
    } else {
        if (is_dir(e2)) {
            return 1;
        }
    }
    return _stricmp(e1->name, e2->name);
}

static void display_and_search(struct _finddata_t *p_file)
{
    int i;
    if (strcmp(p_file->name, ".") == 0 || strcmp(p_file->name, "..") == 0) {
        return;
    }
    if (is_dir(p_file)) {
        for (i = 0; i < g_depth; i++) {
            putchar('\t');
        }
        printf("%s\n", p_file->name);
        safe_chdir(p_file->name);
        g_depth++;
        search_file();
        safe_chdir("..");
        g_depth--;
    } else if (g_show_file) {
        for (i = 0; i < g_depth; i++) {
            putchar('\t');
        }
        printf("%s", p_file->name);
        if (g_show_size) {
            printf("  %10lu", p_file->size);
        }
        if (g_show_time) {
            char buf[TIME_BUF_LEN];
            struct tm *s_tm = localtime(&p_file->time_write);
            strftime(buf, TIME_BUF_LEN, TIME_FORMAT, s_tm);
            printf("  %s", buf);
        }
        putchar('\n');
    }
}

/* Search for all files, including sub directories (recursively) */
static void search_file()
{
    int i;
    struct _finddata_t *p_file;
    struct _finddata_t file;
    intptr_t h_file;
    int max_count = FILE_COUNT_BLOCK;
    int count = 0;
    p_file = safe_malloc_finddata(max_count);
    h_file = _findfirst("*", p_file + count);
    if (h_file == -1L) {
        printf("No files in this directory.\n");
    } else {
        ++count;
        while (_findnext(h_file, p_file + count) == 0) {
            ++count;
            if (count >= max_count) {
                max_count += max_count;
                struct _finddata_t *p_new = safe_malloc_finddata(max_count);
                memcpy(p_new, p_file, count * sizeof(struct _finddata_t));
                free(p_file);
                p_file = p_new;
            }
        }
        if (errno != ENOENT) {
            fprintf(stderr, "Error occurred, errno = %d.", errno);
            exit(-1);
        }
        qsort(p_file, count, sizeof(struct _finddata_t), compare_file);
        for (i = 0; i < count; i++) {
            display_and_search(&p_file[i]);
        }
    }
    _findclose(h_file);
    free(p_file);
}

int main(int argc, char *argv[])
{
    int i;
    int get_path = 0;
    char buf[256];
    if (_getcwd(buf, sizeof(buf) - 1) == NULL) {
        fprintf(stderr, "Cannot get current directory, errno = %d.", errno);
        exit(-1);
    }
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 's':
            case 'S':
                g_show_size = 1;
                g_show_file = 1;
                break;
            case 't':
            case 'T':
                g_show_time = 1;
                g_show_file = 1;
                break;
            case 'f':
            case 'F':
                g_show_file = 1;
                break;
            case '?':
                show_help();
                return 0;
                break;
            default:
                return -1;
                break;
            }
        } else if (!get_path) {
            if (_chdir(argv[i]) != 0) {
                fprintf(stderr, "Invalid path %s!\n", argv[i]);
                return -1;
            }
            get_path = 1;
        }
    }
    search_file();
    safe_chdir(buf);
    return 0;
}
