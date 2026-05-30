#ifndef SEARCH_H
#define SEARCH_H

#include "list.h"

/* 搜索条件结构 */
typedef struct {
    char keyword[200];     /* 搜索关键词 */
    Category category;     /* 分类过滤，-1 表示全部 */
    int  use_category;     /* 是否启用分类过滤 */
} SearchCriteria;

/* 初始化搜索条件 */
void search_init(SearchCriteria *criteria);

/* 按关键词模糊匹配（姓名、单位、手机号、Email、QQ、地址、备注） */
int search_match(const Contact *c, const SearchCriteria *criteria);

/* 从链表中搜索，结果存入结果链表 */
int search_execute(const LinkedList *list, LinkedList *results, const SearchCriteria *criteria);

/* 检查联系人是否重复（按姓名+手机号判断） */
int search_is_duplicate(const LinkedList *list, const Contact *c);

/* 分类统计 */
void search_category_stats(const LinkedList *list, int stats[]);

/* 按分类过滤 */
int search_by_category(const LinkedList *list, LinkedList *results, Category cat);

#endif /* SEARCH_H */
