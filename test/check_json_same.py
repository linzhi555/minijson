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

for basename in basenames:
    iName = os.path.join(srcDir, basename)
    oName = os.path.join(dstDir, basename)
    with open(iName, 'r') as infile:
        with open(oName, 'r') as outfile:
            srcMap = json.load(infile)
            dstMap = json.load(outfile)
            print("srcMap from {} :".format(iName))
            print(srcMap)
            print("dstMap from {} :".format(oName))
            print(dstMap)
            issame = srcMap == dstMap
            print("srcMap == dstMap is", issame)
            if not issame:
                sys.exit(1)
            print("========================================")
