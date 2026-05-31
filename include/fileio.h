#ifndef FILEIO_H
#define FILEIO_H

#include "list.h"

/* Data file paths */
#define DATA_DIR         "data"
#define DATA_FILE        "data/contacts.csv"
#define TRASH_FILE       "data/trash.csv"
#define BACKUP_DIR       "data/backup"

/* Import result structure */
typedef struct {
    int total_lines;      /* Total lines in file */
    int success_count;    /* Successfully imported */
    int error_count;      /* Lines with errors */
    int duplicate_count;  /* Duplicate contacts skipped */
    char error_msg[1024]; /* Error details */
} ImportResult;

/* Load contacts from CSV file */
int fileio_load(LinkedList *list, const char *filepath);

/* Save contacts to CSV file */
int fileio_save(LinkedList *list, const char *filepath);

/* Auto backup current data file */
int fileio_backup(void);

/* Export to specified file */
int fileio_export(LinkedList *list, const char *filepath);

/* Import from file with error reporting */
int fileio_import(LinkedList *list, const char *filepath, ImportResult *result);

/* Save deleted contact to trash */
int fileio_save_trash(const Contact *c);

/* Load trash contacts */
int fileio_load_trash(LinkedList *list);

/* Restore contact from trash by ID */
int fileio_restore_from_trash(LinkedList *book, LinkedList *trash, int trash_id);

/* Ensure data directories exist */
void fileio_ensure_dirs(void);

/* Data validation report */
int fileio_validate_report(LinkedList *list, char *report, int report_size);

#endif /* FILEIO_H */
