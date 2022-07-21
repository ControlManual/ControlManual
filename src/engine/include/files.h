#ifndef __ENGINE_FILES_H__
#define __ENGINE_FILES_H__

#include <stdbool.h>
#include <engine.h>

#ifdef __cplusplus
extern "C" {
#endif
void create_directory(const char* path);
void create_file(const char* path);
void create_file_with_contents(const char* path, const char* contents);
bool directory_exists(const char* path);
const char* join_paths(const char* path_a, const char* path_b);

typedef void (*iter_func)(session*, char*);
void directory_iter_files(const char* path, iter_func fn, session* ses);
#ifdef __cplusplus
}
#endif

#endif