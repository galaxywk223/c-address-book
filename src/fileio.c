#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <direct.h>
#include "fileio.h"
#include "utils.h"

/* CSV 字段分隔符 */
#define CSV_SEP ','

/* 安全写入 CSV 字段（转义逗号和引号） */
static void csv_write_field(FILE *fp, const char *field) {
    if (!field || !*field) return;
    int need_quote = 0;
    for (const char *p = field; *p; p++) {
        if (*p == ',' || *p == '"' || *p == '\n') {
            need_quote = 1;
            break;
        }
    }
    if (need_quote) {
        fputc('"', fp);
        for (const char *p = field; *p; p++) {
            if (*p == '"') fputc('"', fp);
            fputc(*p, fp);
        }
        fputc('"', fp);
    } else {
        fputs(field, fp);
    }
}

/* 安全读取 CSV 字段 */
static int csv_read_field(const char **src, char *dest, int dest_size) {
    const char *p = *src;
    int len = 0;

    if (*p == '"') {
        /* 带引号字段 */
        p++;
        while (*p && len < dest_size - 1) {
            if (*p == '"') {
                if (*(p + 1) == '"') {
                    dest[len++] = '"';
                    p += 2;
                } else {
                    p++;
                    break;
                }
            } else {
                dest[len++] = *p++;
            }
        }
    } else {
        /* 无引号字段 */
        while (*p && *p != ',' && *p != '\n' && *p != '\r' && len < dest_size - 1) {
            dest[len++] = *p++;
        }
    }
    dest[len] = '\0';

    /* 跳过分隔符 */
    if (*p == ',') p++;
    /* 跳过换行 */
    if (*p == '\r') p++;
    if (*p == '\n') p++;

    *src = p;
    return len;
}

void fileio_ensure_dirs(void) {
    _mkdir(DATA_DIR);
    _mkdir(BACKUP_DIR);
}

int fileio_load(LinkedList *list, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        return 0;  /* 文件不存在，正常情况 */
    }

    char line[1024];
    int count = 0;

    /* 跳过表头 */
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return 0;
    }

    while (fgets(line, sizeof(line), fp)) {
        /* 去除换行 */
        size_t len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = '\0';

        if (len == 0) continue;

        Contact c;
        memset(&c, 0, sizeof(Contact));

        const char *p = line;
        char buf[256];

        /* id */
        csv_read_field(&p, buf, sizeof(buf));
        c.id = atoi(buf);

        /* name */
        csv_read_field(&p, c.name, MAX_NAME);

        /* company */
        csv_read_field(&p, c.company, MAX_COMPANY);

        /* phone */
        csv_read_field(&p, c.phone, MAX_PHONE);

        /* mobile */
        csv_read_field(&p, c.mobile, MAX_MOBILE);

        /* category */
        csv_read_field(&p, buf, sizeof(buf));
        c.category = (Category)atoi(buf);

        /* email */
        csv_read_field(&p, c.email, MAX_EMAIL);

        /* qq */
        csv_read_field(&p, c.qq, MAX_QQ);

        /* address */
        csv_read_field(&p, c.address, MAX_ADDRESS);

        /* note */
        csv_read_field(&p, c.note, MAX_NOTE);

        /* create_time */
        csv_read_field(&p, buf, sizeof(buf));
        c.create_time = (time_t)atoll(buf);

        /* modify_time */
        csv_read_field(&p, buf, sizeof(buf));
        c.modify_time = (time_t)atoll(buf);

        list_append(list, &c);
        count++;
    }

    fclose(fp);
    return count;
}

int fileio_save(LinkedList *list, const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        fprintf(stderr, "  [错误] 无法写入文件: %s\n", filepath);
        return 0;
    }

    /* 写入 UTF-8 BOM（方便 Excel 打开中文） */
    fprintf(fp, "\xEF\xBB\xBF");

    /* 写入表头 */
    fprintf(fp, "id,name,company,phone,mobile,category,email,qq,address,note,create_time,modify_time\n");

    Node *cur = list->head;
    while (cur) {
        Contact *c = &cur->data;

        fprintf(fp, "%d,", c->id);
        csv_write_field(fp, c->name);     fputc(',', fp);
        csv_write_field(fp, c->company);  fputc(',', fp);
        csv_write_field(fp, c->phone);    fputc(',', fp);
        csv_write_field(fp, c->mobile);   fputc(',', fp);
        fprintf(fp, "%d,", c->category);
        csv_write_field(fp, c->email);    fputc(',', fp);
        csv_write_field(fp, c->qq);       fputc(',', fp);
        csv_write_field(fp, c->address);  fputc(',', fp);
        csv_write_field(fp, c->note);     fputc(',', fp);
        fprintf(fp, "%lld,", (long long)c->create_time);
        fprintf(fp, "%lld\n", (long long)c->modify_time);

        cur = cur->next;
    }

    fclose(fp);
    return 1;
}

int fileio_backup(void) {
    FILE *fp = fopen(DATA_FILE, "r");
    if (!fp) return 0;
    fclose(fp);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char backup_path[256];
    snprintf(backup_path, sizeof(backup_path),
             "%s/contacts_%04d%02d%02d_%02d%02d%02d.csv",
             BACKUP_DIR,
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);

    FILE *src = fopen(DATA_FILE, "r");
    FILE *dst = fopen(backup_path, "w");
    if (!src || !dst) {
        if (src) fclose(src);
        if (dst) fclose(dst);
        return 0;
    }

    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), src)) > 0) {
        fwrite(buf, 1, n, dst);
    }

    fclose(src);
    fclose(dst);
    return 1;
}

int fileio_export(LinkedList *list, const char *filepath) {
    return fileio_save(list, filepath);
}

int fileio_import(LinkedList *list, const char *filepath) {
    int count = fileio_load(list, filepath);
    return count;
}

int fileio_save_trash(const Contact *c) {
    FILE *fp = fopen(TRASH_FILE, "a");
    if (!fp) return 0;

    /* 首次写入时添加表头 */
    fseek(fp, 0, SEEK_END);
    if (ftell(fp) == 0) {
        fprintf(fp, "\xEF\xBB\xBF");
        fprintf(fp, "id,name,company,phone,mobile,category,email,qq,address,note,create_time,modify_time\n");
    }

    fprintf(fp, "%d,", c->id);
    csv_write_field(fp, c->name);     fputc(',', fp);
    csv_write_field(fp, c->company);  fputc(',', fp);
    csv_write_field(fp, c->phone);    fputc(',', fp);
    csv_write_field(fp, c->mobile);   fputc(',', fp);
    fprintf(fp, "%d,", c->category);
    csv_write_field(fp, c->email);    fputc(',', fp);
    csv_write_field(fp, c->qq);       fputc(',', fp);
    csv_write_field(fp, c->address);  fputc(',', fp);
    csv_write_field(fp, c->note);     fputc(',', fp);
    fprintf(fp, "%lld,", (long long)c->create_time);
    fprintf(fp, "%lld\n", (long long)c->modify_time);

    fclose(fp);
    return 1;
}
