#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "minijson.h"

long get_file_size(const char* filename) {
    FILE* file = fopen(filename, "rb");  // 以二进制模式打开文件
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    // 将文件指针移动到文件末尾
    fseek(file, 0, SEEK_END);
    // 获取文件指针的位置，即文件大小
    long size = ftell(file);

    // 关闭文件
    fclose(file);

    return size;
}

#define FAIL(x) \
    ret = x;    \
    goto finish

// mjfmt is a simple tool that that can format any json
int main(int argc, char** argv) {
    int ret = 0;
    if (argc < 2) {
        printf("Usage: mjfmt xxx");
    }
    char* filename = argv[1];

    JsonValue val;
    init_jvalue_null(&val);

    char* buffer = NULL;
    int size = get_file_size(filename);
    if (size < 0) {
        FAIL(1);
    }
    buffer = calloc(size, sizeof(char) + 1);
    FILE* file = fopen(filename, "r");
    if (fread(buffer, sizeof(char), size, file) == 0) {
        perror("Error fail reading file");
        FAIL(1);
    }

    JsonStr err;
    init_jstr(&err);

    if (minijson_parse_any(&val, buffer, &err) != 0) {
        printf("error when parse json: %s\n", err.data);
    }

    JsonStr output;
    init_jstr(&output);

    jvalue_output(&output, &val, 0);
    printf("%s\n", output.data);

finish:
    free(buffer);
    free_jstr(&output);
    free_jstr(&err);
    free_jvalue(&val);
    return ret;
}
