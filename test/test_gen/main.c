#include <stdio.h>
#include "minijson.h"

#include "type.h"
#include "type.h.generated.h"

int main() {
    struct Student stu = { .student_id = 100001, .name = "z3", .age = 11, .weight = 50.5, .height = 170.1 };
    JsonMap map;
    init_jmap(&map);
    Student_to_json(&map, &stu);
    JsonStr out;
    init_jstr(&out);
    jmap_output(&out, &map, 0);
    printf("%s\n", out.data);

    struct Student stu2;
    int err = Student_from_json(&stu2, &map);
    if (err != 0) {
        fprintf(stderr, "map to struct fail error code %d\n", err);
        return -1;
    }
    free_jmap(&map);

    printf("%ld %s %ld %f %f\n", stu2.student_id, stu2.name, stu2.age, stu2.weight, stu2.height);

    printf("test mjgen.py SUCCESS\n");
    printf("=========================\n");
}
