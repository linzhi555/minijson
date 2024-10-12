#include <stdio.h>
#include "minijson.h"

const char* str = "{\"name\":\"cisbest\"},\"id\":\"1001\"";

int main() {
    printf("-----test1 start----\n");
    printf("minijson version :%s\n", minijson_version());
    JsonMap jmap;
    int res = minijson_parse_str(&jmap, str);
    printf("parse length: %d\n", res);
}
