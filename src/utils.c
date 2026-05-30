#include "utils.h"
#include <time.h>

void util_clear_screen(void) {
    system("cls");
}

void util_pause(void) {
    printf("\n  按 Enter 键继续...");
    util_clear_input();
    getchar();
}

void util_clear_input(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* discard */
    }
}

int util_get_line(char *buf, int size) {
    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return 0;
    }
    /* 去除换行符 */
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        /* 输入超长，清空剩余 */
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {
            /* discard */
        }
    }
    return 1;
}

int util_get_int(void) {
    char buf[64];
    util_get_line(buf, sizeof(buf));
    return atoi(buf);
}

int util_get_int_default(int default_val) {
    char buf[64];
    util_get_line(buf, sizeof(buf));
    if (buf[0] == '\0') return default_val;
    return atoi(buf);
}

void util_to_lower(char *str) {
    for (; *str; str++) {
        *str = (char)tolower((unsigned char)*str);
    }
}

void util_trim(char *str) {
    if (!str || !*str) return;
    /* 去尾部空白 */
    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
    /* 去首部空白 */
    char *p = str;
    while (*p && isspace((unsigned char)*p)) p++;
    if (p != str) {
        memmove(str, p, strlen(p) + 1);
    }
}

int util_str_contains(const char *haystack, const char *needle) {
    if (!haystack || !needle || !*needle) return 0;
    /* 不区分大小写查找 */
    size_t hlen = strlen(haystack);
    size_t nlen = strlen(needle);
    if (nlen > hlen) return 0;
    for (size_t i = 0; i <= hlen - nlen; i++) {
        size_t j;
        for (j = 0; j < nlen; j++) {
            if (tolower((unsigned char)haystack[i + j]) !=
                tolower((unsigned char)needle[j]))
                break;
        }
        if (j == nlen) return 1;
    }
    return 0;
}

void util_print_line(void) {
    printf("  ---------------------------------------------------------------"
           "----------------\n");
}

void util_print_title(const char *title) {
    printf("\n");
    util_print_line();
    printf("  %s\n", title);
    util_print_line();
}

void util_print_page_info(int total, int page, int page_size) {
    int total_pages = (total + page_size - 1) / page_size;
    if (total_pages == 0) total_pages = 1;
    printf("\n  共 %d 条记录，第 %d/%d 页\n", total, page, total_pages);
}

void util_format_time(time_t t, char *buf, int size) {
    if (t <= 0) {
        buf[0] = '\0';
        return;
    }
    struct tm *tm_info = localtime(&t);
    strftime(buf, size, "%Y-%m-%d %H:%M:%S", tm_info);
}
