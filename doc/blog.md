# [原创] minijson： 手搓一个c语言json库。

最近的业余时间，我在用c语言写一个国际象棋的小游戏，
在过程中，我想要将我的游戏状态保存到硬盘里，以供下一次读取。
但是呢用什么方式保存游戏状态呢，我打算使用json格式，
不在乎开发速度，因此需要一个c语言的json库，因为是业余项目，因此我先打算自己手搓一个json库，
来实现结构转json文件，以及从json文件转结构体。
项目：https://github.com/linzhi555/minijson


先介绍一下json格式，json是一种数据交换格式，也是一种标记语言
在我看来json是一种很麻烦的标记语言，key值一定要双引号，不能有注释，不能有多余的逗号。
但是json有一个特点，它真的太流行了,到处都用json格式，什么语言都用，
甚至vscode还用这个json当配置文件。。。。

json格式其实非常简单，他有4种原子类型，null类型，bool类型，Num类型，String类型。
以及2种复合类型，Array和Object。
Array 是数组类型，其中可以包括任意类型，
Object 是对象类型，本质就是map，包含一系列的键值对， 键是字符串，值是可以使任意类型。
PS,我觉得Object名字很奇怪，其他的类型就不是对象吗？因此在我的库里我就叫它map，因为它就是键值对的组合。

Json库的任务就是将一段字符串转换成一个Json Object对象树，
但在一切开始前我们需要先了解词法分析这一步，词法分析我个人理解其为是一种断句分析，
给定一个字符串，进行分析，这其实很复杂的，因为有格式问题，比如有两个json，
"{"a":11}" 以及 "{ "a" :11   }",这两个字符串是不同的，但其实的含义是一样的，只是空格不同。
但是经过词法分析后呢，这两个就被统一的转换为了 { , "a" , ":" , 11 ,}这5个Token，
两个字符串的词法分析后的结果是一样的，
而且token是有类型的，11是num token，“a”是字符类型。
有了lexer方便多了！就不用在后面的解析中处理什么空格啊，字符串解析啊，数字解析之类的很烦的事。

在本项目中使用lexer负责将json的字符流转换为token流。
下面是lexer的定义，在本项目中，lexer是这么一个工具，给它一个原始字符串，我们使用lexer_peek
就能得到lexer当前看到的token,使用lexer_next就可让它的指针跳到下一个token开始。
lexer的使用可以看出是一个token流，可以不断的从token流中取出一个token。


我们的任务改变了，现在的任务是给定一个lexer，通过消耗这个lexer的token流来构造一颗json object树。
为了完成这个任务，我要介绍一组形如parse_XXX函数的函数。XXX是类型。
该类型的函数可以消耗lexer，来解析一个XXX，如果解析失败，那么lexer将不被消耗。如果成功那么lexer就会到达新的位置。
这样的函数有好几个，
parse_map,
parse_map_fields,
parse_array,
parse_value,
这样的设计原因是，这些parse函数不是孤立的，它们是互相引用的，根据逻辑形成递归结构。
比如我们可以用parse_map来消耗lexer形成一个map，
那么如果有一个parse_map(map,lexer)就能解析完成我们的任务。而写一个map是很难的，
因为map里面可能有子map，子map里面可能还有map。

我们可以通过递归，来简化任务。
比如一个map有三部分组成，map = { + N个kv字段 + },因此它我们分成三个阶段来解析。

parse_map的伪代码可以是

```
int parse_map(map,lexer){
    if lexer_peek() 不是 { 那么 return 解析失败

    while(true){
        如果 parse_map_filed(map,lexer) 失败{
            结束循环
        }
    }

    if lexer_peek() 是 } 那么 return 解析失败
}
```
经过上面的函数，我们将一个大任务（解析map）分解成了三部分，
其中我们使用一个循环来解析不定长的元素。
其中parse_map_field是关键。
而一个解析 map_field也可以分解成三部分，解析key，解析 ：，解析value，
map_field = key + : + value

解析 key 和 ： 很简单只需要判断lexer的token是不是string 类型 以及
colon类型就行。而解析value，就稍微复杂了。
因为一个value是可以是多类型
value = null 或 num 或 string 或 bool 或 array 或 map
那么该怎么用代码实现这个逻辑呢，
```
int parse_value(value,lexer){
    if parse_null(value,lexer) 成功 那么 return 成功
    if parse_bool(value,lexer) 成功 那么 return 成功
    if parse_num(value,lexer) 成功 那么 return 成功
    if parse_string(value,lexer) 成功 那么 return 成功
    if parse_array(value,lexer) 成功 那么 return 成功
    if parse_map(value,lexer) 成功 那么 return 成功

    return 解析失败
}
```
可以看到对于解析某种类型包含或逻辑的类型的成员，那么解析的时候，我们可以一个个类型试过去，只要有一种类型成功，那么解析就成功。

















