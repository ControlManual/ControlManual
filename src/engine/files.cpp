#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cstring>

namespace fs = std::filesystem;

extern "C" char* home() {
    #ifdef _WIN32
        return getenv("HOMEPATH");
    #else
        return getenv("HOME");
    #endif
}

extern "C" void create_directory(const char* path) {
    fs::create_directory(path);
}

extern "C" void create_file(const char* path) {
    std::ofstream tmp(path);
    tmp.close();
}

extern "C" void create_file_with_contents(const char* path, const char* contents) {
    std::ofstream f(path);
    f << contents;
    f.close();
}

extern "C" bool directory_exists(const char* path) {
    return fs::exists(path) && fs::is_directory(path);
}

extern "C" const char* join_paths(
    const char* path_a,
    const char* path_b
) {
    fs::path a { path_a };
    fs::path b { path_b };
    
    std::string str = (a / b).string();
    char* new_str = new char[str.size() + 1];
    std::strcpy(new_str, str.c_str());
    // TODO: make this better

    return new_str;
}