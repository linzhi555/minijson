#pragma once
#include <ctype.h>

#define LOG(...)                                        \
    printf("%s %s %d: ", __FILE__, __func__, __LINE__); \
    printf(__VA_ARGS__);                                \
    printf("\n")

int skip_space(const char *cs);
int find_space(const char *cs);

// return a const char* of n ' ', n < 255
const char *nspace(int n);
