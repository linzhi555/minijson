#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "minijson.h"
#include "type.h"
#include "type.h.generated.h"

int main() {
    JsonStr out;
    init_jstr(&out);
    JsonMap map;
    init_jmap(&map);

    printf("-------------------------\n");

    struct Student stu1 = { .student_id = 100001, .name = "z3", .age = 11, .weight = 50.5, .height = 170.1 };
    Student_to_json(&map, &stu1);
    jmap_output(&out, &map, 0);
    printf("%s\n", out.data);

    printf("-------------------------\n");

    struct Student stu2;
    int err = Student_from_json(&stu2, &map);
    if (err != 0) {
        fprintf(stderr, "map to struct fail error code %d\n", err);
        return -1;
    }
    assert(stu2.student_id == stu1.student_id);
    assert(stu2.weight == stu1.weight);
    assert(strcmp(stu1.name, stu2.name) == 0);
    printf("%d %s %d %f %f\n", stu2.student_id, stu2.name, stu2.age, stu2.weight, stu2.height);

    printf("-------------------------\n");

    free_jmap(&map);
    init_jmap(&map);
    free_jstr(&out);
    init_jstr(&out);

    struct Class c1;
    c1.s = stu2;
    c1.t.age = 50;
    c1.t.name = "mike";
    c1.t.teacher_id = 324;

    Class_to_json(&map, &c1);
    jmap_output(&out, &map, 0);
    printf("%s\n", out.data);

    struct Class c2;
    Class_from_json(&c2, &map);

    assert(c2.t.age == c1.t.age);
    assert(c2.s.height == c1.s.height);
    assert(strcmp(c2.t.name, c1.t.name) == 0);

    printf("-------------------------\n");

    free_jmap(&map);
    free_jstr(&out);
    printf("test mjgen.py SUCCESS\n");
    printf("=========================\n");
}
