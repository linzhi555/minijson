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
