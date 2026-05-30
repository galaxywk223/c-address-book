#ifndef LIST_H
#define LIST_H

#include "contact.h"

/* 双向链表节点 */
typedef struct Node {
    Contact      data;
    struct Node *prev;
    struct Node *next;
} Node;

/* 链表结构 */
typedef struct {
    Node *head;
    Node *tail;
    int   count;
    int   next_id;   /* 下一个可用 ID */
} LinkedList;

/* ---------- 链表基础操作 ---------- */

/* 初始化链表 */
void list_init(LinkedList *list);

/* 销毁链表（释放所有节点内存） */
void list_destroy(LinkedList *list);

/* 添加节点到链表尾部（深拷贝 contact） */
Node* list_append(LinkedList *list, const Contact *contact);

/* 按 ID 查找节点 */
Node* list_find_by_id(const LinkedList *list, int id);

/* 按索引查找节点（0-based） */
Node* list_find_by_index(const LinkedList *list, int index);

/* 删除节点（从链表中摘除，不释放） */
void list_remove_node(LinkedList *list, Node *node);

/* 释放节点内存 */
void node_free(Node *node);

/* ---------- 排序功能 ---------- */

/* 比较函数类型 */
typedef int (*CompareFunc)(const Contact *a, const Contact *b);

/* 按姓名比较（拼音序） */
int cmp_by_name(const Contact *a, const Contact *b);

/* 按手机号比较 */
int cmp_by_mobile(const Contact *a, const Contact *b);

/* 按单位比较 */
int cmp_by_company(const Contact *a, const Contact *b);

/* 按分类比较 */
int cmp_by_category(const Contact *a, const Contact *b);

/* 链表排序（归并排序，稳定） */
void list_sort(LinkedList *list, CompareFunc cmp);

#endif /* LIST_H */
