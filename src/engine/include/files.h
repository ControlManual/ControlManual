#ifndef __ENGINE_FILES_H__
#define __ENGINE_FILES_H__

#include <stdbool.h>

char* home();
void create_directory(const char* path);
void create_file(const char* path);
void create_file_with_contents(const char* path, const char* contents);
bool directory_exists(const char* path);
const char* join_paths(const char* path_a, const char* path_b);

#endif