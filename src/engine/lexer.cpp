#include <iostream>
#include <string>
#include <core.h>
#include <lexer.h>

static void _get_name(std::string& data) {
    for (auto &ch : data) {
        
    }
};

extern "C" vector* tokenize(char* data) {
    vector* tokens = vector_new();
    std::string str(data);

    return tokens;
}