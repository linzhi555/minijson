#pragma once
#include <stdint.h>
// generate:minijson
struct Student {
    int64_t student_id;
    char* name;
    int64_t age;
    double height;
    double weight;
};

// generate:minijson
struct Teacher {
    int64_t teacher_id;
    char* name;
    int64_t age;
};
