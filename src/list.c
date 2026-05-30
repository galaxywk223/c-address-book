#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

void list_init(LinkedList *list) {
    list->head     = NULL;
    list->tail     = NULL;
    list->count    = 0;
    list->next_id  = 1;
}

void list_destroy(LinkedList *list) {
    Node *cur = list->head;
    while (cur) {
        Node *next = cur->next;
        free(cur);
        cur = next;
    }
    list->head    = NULL;
    list->tail    = NULL;
    list->count   = 0;
}

Node* list_append(LinkedList *list, const Contact *contact) {
    Node *node = (Node*)malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "  [错误] 内存分配失败\n");
        return NULL;
    }
    memcpy(&node->data, contact, sizeof(Contact));
    node->prev = list->tail;
    node->next = NULL;

    if (list->tail) {
        list->tail->next = node;
    } else {
        list->head = node;
    }
    list->tail = node;
    list->count++;

    /* 更新 next_id */
    if (contact->id >= list->next_id) {
        list->next_id = contact->id + 1;
    }

    return node;
}

Node* list_find_by_id(const LinkedList *list, int id) {
    Node *cur = list->head;
    while (cur) {
        if (cur->data.id == id) return cur;
        cur = cur->next;
    }
    return NULL;
}

Node* list_find_by_index(const LinkedList *list, int index) {
    if (index < 0 || index >= list->count) return NULL;
    Node *cur = list->head;
    for (int i = 0; i < index && cur; i++) {
        cur = cur->next;
    }
    return cur;
}

void list_remove_node(LinkedList *list, Node *node) {
    if (!node) return;
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }
    node->prev = NULL;
    node->next = NULL;
    list->count--;
}

void node_free(Node *node) {
    if (node) free(node);
}

/* ---------- 排序（归并排序） ---------- */

int cmp_by_name(const Contact *a, const Contact *b) {
    return strcmp(a->name, b->name);
}

int cmp_by_mobile(const Contact *a, const Contact *b) {
    return strcmp(a->mobile, b->mobile);
}

int cmp_by_company(const Contact *a, const Contact *b) {
    return strcmp(a->company, b->company);
}

int cmp_by_category(const Contact *a, const Contact *b) {
    if (a->category != b->category) {
        return (int)a->category - (int)b->category;
    }
    return strcmp(a->name, b->name);
}

/* 归并排序辅助函数 */
static Node* merge(Node *a, Node *b, CompareFunc cmp) {
    Node dummy;
    Node *tail = &dummy;
    dummy.next = NULL;

    while (a && b) {
        if (cmp(&a->data, &b->data) <= 0) {
            tail->next = a;
            a->prev = tail;
            a = a->next;
        } else {
            tail->next = b;
            b->prev = tail;
            b = b->next;
        }
        tail = tail->next;
    }
    tail->next = a ? a : b;
    if (tail->next) tail->next->prev = tail;
    return dummy.next;
}

static Node* merge_sort(Node *head, CompareFunc cmp) {
    if (!head || !head->next) return head;

    /* 找中点 */
    Node *slow = head, *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    Node *mid = slow->next;
    slow->next = NULL;
    if (mid) mid->prev = NULL;

    Node *left  = merge_sort(head, cmp);
    Node *right = merge_sort(mid, cmp);
    return merge(left, right, cmp);
}

void list_sort(LinkedList *list, CompareFunc cmp) {
    if (!list->head || !list->head->next) return;

    list->head = merge_sort(list->head, cmp);

    /* 重建 tail 和 prev 指针 */
    Node *cur = list->head;
    cur->prev = NULL;
    while (cur->next) {
        cur->next->prev = cur;
        cur = cur->next;
    }
    list->tail = cur;
}
