#include <stdio.h>
#include <stdlib.h>
#include "minijson.h"

#define MAX_SIZE 10240  // 定义数组的最大大小
char buffer[MAX_SIZE] = { 0 };

int read_test_files(const char* filename) {
    FILE* file;
    size_t bytesRead;

    // 打开文件
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("无法打开文件");
        return -1;
    }

    // 读取文件内容到数组
    bytesRead = fread(buffer, sizeof(char), MAX_SIZE - 1, file);
    buffer[bytesRead] = '\0';  // 确保字符串以 null 结尾

    // 输出读取的内容
    printf("读取的内容:\n%s\n", buffer);

    // 关闭文件
    fclose(file);
    return 0;
}

int test_str_json(const char* str) {
    JsonMap jmap;
    int ret = 0;
    init_jmap(&jmap);
    char err[100] = { 0 };
    int res = minijson_parse_str(&jmap, str, err);
    if (res == -1) {
        printf("parse error : %s \n", err);
        ret = -1;
        goto final;
    } else {
        printf("parse succcess\n");
    }
    jmap_debug(&jmap);
final:
    free_jmap(&jmap);
    return ret;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage test xxx.json");
    }
    char* filename = argv[1];

    printf("-----test1 start----\n");
    printf("minijson version :%s\n", minijson_version());

    if (read_test_files(filename)) {
        printf("json file test fail");
        return EXIT_FAILURE;
    }
    test_str_json(buffer);
    return EXIT_SUCCESS;
}
