#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* 每页显示的联系人数量 */
#define PAGE_SIZE 10

/* 清屏 */
void util_clear_screen(void);

/* 暂停等待用户按键 */
void util_pause(void);

/* 清除输入缓冲区 */
void util_clear_input(void);

/* 安全读取一行（去除换行符，防止溢出） */
int util_get_line(char *buf, int size);

/* 读取整数 */
int util_get_int(void);

/* 读取整数，带默认值（空输入返回 default_val） */
int util_get_int_default(int default_val);

/* 转小写字符串 */
void util_to_lower(char *str);

/* 去除首尾空白 */
void util_trim(char *str);

/* 字符串是否包含子串（不区分大小写） */
int util_str_contains(const char *haystack, const char *needle);

/* 打印分隔线 */
void util_print_line(void);

/* 打印标题 */
void util_print_title(const char *title);

/* 分页显示：显示页码信息 */
void util_print_page_info(int total, int page, int page_size);

/* 时间格式化输出 */
void util_format_time(time_t t, char *buf, int size);

#endif /* UTILS_H */
