#include "minijson.h"

// parse the char* to JsonNum
int jnum_from_cstr(JsonNum *num, const char *cs, size_t n) {
    int64_t ipart = 0;  // interger part 11.000203 's ipart is 11
    int64_t fpart = 0;  // float part , 11.000203 's fpart is 203
    int64_t dot0s = 0;  // zero nums after dot, 11.000203 's dot0s is 3

    char state = 's';  // s : start, i : interger , d: dot, f:float
    size_t offset;
    for (offset = 0; offset < n; offset++) {
        char c = cs[offset];
        if (state == 's') {
            if (isspace(c)) continue;
            if (!isdigit(c)) goto fail;
            ipart = c - '0';
            state = 'i';
            continue;
        }

        else if (state == 'i') {
            if (c == '.') {
                state = 'd';
                continue;
            }
            if (!isdigit(c)) break;
            ipart *= 10;
            ipart += c - '0';
            continue;
        }

        else if (state == 'd') {
            if (c == '0') {
                dot0s++;
                continue;
            } else if (isdigit(c)) {
                state = 'f';
                fpart = c - '0';
                continue;
            }
            break;
        }

        else if (state == 'f') {
            if (!isdigit(c)) break;
            fpart *= 10;
            fpart += c - '0';
            continue;
        }
    }

    if (state == 'i') {
        num->isInt = true;
        num->Int64 = ipart;
    } else if (state == 'f' || (state == 'd' && dot0s > 0)) {
        num->isInt = false;
        num->Double = ipart;
        int fpartlen = 0;
        for (size_t temp = fpart; temp != 0; temp /= 10) fpartlen++;
        double res = fpart;
        for (int i = 0; i < dot0s + fpartlen; i++) {
            res /= 10;
        }
        num->Double += res;
    } else {
        goto fail;
    }
    return offset;
fail:
    return 0;
}
