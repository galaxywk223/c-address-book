#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "validate.h"

int validate_name(const char *name) {
    if (!name || !*name) return VALID_EMPTY;
    if ((int)strlen(name) >= MAX_NAME) return VALID_TOO_LONG;
    return VALID_OK;
}

int validate_mobile(const char *mobile) {
    if (!mobile || !*mobile) return VALID_OK;  /* 可选字段 */
    int len = (int)strlen(mobile);
    if (len > MAX_MOBILE - 1) return VALID_TOO_LONG;
    for (int i = 0; i < len; i++) {
        if (!isdigit((unsigned char)mobile[i]) && mobile[i] != '+'
            && mobile[i] != '-' && mobile[i] != ' ') {
            return VALID_BAD_FMT;
        }
    }
    return VALID_OK;
}

int validate_phone(const char *phone) {
    if (!phone || !*phone) return VALID_OK;  /* 可选字段 */
    int len = (int)strlen(phone);
    if (len > MAX_PHONE - 1) return VALID_TOO_LONG;
    for (int i = 0; i < len; i++) {
        if (!isdigit((unsigned char)phone[i]) && phone[i] != '-'
            && phone[i] != '(' && phone[i] != ')' && phone[i] != ' '
            && phone[i] != '+') {
            return VALID_BAD_FMT;
        }
    }
    return VALID_OK;
}

int validate_email(const char *email) {
    if (!email || !*email) return VALID_OK;  /* 可选字段 */
    if ((int)strlen(email) >= MAX_EMAIL) return VALID_TOO_LONG;
    const char *at = strchr(email, '@');
    if (!at || at == email) return VALID_BAD_FMT;
    const char *dot = strrchr(at, '.');
    if (!dot || dot == at + 1 || !*(dot + 1)) return VALID_BAD_FMT;
    return VALID_OK;
}

int validate_qq(const char *qq) {
    if (!qq || !*qq) return VALID_OK;  /* 可选字段 */
    int len = (int)strlen(qq);
    if (len > MAX_QQ - 1) return VALID_TOO_LONG;
    for (int i = 0; i < len; i++) {
        if (!isdigit((unsigned char)qq[i])) return VALID_BAD_FMT;
    }
    /* QQ 号一般 5-12 位 */
    if (len < 5 || len > 12) return VALID_BAD_FMT;
    return VALID_OK;
}

int validate_contact(const Contact *c) {
    int r;
    r = validate_name(c->name);
    if (r != VALID_OK) return r;
    r = validate_mobile(c->mobile);
    if (r != VALID_OK) return r;
    r = validate_phone(c->phone);
    if (r != VALID_OK) return r;
    r = validate_email(c->email);
    if (r != VALID_OK) return r;
    r = validate_qq(c->qq);
    if (r != VALID_OK) return r;
    return VALID_OK;
}

const char* validate_error_msg(int code) {
    switch (code) {
        case VALID_OK:       return "验证通过";
        case VALID_EMPTY:    return "字段不能为空";
        case VALID_TOO_LONG: return "字段长度超出限制";
        case VALID_BAD_FMT:  return "格式不正确";
        default:             return "未知错误";
    }
}
