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


c_int_types = ["int64_t",
               "int",
               "uint64_t",
               "unsigned int",
               "size_t",
               "ssize_t",]

c_float_types = ["double","float"]

c_str_types = ["char*",]

c_bool_types = ["bool","_Bool"]


def _type_set_stmt(ctype: str, name: str) -> List[str] | None:
    stmts: str
    if ctype in c_int_types:
        stmts ='    jmap_set_int(dst, "{n}", (int64_t) t->{n});'.format(n=name)
    elif ctype in c_float_types:
        stmts = '    jmap_set_float(dst, "{n}", (double) t->{n});'.format(n=name)
    elif ctype in c_str_types:
        stmts = '    jmap_set_str(dst, "{n}", (const char*) t->{n});'.format(n=name)
    elif ctype in c_bool_types:
        stmts= '    jmap_set_str(dst, "{n}", (double) t->{n});'.format(n=name)
    else:
        return None
    return stmts.split("\n")


def _type_get_stmt(ctype: str, name: str) -> List[str] | None:
    stmts: str = ''
    if ctype in c_int_types:
        stmts =  'int64_t {}_temp;\n'.format(name)
        stmts += 'jmap_get_int(map, "{n}", &{n}_temp);\n'.format(n=name)

    elif ctype in c_float_types:
        stmts =  'double {}_temp;\n'.format(name)
        stmts += 'jmap_get_float(map, "{n}", &{n}_temp);\n'.format(n=name)

    elif ctype in c_str_types:
        stmts =  'char* {}_temp;\n'.format(name)
        stmts += 'jmap_get_str(map, "{n}", &{n}_temp);\n'.format(n=name)


    elif ctype in c_bool_types:
        stmts =  'bool {}_temp;\n'.format(name)
        stmts += 'jmap_get_bool(map, "{n}", &{n}_temp);\n'.format(n=name)

    else:
        return None

    stmts += 'if (err != 0) return err;\n'
    stmts += 'dst->{n} = ({t}) {n}_temp;'.format(t=ctype,n=name)

    ret = stmts.split('\n')
    ret = ["    " + l for l in ret]
    ret.append("")

    return ret


def _generate_toMapFunc(stct: CStruct) -> Tuple[List[str], List[str]]:

    hfile, cfile = [], []
    temp = "void {n}_to_json(JsonMap* dst,const struct {n}* t)".format(n=stct.name)
    hfile.append(temp+";")

    cfile.append(temp+"{")
    for field in stct.fields:
        stmts = _type_set_stmt(field.ctype, field.name)
        if stmts is None:
            panic("can not generate set statement for type: " + field.ctype)
        else:
            cfile += stmts

    cfile.append("}")
    return (hfile, cfile)


def _generate_fromMapFunc(stct: CStruct) -> Tuple[List[str], List[str]]:

    hfile, cfile = [], []
    temp = "int {n}_from_json(struct {n} *dst,const JsonMap* map)".format(n=stct.name)
    hfile.append(temp+";")

    cfile.append(temp+"{")
    cfile.append('    '+'int err = 0;')
    for field in stct.fields:
        stmts = _type_get_stmt(field.ctype, field.name)
        if stmts is None:
            panic("can not generate set statement for type: " + field.ctype)
        else:
            cfile += stmts

    cfile.append('    return 0;')
    cfile.append("}")
    return (hfile, cfile)


def generate_code(origin: str, structs: List[CStruct]) -> Tuple[List[str], List[str]]:
    """
    param origin: the original file where the structs definitions come from
    param structs: some structs definitions then generate the convertion code
    return: (hfile,cfile)
    hfile means the content of .h file, cfile means the content .c file
    """
    hfile: List[str] = []
    cfile: List[str] = []

    hfile.append('#pragma once')
    hfile.append('#include "minijson.h"')
    hfile.append('#include "{}"'.format(origin))

    cfile.append('#include "minijson.h"')
    cfile.append('#include "{}"'.format(origin))
    cfile.append('#include "{}.generated.h"'.format(origin))

    generates = [_generate_toMapFunc, _generate_fromMapFunc]
    for stct in structs:
        for gfuncs in generates:
            hAppends, cAppends = gfuncs(stct)
            hfile += hAppends
            cfile += cAppends

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


def save_file(filepath: str, lines: List[str]):
    with open(filepath, "w") as file:
        for line in lines:
            file.write(line + os.linesep)  # 使用系统特定的换行符


if __name__ == "__main__":
    if len(sys.argv) <= 1:
        print("Usage：mjgen.py xxx.h")
        exit(1)
    filepath = sys.argv[1]

    f = open(filepath)
    code = f.readlines()
    code = [line.rstrip("\n") for line in code]

    blocks = exact_target_blocks(code)
    structs = []
    for block in blocks:
        s = parse_cstruct(block)
        if s is None:
            print("parse error when parsing:")
            for line in block:
                print(line)
            sys.exit(1)
        print(s)
        structs.append(s)

    hfile, cfile = generate_code(os.path.basename(filepath), structs)

    hout = filepath + ".generated.h"
    cout = filepath + ".generated.c"

    save_file(hout, hfile)
    save_file(cout, cfile)
