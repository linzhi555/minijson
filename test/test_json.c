#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "minijson.h"

#define MAX_SIZE 10240  // 定义数组的最大大小
char buffer[MAX_SIZE] = { 0 };

char files[10][1000] = { 0 };
int filenum = 0;

int scan_json_file(char* dirpath) {
    struct dirent* entry;
    DIR* dp = opendir(dirpath);

    if (dp == NULL) {
        perror("opendir");
        return EXIT_FAILURE;
    }

    while ((entry = readdir(dp)) != NULL) {
        // 排除 . 和 .. 目录
        if (entry->d_name[0] != '.') {
            sprintf(files[filenum], "%s/%s", dirpath, entry->d_name);
            filenum++;
        }
    }

    closedir(dp);
    return EXIT_SUCCESS;
}

int read_test_files(const char* filename) {
    FILE* file;
    size_t bytesRead;

    // 打开文件
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("无法打开文件");
        return 0;
    }

    // 读取文件内容到数组
    bytesRead = fread(buffer, sizeof(char), MAX_SIZE - 1, file);
    buffer[bytesRead] = '\0';  // 确保字符串以 null 结尾

    // 输出读取的内容
    printf("读取的内容:\n%s\n", buffer);

    // 关闭文件
    fclose(file);
    return bytesRead;
}

int test_str_json(const char* str) {
    JsonMap jmap;
    int ret = 0;
    init_jmap(&jmap);
    JsonStr err;
    init_jstr(&err);

    int res = minijson_parse_str(&jmap, str, &err);
    if (res == -1) {
        printf("parse error : %s \n", jstr_cstr(&err));
        ret = -1;
        goto final;
    } else {
        printf("parse succcess\n");
    }
    jmap_output(&jmap, 0);
final:
    free_jmap(&jmap);
    return ret;
}

int main() {
    printf("-----test1 start----\n");
    printf("minijson version :%s\n", minijson_version());

    scan_json_file("./jsons");

    for (int i = 0; i < filenum; i++) {
        const char* filename = files[i];
        printf("%s\n", filename);
        if (read_test_files(filename) == 0) {
            printf("json file test fail");
            return EXIT_FAILURE;
        }
        test_str_json(buffer);
    }

    return EXIT_SUCCESS;
}
