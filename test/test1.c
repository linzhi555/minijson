#include <stdio.h>
#include "minijson.h"

const char* str = "{\"age\":11 , \"name\":\"cisbest\", \"id\":\"1001\"} ";

int main() {
    printf("-----test1 start----\n");
    printf("minijson version :%s\n", minijson_version());
    JsonMap jmap;
    init_jmap(&jmap);
    char err[100] = { 0 };
    int res = minijson_parse_str(&jmap, str, err);
    if (res == -1) {
        printf("parse error : %s \n", err);
    } else {
        printf("parse succcess\n");
    }
    jmap_debug(&jmap);
}
