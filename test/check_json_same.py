#!python3
import json
import os
import glob
import sys

srcDir = "./jsons"
dstDir = "./target"

# 使用 glob 模块找到所有的 JSON 文件
json_files = glob.glob(os.path.join(srcDir, '*.json'))
# 提取并返回基本名称
basenames = [os.path.basename(file) for file in json_files]


class ParseResult:
    def __init__(self):
        self.isErr: bool = True
        self.err: str = ""
        self.some: dict = {}

    def __eq__(self, o):
        if self.isErr is True and o.isErr is True:
            return True
        if self.isErr != o.isErr:
            return False
        return self.some == o.some

    def __str__(self):
        return "isErr:{} , some: {}".format(self.isErr, self.some)

    @staticmethod
    def error(e: str):
        res = ParseResult()
        res.isErr = True
        res.err = e
        return res

    @staticmethod
    def some(d: dict):
        res = ParseResult()
        res.isErr = False
        res.some = d
        return res


def parse_json_file(fp: str) -> ParseResult:
    try:
        with open(fp, 'r') as file:
            obj = json.load(file)
            return ParseResult.some(obj)
    except json.JSONDecodeError:
        return ParseResult.error("FailParseJson")
    except FileNotFoundError:
        return ParseResult.error("FailOpenFile")


for basename in basenames:
    iName = os.path.join(srcDir, basename)
    oName = os.path.join(dstDir, basename)
    srcRes = parse_json_file(iName)
    dstRes = parse_json_file(oName)

    print("checking srcMap : {} and dstMap : {} :".format(iName, oName))
    issame = srcRes == dstRes
    print("srcMap == dstMap is", issame)
    if not issame:
        print("-------------------------------")
        print("-------------------------------")
        print("TEST FAIL when compare parse result ",
              iName, oName, "diff:")

        print("-------------", iName, "-----------------")
        print(srcRes)
        print("-------------", oName, "----------------")
        print(dstRes)
        sys.exit(1)
    print("========================================")
