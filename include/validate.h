#ifndef VALIDATE_H
#define VALIDATE_H

#include "contact.h"

/* 验证结果码 */
#define VALID_OK        0
#define VALID_EMPTY     1   /* 必填字段为空 */
#define VALID_TOO_LONG  2   /* 字段过长 */
#define VALID_BAD_FMT   3   /* 格式错误 */

/* 验证姓名（必填，长度 <= MAX_NAME-1） */
int validate_name(const char *name);

/* 验证手机号（可选，11 位数字） */
int validate_mobile(const char *mobile);

/* 验证固定电话（可选，格式灵活） */
int validate_phone(const char *phone);

/* 验证 Email（可选，基本格式检查） */
int validate_email(const char *email);

/* 验证 QQ 号（可选，5-12 位数字） */
int validate_qq(const char *qq);

/* 验证整个联系人结构 */
int validate_contact(const Contact *c);

/* 获取验证错误信息 */
const char* validate_error_msg(int code);

#endif /* VALIDATE_H */
