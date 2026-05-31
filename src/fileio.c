#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <direct.h>
#include "fileio.h"
#include "utils.h"
#include "validate.h"
#include "search.h"

/* CSV field separator */
#define CSV_SEP ','

/* Safe CSV field write (escape commas, quotes, newlines) */
static void csv_write_field(FILE *fp, const char *field) {
    if (!field || !*field) {
        fputs("", fp);
        return;
    }
    int need_quote = 0;
    for (const char *p = field; *p; p++) {
        if (*p == ',' || *p == '"' || *p == '\n' || *p == '\r') {
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

/* Safe CSV field read */
static int csv_read_field(const char **src, char *dest, int dest_size) {
    const char *p = *src;
    int len = 0;

    if (*p == '"') {
        /* Quoted field */
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
        /* Unquoted field */
        while (*p && *p != ',' && *p != '\n' && *p != '\r' && len < dest_size - 1) {
            dest[len++] = *p++;
        }
    }
    dest[len] = '\0';

    /* Skip separator */
    if (*p == ',') p++;
    /* Skip newlines */
    while (*p == '\r' || *p == '\n') p++;

    *src = p;
    return len;
}

/* Parse a single CSV line into Contact */
static int parse_csv_line(const char *line, Contact *c) {
    const char *p = line;
    char buf[256];

    /* id */
    if (!*p) return -1;
    csv_read_field(&p, buf, sizeof(buf));
    c->id = atoi(buf);

    /* name */
    if (!*p) return -2;
    csv_read_field(&p, c->name, MAX_NAME);

    /* company */
    if (!*p) return -3;
    csv_read_field(&p, c->company, MAX_COMPANY);

    /* phone */
    if (!*p) return -4;
    csv_read_field(&p, c->phone, MAX_PHONE);

    /* mobile */
    if (!*p) return -5;
    csv_read_field(&p, c->mobile, MAX_MOBILE);

    /* category */
    if (!*p) return -6;
    csv_read_field(&p, buf, sizeof(buf));
    c->category = (Category)atoi(buf);

    /* email */
    if (!*p) return -7;
    csv_read_field(&p, c->email, MAX_EMAIL);

    /* qq */
    if (!*p) return -8;
    csv_read_field(&p, c->qq, MAX_QQ);

    /* address */
    if (!*p) return -9;
    csv_read_field(&p, c->address, MAX_ADDRESS);

    /* note */
    if (!*p) return -10;
    csv_read_field(&p, c->note, MAX_NOTE);

    /* create_time */
    if (!*p) return -11;
    csv_read_field(&p, buf, sizeof(buf));
    c->create_time = (time_t)atoll(buf);

    /* modify_time */
    csv_read_field(&p, buf, sizeof(buf));
    c->modify_time = (time_t)atoll(buf);

    return 0;
}

void fileio_ensure_dirs(void) {
    _mkdir(DATA_DIR);
    _mkdir(BACKUP_DIR);
}

int fileio_load(LinkedList *list, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        return 0;
    }

    char line[4096];
    int count = 0;

    /* Skip header */
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return 0;
    }

    /* Skip BOM if present */
    if ((unsigned char)line[0] == 0xEF && (unsigned char)line[1] == 0xBB && (unsigned char)line[2] == 0xBF) {
        /* BOM detected, already skipped by fgets */
    }

    while (fgets(line, sizeof(line), fp)) {
        /* Remove trailing newlines */
        size_t len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = '\0';

        if (len == 0) continue;

        Contact c;
        memset(&c, 0, sizeof(Contact));

        int result = parse_csv_line(line, &c);
        if (result == 0) {
            list_append(list, &c);
            count++;
        }
    }

    fclose(fp);
    return count;
}

int fileio_save(LinkedList *list, const char *filepath) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        fprintf(stderr, "  [Error] Cannot write file: %s\n", filepath);
        return 0;
    }

    /* Write UTF-8 BOM */
    fprintf(fp, "\xEF\xBB\xBF");

    /* Write header */
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

int fileio_import(LinkedList *list, const char *filepath, ImportResult *result) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        if (result) {
            result->total_lines = 0;
            result->success_count = 0;
            result->error_count = 1;
            result->duplicate_count = 0;
            snprintf(result->error_msg, sizeof(result->error_msg), 
                     "Cannot open file: %s", filepath);
        }
        return 0;
    }

    if (result) {
        result->total_lines = 0;
        result->success_count = 0;
        result->error_count = 0;
        result->duplicate_count = 0;
        result->error_msg[0] = '\0';
    }

    char line[4096];
    int count = 0;
    int line_num = 0;

    /* Skip header */
    if (fgets(line, sizeof(line), fp) == NULL) {
        fclose(fp);
        return 0;
    }

    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        if (result) result->total_lines++;

        size_t len = strlen(line);
        while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r'))
            line[--len] = '\0';

        if (len == 0) continue;

        Contact c;
        memset(&c, 0, sizeof(Contact));

        int parse_result = parse_csv_line(line, &c);
        if (parse_result != 0) {
            if (result) {
                result->error_count++;
                /* Append error info */
                int msg_len = (int)strlen(result->error_msg);
                if (msg_len < (int)sizeof(result->error_msg) - 50) {
                    snprintf(result->error_msg + msg_len, 
                             sizeof(result->error_msg) - msg_len,
                             "Line %d: parse error (%d); ", line_num, parse_result);
                }
            }
            continue;
        }

        /* Check for duplicates */
        if (search_is_duplicate(list, &c)) {
            if (result) result->duplicate_count++;
            continue;
        }

        list_append(list, &c);
        count++;
        if (result) result->success_count++;
    }

    fclose(fp);
    return count;
}

int fileio_save_trash(const Contact *c) {
    FILE *fp = fopen(TRASH_FILE, "a");
    if (!fp) return 0;

    /* Add header if file is empty */
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

int fileio_load_trash(LinkedList *list) {
    return fileio_load(list, TRASH_FILE);
}

int fileio_restore_from_trash(LinkedList *book, LinkedList *trash, int trash_id) {
    Node *node = list_find_by_id(trash, trash_id);
    if (!node) return 0;

    Contact c = node->data;
    c.id = book->next_id;
    c.modify_time = time(NULL);
    
    list_append(book, &c);
    list_remove_node(trash, node);
    node_free(node);
    
    /* Save updated trash */
    fileio_save(trash, TRASH_FILE);
    
    return c.id;
}

int fileio_validate_report(LinkedList *list, char *report, int report_size) {
    int issues = 0;
    int pos = 0;

    pos += snprintf(report + pos, report_size - pos, 
                    "=== Data Validation Report ===\n\n");

    /* Check empty fields */
    pos += snprintf(report + pos, report_size - pos, 
                    "[Empty Fields]\n");
    Node *cur = list->head;
    while (cur && pos < report_size - 100) {
        Contact *c = &cur->data;
        if (!c->name[0]) {
            pos += snprintf(report + pos, report_size - pos,
                           "  ID %d: empty name\n", c->id);
            issues++;
        }
        if (!c->mobile[0] && !c->phone[0]) {
            pos += snprintf(report + pos, report_size - pos,
                           "  ID %d (%s): no phone number\n", c->id, c->name);
            issues++;
        }
        cur = cur->next;
    }

    /* Check validation errors */
    pos += snprintf(report + pos, report_size - pos, 
                    "\n[Validation Errors]\n");
    cur = list->head;
    while (cur && pos < report_size - 100) {
        Contact *c = &cur->data;
        int r;
        if (c->mobile[0]) {
            r = validate_mobile(c->mobile);
            if (r != VALID_OK) {
                pos += snprintf(report + pos, report_size - pos,
                               "  ID %d (%s): mobile - %s\n", c->id, c->name, 
                               validate_error_msg(r));
                issues++;
            }
        }
        if (c->email[0]) {
            r = validate_email(c->email);
            if (r != VALID_OK) {
                pos += snprintf(report + pos, report_size - pos,
                               "  ID %d (%s): email - %s\n", c->id, c->name,
                               validate_error_msg(r));
                issues++;
            }
        }
        if (c->qq[0]) {
            r = validate_qq(c->qq);
            if (r != VALID_OK) {
                pos += snprintf(report + pos, report_size - pos,
                               "  ID %d (%s): QQ - %s\n", c->id, c->name,
                               validate_error_msg(r));
                issues++;
            }
        }
        cur = cur->next;
    }

    /* Check duplicates */
    pos += snprintf(report + pos, report_size - pos, 
                    "\n[Duplicates]\n");
    cur = list->head;
    while (cur && pos < report_size - 100) {
        Node *check = cur->next;
        while (check && pos < report_size - 100) {
            if (strcmp(cur->data.name, check->data.name) == 0 &&
                cur->data.mobile[0] && check->data.mobile[0] &&
                strcmp(cur->data.mobile, check->data.mobile) == 0) {
                pos += snprintf(report + pos, report_size - pos,
                               "  ID %d and ID %d: same name+mobile (%s, %s)\n",
                               cur->data.id, check->data.id,
                               cur->data.name, cur->data.mobile);
                issues++;
            }
            check = check->next;
        }
        cur = cur->next;
    }

    if (issues == 0) {
        pos += snprintf(report + pos, report_size - pos, 
                        "\nNo issues found.\n");
    } else {
        pos += snprintf(report + pos, report_size - pos, 
                        "\nTotal issues: %d\n", issues);
    }

    return issues;
}
