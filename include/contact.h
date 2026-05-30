#ifndef CONTACT_H
#define CONTACT_H

#include <time.h>

/* 最大字段长度 */
#define MAX_NAME      50
#define MAX_COMPANY  100
#define MAX_PHONE     30
#define MAX_MOBILE    20
#define MAX_CATEGORY  20
#define MAX_EMAIL     60
#define MAX_QQ        20
#define MAX_ADDRESS  200
#define MAX_NOTE     200

/* 分类枚举 */
typedef enum {
    CAT_FAMILY = 0,   /* 家人 */
    CAT_FRIEND,       /* 朋友 */
    CAT_WORK,         /* 同事 */
    CAT_BUSINESS,     /* 商业 */
    CAT_OTHER,        /* 其他 */
    CAT_COUNT         /* 分类总数 */
} Category;

/* 联系人结构体 */
typedef struct {
    int    id;                      /* 唯一标识 */
    char   name[MAX_NAME];          /* 姓名 */
    char   company[MAX_COMPANY];    /* 单位 */
    char   phone[MAX_PHONE];        /* 固定电话 */
    char   mobile[MAX_MOBILE];      /* 手机号 */
    Category category;              /* 分类 */
    char   email[MAX_EMAIL];        /* Email */
    char   qq[MAX_QQ];              /* QQ */
    char   address[MAX_ADDRESS];    /* 地址 */
    char   note[MAX_NOTE];          /* 备注 */
    time_t create_time;             /* 创建时间 */
    time_t modify_time;             /* 最后修改时间 */
} Contact;

/* 获取分类名称 */
const char* category_name(Category cat);

/* 打印单个联系人（详细） */
void contact_print(const Contact *c);

/* 打印单个联系人（表格行） */
void contact_print_row(const Contact *c);

/* 打印表头 */
void contact_print_header(void);

/* 交互式输入联系人（不含 ID 和时间，由调用者设置） */
int contact_input(Contact *c);

/* 交互式修改联系人 */
int contact_modify(Contact *c);

#endif /* CONTACT_H */
