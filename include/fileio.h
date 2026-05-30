#ifndef FILEIO_H
#define FILEIO_H

#include "list.h"

/* 数据文件路径（全局） */
#define DATA_DIR         "data"
#define DATA_FILE        "data/contacts.csv"
#define TRASH_FILE       "data/trash.csv"
#define BACKUP_DIR       "data/backup"

/* 从 CSV 加载联系人到链表 */
int fileio_load(LinkedList *list, const char *filepath);

/* 保存链表到 CSV 文件 */
int fileio_save(LinkedList *list, const char *filepath);

/* 自动备份当前数据文件 */
int fileio_backup(void);

/* 导出到指定文件 */
int fileio_export(LinkedList *list, const char *filepath);

/* 从指定文件导入（追加到链表，去重） */
int fileio_import(LinkedList *list, const char *filepath);

/* 保存删除的联系人到回收站 */
int fileio_save_trash(const Contact *c);

/* 确保数据目录存在 */
void fileio_ensure_dirs(void);

#endif /* FILEIO_H */
