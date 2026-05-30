#include <stdio.h>
#include <string.h>
#include "search.h"
#include "utils.h"

void search_init(SearchCriteria *criteria) {
    memset(criteria, 0, sizeof(SearchCriteria));
    criteria->category = CAT_OTHER;
    criteria->use_category = 0;
}

int search_match(const Contact *c, const SearchCriteria *criteria) {
    /* 分类过滤 */
    if (criteria->use_category && c->category != criteria->category) {
        return 0;
    }

    /* 关键词为空则匹配所有 */
    if (!criteria->keyword[0]) return 1;

    /* 模糊匹配各字段 */
    if (util_str_contains(c->name, criteria->keyword))    return 1;
    if (util_str_contains(c->company, criteria->keyword)) return 1;
    if (util_str_contains(c->mobile, criteria->keyword))  return 1;
    if (util_str_contains(c->phone, criteria->keyword))   return 1;
    if (util_str_contains(c->email, criteria->keyword))   return 1;
    if (util_str_contains(c->qq, criteria->keyword))      return 1;
    if (util_str_contains(c->address, criteria->keyword)) return 1;
    if (util_str_contains(c->note, criteria->keyword))    return 1;

    return 0;
}

int search_execute(const LinkedList *list, LinkedList *results,
                   const SearchCriteria *criteria) {
    int count = 0;
    Node *cur = list->head;
    while (cur) {
        if (search_match(&cur->data, criteria)) {
            list_append(results, &cur->data);
            count++;
        }
        cur = cur->next;
    }
    return count;
}

int search_is_duplicate(const LinkedList *list, const Contact *c) {
    Node *cur = list->head;
    while (cur) {
        /* 姓名相同 + 手机号相同 = 重复 */
        if (strcmp(cur->data.name, c->name) == 0 &&
            c->mobile[0] &&
            strcmp(cur->data.mobile, c->mobile) == 0) {
            return 1;
        }
        cur = cur->next;
    }
    return 0;
}

void search_category_stats(const LinkedList *list, int stats[]) {
    for (int i = 0; i < CAT_COUNT; i++) stats[i] = 0;
    Node *cur = list->head;
    while (cur) {
        if (cur->data.category >= 0 && cur->data.category < CAT_COUNT) {
            stats[cur->data.category]++;
        }
        cur = cur->next;
    }
}

int search_by_category(const LinkedList *list, LinkedList *results,
                       Category cat) {
    int count = 0;
    Node *cur = list->head;
    while (cur) {
        if (cur->data.category == cat) {
            list_append(results, &cur->data);
            count++;
        }
        cur = cur->next;
    }
    return count;
}
