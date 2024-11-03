#include "miniutils.h"
#include <stdio.h>
#include <string.h>
// skip empty, return the index of first Non-empty
int skip_space(const char *cs) {
    for (int i = 0; cs[i] != '\0'; i++) {
        if (!isspace(cs[i])) return i;
    }
    return 0;
}

// return the index of fist empty char
int find_space(const char *cs) {
    for (int i = 0; cs[i] != '\0'; i++) {
        if (isspace(cs[i])) return i;
    }
    return 0;
}

// return the index of char that can not be parse of number
int find_no_dot_or_digit(const char *cs) {
    int i;
    for (i = 0; cs[i] != '\0'; i++) {
        if (!isdigit(cs[i]) && cs[i] != '.') return i;
    }
    return i;
}

const char *spaces =
    "                                                   \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        \
                                                        ";

const char *nspace(int n) {
    if (n > (int) strlen(spaces)) {
        return spaces;
    }

    return spaces + (strlen(spaces) - n);
}
