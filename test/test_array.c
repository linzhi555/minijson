#include <assert.h>
#include <stdio.h>
#include "minijson.h"

const char* teststr = "[1,2,3,4.0, 11,12]";

int main() {
    JsonArray arr;
    init_jarray(&arr);

    JsonStr err;
    init_jstr(&err);
    if (minijson_parse_array(&arr, teststr, &err) != 0) {
        printf("error when parse array: %s\n", err.data);
    }

    JsonStr output;
    init_jstr(&output);

    jarray_output(&output, &arr, 0);
    printf("%s\n", output.data);

    printf("test array SUCCESS\n");
    printf("=========================\n");

    free_jstr(&output);
    free_jstr(&err);
    free_jarry(&arr);
}
