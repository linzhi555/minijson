#pragma once
#include <stddef.h>
#include <stdint.h>
// generate:minijson
struct Student {
    int student_id;
    char* name;
    unsigned int age;
    float height;
    double weight;
};

// generate:minijson
struct Teacher {
    int64_t teacher_id;
    char* name;
    size_t age;
};
