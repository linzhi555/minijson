#!python3
import json
import sys
import os

# TODO design a more complete test


def run_test(fname):
    res = os.system("./test1 {}".format(fname))
    if res != 0:
        print(fname, "test fail")
    else:
        print(fname, "test success")


if len(sys.argv) < 2:
    print("usage: test.py xxx.json")
    sys.exit(-1)
infile = sys.argv[1]

run_test(infile)
with open(infile, 'r') as file:
    data = json.load(file)
    print(data)
