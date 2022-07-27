/*************************************************************************
	> File Name: leptjson.h
	> Author: YWH
	> Mail: 925957192@qq.com
	> Created Time: Fri 15 Jul 2022 03:39:12 AM EDT
 ************************************************************************/

#ifndef LEPTJSON_H__
#define LEPTJSON_H__ 

#include <stddef.h> /* size_t */

/*json中的6种数据类型,真假算两种，则总共7种*/
typedef enum 
{
    LEPT_NULL,
    LEPT_FALSE,
    LEPT_TRUE,
    LEPT_NUMBER,
    LEPT_STRING,
    LEPT_ARRAY,
    LEPT_OBJECT
}lept_type;

/*json的树形结构*/
typedef struct lept_value lept_value;
typedef struct lept_member lept_member;
struct lept_value 
{
    union {
        struct { char* s; size_t len; }s;                       /* type==LEPT_STRING */
        struct { lept_value*  e; size_t size, capacity; }a;     /* type==LEPT_ARRAY  */
        struct { lept_member* m; size_t size, capacity; }o;     /* type==LEPT_OBJECT */
        double n;                                               /* type==LEPT_NUMBER */
    }u;
    lept_type type;
};
struct lept_member 
{
    char* k; size_t klen;   /* member key string */
    lept_value v;           /* member value*/
};

#define LEPT_KEY_NOT_EXIST ((size_t)-1)
/*以下是函数会用到的返回值的枚举值*/
enum 
{
    LEPT_PARSE_OK = 0,                          /*无错误*/ 
    LEPT_PARSE_EXPECT_VALUE,                    /*json只含有空白*/ 
    LEPT_PARSE_INVALID_VALUE,                   /*value不是规定的字面值*/ 
    LEPT_PARSE_ROOT_NOT_SINGULAR,               /*在value后的空白之后还有其他字符*/ 
    LEPT_PARSE_NUMBER_TOO_BIG,                  /*json数字是否过大*/ 
    LEPT_PARSE_MISS_QUOTATION_MARK,             /* 缺失双引号*/
    LEPT_PARSE_INVALID_STRING_ESCAPE,           /*无效的字符转义*/
    LEPT_PARSE_INVALID_STRING_CHAR,             /*无效字符*/
    LEPT_PARSE_INVALID_UNICODE_HEX,             /*无效的unicode十六进位格式*/
    LEPT_PARSE_INVALID_UNICODE_SURROGATE,       /*无效的代理对*/
    LEPT_PARSE_MISS_COMMA_OR_SQUARE_BRACKET,    /*没有逗号或者方括号*/
    LEPT_PARSE_MISS_KEY,                        /*member key值只能为json类型的string*/
    LEPT_PARSE_MISS_COLON,                      /*member key和value之间通过 : 分开*/
    LEPT_PARSE_MISS_COMMA_OR_CURLY_BRACKET      /*object对象中的每个member之间用 , 隔开 或没有}*/
};

#define lept_init(v) do { (v)->type = LEPT_NULL; } while(0)

/*解析JSON函数*/
int lept_parse(lept_value* v, const char* json);
/*JSON生成器*/
char* lept_stringify(const lept_value* v, size_t* length);

/* lept_value 复制*/
void lept_copy(lept_value* dst, const lept_value* src); /* deep copy */
void lept_move(lept_value* dst, lept_value* src);       /* move semantics */
void lept_swap(lept_value* lhs, lept_value* rhs);       /* swap */

/*针对需要动态分配的内存进行释放*/
void lept_free(lept_value* v);

/*访问结果函数*/
lept_type lept_get_type(const lept_value* v);
/* 比较两个结果函数 */
int lept_is_equal(const lept_value* lhs, const lept_value* rhs);

#define lept_set_null(v) lept_free(v)

int lept_get_boolean(const lept_value* v);
void lept_set_boolean(lept_value* v, int b);

/*获取json数字值*/
double lept_get_number(const lept_value* v);
void lept_set_number(lept_value* v, double n);

const char* lept_get_string(const lept_value* v);
size_t lept_get_string_length(const lept_value* v);
void lept_set_string(lept_value* v, const char* s, size_t len);

void lept_set_array(lept_value* v, size_t capacity);
size_t lept_get_array_size(const lept_value* v);
size_t lept_get_array_capacity(const lept_value* v);
void lept_reserve_array(lept_value* v, size_t capacity);
void lept_shrink_array(lept_value* v);
void lept_clear_array(lept_value* v);
lept_value* lept_get_array_element(const lept_value* v, size_t index);
lept_value* lept_pushback_array_element(lept_value* v);
void lept_popback_array_element(lept_value* v);
lept_value* lept_insert_array_element(lept_value* v, size_t index);
void lept_erase_array_element(lept_value* v, size_t index, size_t count);

void lept_set_object(lept_value* v, size_t capacity);
size_t lept_get_object_size(const lept_value* v);
size_t lept_get_object_capacity(const lept_value* v);
void lept_reserve_object(lept_value* v, size_t capacity);
void lept_shrink_object(lept_value* v);
void lept_clear_object(lept_value* v);
const char* lept_get_object_key(const lept_value* v, size_t index);
size_t lept_get_object_key_length(const lept_value* v, size_t index);
lept_value* lept_get_object_value(const lept_value* v, size_t index);
size_t lept_find_object_index(const lept_value* v, const char* key, size_t klen);
lept_value* lept_find_object_value(lept_value* v, const char* key, size_t klen);
lept_value* lept_set_object_value(lept_value* v, const char* key, size_t klen);
void lept_remove_object_value(lept_value* v, size_t index);

#endif /*LEPTJSON_H__ */
