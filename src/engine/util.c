#include <engine/util.h>
#include <core/util.h> // safe_malloc, FUNCTYPE
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <core/error.h>

#ifdef PLATFORM_POSIX
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#define SEPERATOR '/'
#else
#include <windows.h>
#include <io.h>
#define F_OK 0
#define access _access
#define SEPERATOR '\\'
#endif

#define ITERDIR_ERR char* str = safe_malloc(27 + strlen(path)); \
        sprintf(str, "couldn't open directory '%s'", path); \
        THROW_HEAP(str, "<native code>"); \
        return false;

char* char_to_string(char c) {
    char* str = safe_malloc(2);
    str[0] = c;
    str[1] = '\0';
    return str;
}


bool exists(char* path) {
    return access(path, F_OK) == 0;
}

char* home(void) {
#ifdef PLATFORM_POSIX
    struct passwd* p = getpwuid(getuid());
    if (p) return p->pw_dir;
    char* d = getenv("HOME");
    if (!d) FAIL("failed to acquire home directory");
    return d;
#else
    char* d = getenv("USERPROFILE");
    if (!d) FAIL("failed to acquire home directory");
    return d;
#endif
}

char* cat_path(char* a, char* b) {
    char* result = malloc(strlen(a) + strlen(b) + 2);
    sprintf(result, "%s%c%s", a, SEPERATOR, b);
    return result;
}

void create_dir(char* path) {
#ifdef PLATFORM_POSIX
    if (mkdir(path, 0777) == -1) FAIL(
        "failed to create directory"
    );
#else
    if (!CreateDirectory(path, NULL)) FAIL(
        "failed to create directory"
    ); 
#endif
}

char* read_file(char* path) {
    FILE* f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* string = safe_malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    return string;
}

bool iterate_dir(char* path, dir_iter_func func) {
#ifdef PLATFORM_POSIX
    DIR* dir;
    struct dirent* ent;
    if ((dir = opendir(path)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            char* p = cat_path(path, ent->d_name);
            func(p);
            free(p);
        }
        closedir(dir);
        return true;
    } else {
        ITERDIR_ERR;
    }
#else
    WIN32_FIND_DATA data;
    HANDLE find = FindFirstFile(path, &data);
    if (find == INVALID_HANDLE_VALUE) {
        ITERDIR_ERR;
    } 

    do {
        func(data.cFileName);
    } while (FindNextFile(find, &data) != 0);

    FindClose(find);
#endif
}

/* This assumes that the path exists. */
bool is_file(char* path) {
#ifdef PLATFORM_POSIX
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
#else
    DWORD attributes = GetFileAttributesA(path);
    return !(attributes & FILE_ATTRIBUTE_DIRECTORY)
#endif
}