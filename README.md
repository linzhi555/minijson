# minijson 一个小型的c语言json解析库

## 特性：
轻量，无外部依赖,库编译大小小于100K。
将错误视为值,而不是全局错误,使用内置的JsonStr来进行错误处理传递，
丰富的Json类型操作API，包括对map，array，string的操作。
好用的JsonStr类型，包括可以将序列化的结果存在不固定长度的jsonStr中。

## 编译

编译静态库
```
    make
```

编译测试单元
```
cd test && test
```
