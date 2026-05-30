#include <stdio.h>
#include <string.h>
#include <time.h>
#include "contact.h"
#include "validate.h"
#include "utils.h"

static const char *CATEGORY_NAMES[] = {
    "家人", "朋友", "同事", "商业", "其他"
};

const char* category_name(Category cat) {
    if (cat >= 0 && cat < CAT_COUNT) {
        return CATEGORY_NAMES[cat];
    }
    return "未知";
}

void contact_print(const Contact *c) {
    char time_buf[64];
    printf("  编号: %d\n", c->id);
    printf("  姓名: %s\n", c->name);
    printf("  单位: %s\n", c->company[0] ? c->company : "(无)");
    printf("  固定电话: %s\n", c->phone[0] ? c->phone : "(无)");
    printf("  手机号: %s\n", c->mobile[0] ? c->mobile : "(无)");
    printf("  分类: %s\n", category_name(c->category));
    printf("  Email: %s\n", c->email[0] ? c->email : "(无)");
    printf("  QQ: %s\n", c->qq[0] ? c->qq : "(无)");
    printf("  地址: %s\n", c->address[0] ? c->address : "(无)");
    printf("  备注: %s\n", c->note[0] ? c->note : "(无)");
    util_format_time(c->create_time, time_buf, sizeof(time_buf));
    printf("  创建时间: %s\n", time_buf);
    util_format_time(c->modify_time, time_buf, sizeof(time_buf));
    printf("  修改时间: %s\n", time_buf);
}

void contact_print_header(void) {
    printf("  %-4s %-10s %-12s %-14s %-6s %-20s\n",
           "编号", "姓名", "手机号", "单位", "分类", "Email");
    printf("  %-4s %-10s %-12s %-14s %-6s %-20s\n",
           "----", "----------", "------------", "--------------",
           "------", "--------------------");
}

void contact_print_row(const Contact *c) {
    /* 截断显示，避免行过长 */
    char name[MAX_NAME], company[MAX_COMPANY], mobile[MAX_MOBILE];
    char email[MAX_EMAIL];
    snprintf(name, sizeof(name), "%s", c->name);
    snprintf(company, sizeof(company), "%s", c->company);
    snprintf(mobile, sizeof(mobile), "%s", c->mobile);
    snprintf(email, sizeof(email), "%s", c->email);
    printf("  %-4d %-10s %-12s %-14s %-6s %-20s\n",
           c->id, name, mobile, company,
           category_name(c->category), email);
}

int contact_input(Contact *c) {
    int valid;

    memset(c, 0, sizeof(Contact));

    /* 姓名（必填） */
    do {
        printf("  姓名（必填）: ");
        util_get_line(c->name, MAX_NAME);
        util_trim(c->name);
        valid = validate_name(c->name);
        if (valid != VALID_OK) {
            printf("  [错误] %s\n", validate_error_msg(valid));
        }
    } while (valid != VALID_OK);

    /* 单位 */
    printf("  单位: ");
    util_get_line(c->company, MAX_COMPANY);
    util_trim(c->company);

    /* 固定电话 */
    do {
        printf("  固定电话: ");
        util_get_line(c->phone, MAX_PHONE);
        util_trim(c->phone);
        valid = validate_phone(c->phone);
        if (valid != VALID_OK) {
            printf("  [错误] %s\n", validate_error_msg(valid));
        }
    } while (valid != VALID_OK);

    /* 手机号 */
    do {
        printf("  手机号: ");
        util_get_line(c->mobile, MAX_MOBILE);
        util_trim(c->mobile);
        valid = validate_mobile(c->mobile);
        if (valid != VALID_OK) {
            printf("  [错误] %s\n", validate_error_msg(valid));
        }
    } while (valid != VALID_OK);

    /* 分类 */
    printf("  分类（0=家人 1=朋友 2=同事 3=商业 4=其他，默认4）: ");
    int cat = util_get_int_default(4);
    if (cat < 0 || cat >= CAT_COUNT) cat = CAT_OTHER;
    c->category = (Category)cat;

    /* Email */
    do {
        printf("  Email: ");
        util_get_line(c->email, MAX_EMAIL);
        util_trim(c->email);
        valid = validate_email(c->email);
        if (valid != VALID_OK) {
            printf("  [错误] %s\n", validate_error_msg(valid));
        }
    } while (valid != VALID_OK);

    /* QQ */
    do {
        printf("  QQ: ");
        util_get_line(c->qq, MAX_QQ);
        util_trim(c->qq);
        valid = validate_qq(c->qq);
        if (valid != VALID_OK) {
            printf("  [错误] %s\n", validate_error_msg(valid));
        }
    } while (valid != VALID_OK);

    /* 地址 */
    printf("  地址: ");
    util_get_line(c->address, MAX_ADDRESS);
    util_trim(c->address);

    /* 备注 */
    printf("  备注: ");
    util_get_line(c->note, MAX_NOTE);
    util_trim(c->note);

    c->create_time = time(NULL);
    c->modify_time = c->create_time;

    return 1;
}

int contact_modify(Contact *c) {
    char buf[256];
    int valid;

    printf("  （直接按回车保留原值）\n\n");

    /* 姓名 */
    printf("  姓名 [%s]: ", c->name);
    util_get_line(buf, MAX_NAME);
    util_trim(buf);
    if (buf[0] != '\0') {
        valid = validate_name(buf);
        if (valid != VALID_OK) {
            printf("  [错误] %s，保留原值\n", validate_error_msg(valid));
        } else {
            strncpy(c->name, buf, MAX_NAME - 1);
        }
    }

    /* 单位 */
    printf("  单位 [%s]: ", c->company);
    util_get_line(buf, MAX_COMPANY);
    util_trim(buf);
    if (buf[0] != '\0') {
        strncpy(c->company, buf, MAX_COMPANY - 1);
    }

    /* 固定电话 */
    printf("  固定电话 [%s]: ", c->phone);
    util_get_line(buf, MAX_PHONE);
    util_trim(buf);
    if (buf[0] != '\0') {
        valid = validate_phone(buf);
        if (valid != VALID_OK) {
            printf("  [错误] %s，保留原值\n", validate_error_msg(valid));
        } else {
            strncpy(c->phone, buf, MAX_PHONE - 1);
        }
    }

    /* 手机号 */
    printf("  手机号 [%s]: ", c->mobile);
    util_get_line(buf, MAX_MOBILE);
    util_trim(buf);
    if (buf[0] != '\0') {
        valid = validate_mobile(buf);
        if (valid != VALID_OK) {
            printf("  [错误] %s，保留原值\n", validate_error_msg(valid));
        } else {
            strncpy(c->mobile, buf, MAX_MOBILE - 1);
        }
    }

    /* 分类 */
    printf("  分类 [%s]（0=家人 1=朋友 2=同事 3=商业 4=其他，回车保留）: ",
           category_name(c->category));
    util_get_line(buf, sizeof(buf));
    util_trim(buf);
    if (buf[0] != '\0') {
        int cat = atoi(buf);
        if (cat >= 0 && cat < CAT_COUNT) {
            c->category = (Category)cat;
        }
    }

    /* Email */
    printf("  Email [%s]: ", c->email);
    util_get_line(buf, MAX_EMAIL);
    util_trim(buf);
    if (buf[0] != '\0') {
        valid = validate_email(buf);
        if (valid != VALID_OK) {
            printf("  [错误] %s，保留原值\n", validate_error_msg(valid));
        } else {
            strncpy(c->email, buf, MAX_EMAIL - 1);
        }
    }

    /* QQ */
    printf("  QQ [%s]: ", c->qq);
    util_get_line(buf, MAX_QQ);
    util_trim(buf);
    if (buf[0] != '\0') {
        valid = validate_qq(buf);
        if (valid != VALID_OK) {
            printf("  [错误] %s，保留原值\n", validate_error_msg(valid));
        } else {
            strncpy(c->qq, buf, MAX_QQ - 1);
        }
    }

    /* 地址 */
    printf("  地址 [%s]: ", c->address);
    util_get_line(buf, MAX_ADDRESS);
    util_trim(buf);
    if (buf[0] != '\0') {
        strncpy(c->address, buf, MAX_ADDRESS - 1);
    }

    /* 备注 */
    printf("  备注 [%s]: ", c->note);
    util_get_line(buf, MAX_NOTE);
    util_trim(buf);
    if (buf[0] != '\0') {
        strncpy(c->note, buf, MAX_NOTE - 1);
    }

    c->modify_time = time(NULL);
    return 1;
}
