# 我的第一个项目（仅用来自学）

跟学大神Milo Yip的json-tutorial



# 我从中学到了什么？

## 1、什么是json：

是一个用于数据交换的文本格式，虽然 JSON 源至于 JavaScript 语言，但它只是一种数据格式，可用于任何编程语言。现时具类似功能的格式有 XML、YAML，当中以 JSON 的语法最为简单。

json为树状结构，只包含6种数据类型：

- null：表示为null
- Boolean：true或false
- number：浮点数表示
- string：双引号表示——" "
- array：中括号表示——[]
- object：花括号表示——{}

json库提供的作用：

1. 把 JSON 文本解析为一个树状数据结构（parse）。
2. 提供接口访问该数据结构（access）。
3. 把数据结构转换成 JSON 文本（stringify）。

![](D:\C++从入门到入土\面试\PROJECTS\json\Cmode\images\requirement.png)



## 2、测试驱动开发（TDD）

1. 加入一个测试。
2. 运行所有测试，新的测试应该会失败。
3. 编写实现代码。
4. 运行所有测试，若有测试失败回到3。
5. 重构代码。
6. 回到 1。

先设计API，然后写部分测试代码，再写满足测试的实现。



把一个 JSON 解析，然后再生成另一 JSON，逐字符比较两个 JSON 是否一模一样。这种测试可称为往返（roundtrip）测试。但需要注意，同一个 JSON 的内容可以有多种不同的表示方式，例如可以插入不定数量的空白字符，数字 `1.0` 和 `1` 也是等价的。所以另一种测试方式，是比较两次解析的结果（`lept_value` 的树）是否相同



## 3、宏编写技巧

宏里有多过一个语句（statement），就需要用 `do { /*...*/ } while(0)` 包裹成单个语句。

### 为什么使用宏而不用函数或内联函数？

因为这个测试框架使用了 `__LINE__` 这个编译器提供的宏，代表编译时该行的行号。如果用函数或内联函数，每次的行号便都会相同。另外，内联函数是 C99 的新增功能，本教程使用 C89。



## 4、浮点数（double）存储方式

IEEE标准制定浮点数：一种科学计数法，用符号、指数和尾数来表示，底数定为2——即把一个浮点数表示为尾数乘以2的指数次方再添上符号

|        | 符号位 s | 阶码（指数） e | 尾数 b | 长度 |
| ------ | -------- | :------------- | ------ | ---- |
| float  | 1        | 8              | 23     | 32   |
| double | 1        | 11             | 52     | 64   |

double计算：(-1)^s * 1.b * 2^(e-1023)

1.b是二进制表示的，若要表示为10进制，则1+2^(-1)+2^(-2)+......

double的指数可以为负，所以范围是-1022~1023，为了便于计算，阶码没有符号位，当解析double类型时，需要减去一个偏移量1024。

![double](D:\C++从入门到入土\面试\PROJECTS\json\Cmode\images\double.jpg)



## 5、内存管理

#include<stdlib.h>

malloc()	realloc()	free()

memcpy(dst,src,len)：不能解决src dst重叠问题

memmove()：可解决重叠问题，但有额外开销；不重叠情况下，与memcpy()作用相同



内存泄露检测方法：

valgrind --leak-check=full



## 6、unicode

每个字符映射至一个整数码点，范围是0至0x10FFFF，码点通常记作U+XXXX（XXXX为十六进位数）

存储码点的方式称为UTF；现时流行的 UTF 为 UTF-8、UTF-16 和 UTF-32。每种 UTF 会把一个码点储存为一至多个编码单元（code unit）。例如 UTF-8 的编码单元是 8 位的字节、UTF-16 为 16 位、UTF-32 为 32 位。除 UTF-32 外，UTF-8 和 UTF-16 都是可变长度编码。

U+0000 至 U+FFFF 这组 Unicode 字符称为基本多文种平面（basic multilingual plane, BMP），还有另外 16 个平面。那么 BMP 以外的字符，JSON 会使用代理对（surrogate pair）表示 `\uXXXX\uYYYY`。在 BMP 中，保留了 2048 个代理码点。如果第一个码点是 U+D800 至 U+DBFF，我们便知道它的代码对的高代理项（high surrogate），之后应该伴随一个 U+DC00 至 U+DFFF 的低代理项（low surrogate）。然后，我们用下列公式把代理对 (H, L) 变换成真实的码点：

```
codepoint = 0x10000 + (H − 0xD800) × 0x400 + (L − 0xDC00)
```

UTF-8 的编码单元为 8 位（1 字节），每个码点编码成 1 至 4 个字节。它的编码方式很简单，按照码点的范围，把码点的二进位分拆成 1 至最多 4 个字节：

| 码点范围           | 码点位数 |  字节1   |  字节2   |  字节3   |  字节4   |
| :----------------- | :------: | :------: | :------: | :------: | :------: |
| U+0000 ~ U+007F    |    7     | 0xxxxxxx |          |          |          |
| U+0080 ~ U+07FF    |    11    | 110xxxxx | 10xxxxxx |          |          |
| U+0800 ~ U+FFFF    |    16    | 1110xxxx | 10xxxxxx | 10xxxxxx |          |
| U+10000 ~ U+10FFFF |    21    | 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |

### 为什么要做 `x & 0xFF` 这种操作呢？

这是因为 `u` 是 `unsigned` 类型，一些编译器可能会警告这个转型可能会截断数据。但实际上，配合了范围的检测然后右移之后，可以保证写入的是 0~255 内的值。为了避免一些编译器的警告误判，我们加上 `x & 0xFF`。一般来说，编译器在优化之后，这与操作是会被消去的，不会影响性能。



## 7、数据结构

### json——string类存储方式：

JSON 数组存储零至多个元素，最简单就是使用 C 语言的数组。数组最大的好处是能以 $O(1)$ 用索引访问任意元素，次要好处是内存布局紧凑，省内存之余还有高缓存一致性（cache coherence）。但数组的缺点是不能快速插入元素，而且我们在解析 JSON 数组的时候，还不知道应该分配多大的数组才合适。

另一个选择是链表（linked list），它的最大优点是可快速地插入元素（开端、末端或中间），但需要以 $O(n)$ 时间去经索引取得内容。如果我们只需顺序遍历，那么是没有问题的。还有一个小缺点，就是相对数组而言，链表在存储每个元素时有额外内存开销（存储下一节点的指针），而且遍历时元素所在的内存可能不连续，令缓存不命中（cache miss）的机会上升。

### json——对象类表示键值对的存储方式：

要表示键值对的集合，有很多数据结构可供选择，例如：

- 动态数组（dynamic array）：可扩展容量的数组，如 C++ 的 [`std::vector`](https://en.cppreference.com/w/cpp/container/vector)。
- 有序动态数组（sorted dynamic array）：和动态数组相同，但保证元素已排序，可用二分搜寻查询成员。
- 平衡树（balanced tree）：平衡二叉树可有序地遍历成员，如红黑树和 C++ 的 [`std::map`](https://en.cppreference.com/w/cpp/container/map)（[`std::multi_map`](https://en.cppreference.com/w/cpp/container/multimap) 支持重复键）。
- 哈希表（hash table）：通过哈希函数能实现平均 O(1) 查询，如 C++11 的 [`std::unordered_map`](https://en.cppreference.com/w/cpp/container/unordered_map)（[`unordered_multimap`](https://en.cppreference.com/w/cpp/container/unordered_multimap) 支持重复键）。

设一个对象有 n 个成员，数据结构的容量是 m，n ⩽ m，那么一些常用操作的时间／空间复杂度如下：

|                 | 动态数组  | 有序动态数组 |   平衡树   |         哈希表         |
| --------------- | :-------: | :----------: | :--------: | :--------------------: |
| 有序            |    否     |      是      |     是     |           否           |
| 自定成员次序    |    可     |      否      |     否     |           否           |
| 初始化 n 个成员 |   O(n)    |  O(n log n)  | O(n log n) | 平均 O(n)、最坏 O(n^2) |
| 加入成员        | 分摊 O(1) |     O(n)     |  O(log n)  |  平均 O(1)、最坏 O(n)  |
| 移除成员        |   O(n)    |     O(n)     |  O(log n)  |  平均 O(1)、最坏 O(n)  |
| 查询成员        |   O(n)    |   O(log n)   |  O(log n)  |  平均 O(1)、最坏 O(n)  |
| 遍历成员        |   O(n)    |     O(n)     |    O(n)    |          O(m)          |
| 检测对象相等    |  O(n^2)   |     O(n)     |    O(n)    | 平均 O(n)、最坏 O(n^2) |
| 空间            |   O(m)    |     O(m)     |    O(n)    |          O(m)          |



## 8、性能优化

`lept_parse_string()`中，为了处理字符转义，先将 `json` 到 `stack`，第二次是从 `stack` 到 `v->u.s.s`。我们可以在 `json` 扫描 `'\0'`、`'\"'` 和 `'\\'` 3 个字符（ `ch < 0x20` 还是要检查），直至它们其中一个出现，才开始用现在的解析方法。这样做的话，前半没转义的部分可以只复制一次。缺点是，代码变得复杂一些，我们也不能使用 `lept_set_string()`。



## 9、bug的解释

```c
for (;;) {
    /* bug! */
    lept_value* e = lept_context_push(c, sizeof(lept_value));
    lept_init(e);
    size++;
    if ((ret = lept_parse_value(c, e)) != LEPT_PARSE_OK)
        return ret;
    /* ... */
}
```
这个 bug 源于压栈时，会获得一个指针 `e`，指向从堆栈分配到的空间，

然后，我们把这个指针调用 `lept_parse_value(c, e)`，这里会出现问题，因为 `lept_parse_value()` 及之下的函数都需要调用 `lept_context_push()`，而 `lept_context_push()` 在发现栈满了的时候会用 `realloc()` 扩容。这时候，我们上层的 `e` 就会失效，变成一个悬挂指针（dangling pointer），而且 `lept_parse_value(c, e)` 会通过这个指针写入解析结果，造成非法访问。

在使用 C++ 容器时，也会遇到类似的问题。从容器中取得的迭代器（iterator）后，如果改动容器内容，之前的迭代器会失效。这里的悬挂指针问题也是相同的。

编程时要考虑清楚变量的生命周期，特别是指针的生命周期。



## 10、访问与其他功能的思考

### 键值对的查找：

应为采用的是数组，并且没有使用其他数据结构来记录每一个键值对的位置，所以使用简单的线性搜索

### 复制、移动、交换：

深拷贝与浅拷贝

采用`memcpy()`在拷贝过程中可能出现重叠问题，可以使用`memmove()`来避免
