#!python3

import sys
import re
import os
from typing import List, Tuple
from dataclasses import dataclass


def panic(err: str):
    print(err)
    sys.exit(1)


@dataclass
class CStructField:
    """the field of a struct"""
    name: str
    ctype: str


@dataclass
class CStruct:
    """the c struct definition"""
    name: str
    fields: List[CStructField]


c_int_types = [
    "char",
    "short",
    "int",
    "int64_t",
    "long",
    "long long",
]
c_int_types = c_int_types + ["unsigned "+t for t in c_int_types]
c_int_types = c_int_types + ["const "+t for t in c_int_types]


c_float_types = [
    "double",
    "float",
]
c_float_types = c_float_types + ["const "+t for t in c_float_types]
print(c_float_types)

c_str_types = [
    "char*",
    "const char*",
]


def _type_to_func(ctype: str) -> str | None:
    if ctype in c_int_types:
        return "jmap_set_int"

    elif ctype in c_float_types:
        return "jmap_set_float"

    elif ctype in c_str_types:
        return "jmap_set_str"

    elif ctype in ["bool"]:
        return "jmap_set_bool"

    return None


def generate_code(structs: List[CStruct]) -> Tuple[List[str], List[str]]:
    """
    give some structs definitions then generate the convertion code
    return (hfile,cfile)
    hfile means the content of .h file, cfile means the content .c file
    """
    hfile: List[str] = []
    cfile: List[str] = []

    hfile.append('#include "minijson.h"')
    cfile.append('#include "minijson.h"')

    for stct in structs:

        temp = "void {n}_to_json(JsonMap* dst,struct {n}* t)"\
            .format(n=stct.name)
        hfile.append(temp+";")

        cfile.append(temp+"{")
        for field in stct.fields:
            f = _type_to_func(field.ctype)
            if f is None:
                panic("can not find convert func for " + field.ctype)

            cfile.append(
                '    '+'{f}(dst,"{n}", t->{n});'.format(f=f, n=field.name))

        cfile.append("}")

        # temp = "void {n}_from_json(struct {n} *t, JsonMap* dst)"\
        #    .format(n=stct.name)
        # cfile.append(temp+"{")
        # cfile.append("}")

        # hfile.append(temp+";")

    return (hfile, cfile)


def parse_struct_name(s: str) -> str | None:
    items = s.split()
    print()
    if items[0] != "struct":
        return None
    if items[2] != "{":
        return None
    return items[1]


def parse_struct_one_field(s: str) -> CStructField | None:
    s = s.rstrip()
    s = s.rstrip(";")

    items = s.split()
    if len(items) < 2:
        return None
    name = items[len(items)-1]

    ctype = items[0]
    for i in range(1, len(items)-1):
        ctype += " " + items[i]

    if name[0:1] == '*':
        name = name[1:len(name)]
        ctype += "*"

    return CStructField(name=name, ctype=ctype)


def remove_comments(code: List[str]):
    res = []
    for line in code:
        if not re.match(r"\s*//.*", line):
            res.append(line)
    return res


def parse_cstruct(code: List[str]) -> CStruct | None:
    code = remove_comments(code)
    if len(code) < 2:
        return None

    res = CStruct("", [])

    # parse struct name
    name = parse_struct_name(code[0])
    if name is None:
        return None
    res.name = name

    # parse_struct_one_field
    for line in code[1:len(code)-1]:
        field = parse_struct_one_field(line)
        if field:
            res.fields.append(field)
        else:
            return None
    if code[len(code)-1].split()[0] != "};":
        return None
    return res


def exact_target_blocks(code: List[str]) -> List[List[str]]:
    blocks: List[List[str]] = []
    curBlock: List[str] = []
    for line in code:
        if len(curBlock) != 0:
            curBlock.append(line)
            if re.match(r"};", line):
                blocks.append(curBlock)
                curBlock = []
        elif re.match(r"\s*//\s*generate:\s*minijson\s*", line):
            curBlock.append(line)
    return blocks


def save_file(filename: str, lines: List[str]):
    with open(filename, "w") as file:
        for line in lines:
            file.write(line + os.linesep)  # 使用系统特定的换行符


if __name__ == "__main__":
    if len(sys.argv) <= 1:
        print("Usage：mjgen.py xxx.h")
        exit(1)
    filename = sys.argv[1]

    f = open(filename)
    code = f.readlines()
    code = [line.rstrip("\n") for line in code]

    blocks = exact_target_blocks(code)
    structs = []
    for block in blocks:
        s = parse_cstruct(block)
        if s is None:
            print("parse error when parse:")
            for line in block:
                print(line)
            sys.exit(1)
        print(s)
        structs.append(s)
    hfile, cfile = generate_code(structs)

    hout = "test.generated.h"
    cout = "test.generated.c"

    save_file(hout, hfile)
    save_file(cout, cfile)
