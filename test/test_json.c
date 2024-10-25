#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minijson.h"

#define MAX_SIZE 1024 * 1024 * 400  // 定义文件的最大大小 N GB
#define MAX_FILES_NUM 100
#define MAX_FILE_NAME_LEN 1000

char* buffer;

char files[MAX_FILES_NUM][MAX_FILE_NAME_LEN] = { 0 };
int filenum = 0;

// read file of filename to buffer
// return the how many file read
int scan_json_file(char* dirpath) {
    struct dirent* entry;
    DIR* dp = opendir(dirpath);

    if (dp == NULL) {
        return -1;
    }

    while ((entry = readdir(dp)) != NULL) {
        // 排除 . 和 .. 目录
        if (entry->d_name[0] != '.') {
            sprintf(files[filenum], "%s/%s", dirpath, entry->d_name);
            filenum++;
        }
    }

    closedir(dp);
    return filenum;
}

// read file of filename to buffer
// return the bytes read from file
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

// return -1 if fail, return 0 if succcess
static int _str_saveas(const char* str, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        return -1;
    }
    fprintf(file, "%s\n", str);
    fclose(file);
    return 0;
}

// read file of filename to buffer
// return the bytes read from file
static int test_str_json(const char* str, const char* outfile) {
    JsonMap jmap;
    init_jmap(&jmap);

    JsonStr err;
    init_jstr(&err);
    JsonStr out;
    init_jstr(&out);

    int parse_fail = minijson_parse_str(&jmap, str, &err);

    if (parse_fail != 0) {
        printf("parse error: %s \n", jstr_cstr(&err));
        jstr_cpy(&out, &err);  // output error info when fail
    } else {
        jmap_output(&out, &jmap, 0);  // // serialze the map when parse success
        printf("%s", jstr_cstr(&out));
        printf("parse succcess\n");
    }

    int save_err = _str_saveas(jstr_cstr(&out), outfile);
    if (save_err != 0) {
        printf("save file fail: %s\n", outfile);
    }

    free_jstr(&out);
    free_jstr(&err);
    free_jmap(&jmap);
    return parse_fail;
}

const char* basename(const char* raw) {
    for (int i = strlen(raw) - 1; i >= 0; i--) {
        if (raw[i] == '/' || raw[i] == '\\') {
            return &raw[i + 1];
        }
    }
    return raw;
}

int main() {
    int ret = 0;
    buffer = calloc(MAX_SIZE, sizeof(char));

    printf("-----test1 start----\n");
    printf("minijson version :%s\n", minijson_version());

    scan_json_file("./jsons");

    for (int i = 0; i < filenum; i++) {
        const char* infile = files[i];
        printf("\nconverting...: %s\n", infile);
        if (read_test_files(infile) == 0) {
            printf("json file test fail");
            ret = EXIT_FAILURE;
            goto final;
        }
        char outfile[MAX_FILE_NAME_LEN + 20] = { 0 };
        snprintf(outfile, MAX_FILE_NAME_LEN + 20, "./target/%s", basename(infile));
        test_str_json(buffer, outfile);
    }

final:
    free(buffer);
    return ret;
}
