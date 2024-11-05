#include <stdio.h>
#include "minijson.h"
#include "test.generated.h"
#include "type.h"

int main() {
    struct Student stu = { .stddent_id = 100001, .name = "z3", .age = 11, .weight = 50.5, .height = 170.1 };
    JsonMap map;
    init_jmap(&map);
    Student_to_json(&map, &stu);
    JsonStr out;
    init_jstr(&out);
    jmap_output(&out, &map, 0);
    printf("%s\n",out.data);
}
