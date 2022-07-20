#include <stdlib.h>
#include <fstream>

#if __cplusplus < 201703L
#include <experimental/filesystem>
    namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
    namespace fs = std::filesystem;
#endif

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
    fs::path dir(path_a);
    fs::path file(path_b);

    return (dir / file).c_str();
}