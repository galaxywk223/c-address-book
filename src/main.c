#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "contact.h"
#include "list.h"
#include "fileio.h"
#include "search.h"
#include "validate.h"
#include "utils.h"

static LinkedList g_book;
static int g_dirty = 0;
static char g_data_file[256] = DATA_FILE;

static void menu_add(void);
static void menu_batch_add(void);
static void menu_browse(void);
static void menu_search(void);
static void menu_modify(void);
static void menu_delete(void);
static void menu_save(void);
static void menu_import(void);
static void menu_export(void);
static void menu_stats(void);
static void menu_trash(void);
static void menu_validate(void);
static void print_main_menu(void);
static void browse_list(LinkedList *list, const char *title);
static void print_usage(const char *prog);

static void print_usage(const char *prog) {
    printf("Usage: %s [options]\n", prog);
    printf("Options:\n");
    printf("  -d, --data <file>    Specify data file path (default: data/contacts.csv)\n");
    printf("  -i, --import <file>  Import contacts from file and exit\n");
    printf("  -e, --export <file>  Export contacts to file and exit\n");
    printf("  -v, --validate       Run validation report and exit\n");
    printf("  -h, --help           Show this help message\n");
}

int main(int argc, char *argv[]) {
    system("chcp 65001 > nul");
    
    int import_mode = 0;
    int export_mode = 0;
    int validate_mode = 0;
    char import_file[256] = {0};
    char export_file[256] = {0};

    /* Parse command line arguments */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--data") == 0) {
            if (i + 1 < argc) {
                strncpy(g_data_file, argv[++i], sizeof(g_data_file) - 1);
            } else {
                fprintf(stderr, "Error: --data requires a file path\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--import") == 0) {
            if (i + 1 < argc) {
                import_mode = 1;
                strncpy(import_file, argv[++i], sizeof(import_file) - 1);
            } else {
                fprintf(stderr, "Error: --import requires a file path\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--export") == 0) {
            if (i + 1 < argc) {
                export_mode = 1;
                strncpy(export_file, argv[++i], sizeof(export_file) - 1);
            } else {
                fprintf(stderr, "Error: --export requires a file path\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--validate") == 0) {
            validate_mode = 1;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    list_init(&g_book);
    fileio_ensure_dirs();
    fileio_backup();

    int loaded = fileio_load(&g_book, g_data_file);
    if (loaded > 0) {
        printf("  Loaded %d contacts from %s.\n", loaded, g_data_file);
    }

    /* Command line mode: import */
    if (import_mode) {
        ImportResult result;
        int imported = fileio_import(&g_book, import_file, &result);
        printf("\nImport Report:\n");
        printf("  Total lines: %d\n", result.total_lines);
        printf("  Imported: %d\n", result.success_count);
        printf("  Duplicates skipped: %d\n", result.duplicate_count);
        printf("  Errors: %d\n", result.error_count);
        if (result.error_msg[0]) {
            printf("  Error details: %s\n", result.error_msg);
        }
        if (imported > 0) {
            fileio_save(&g_book, g_data_file);
            printf("  Saved to %s\n", g_data_file);
        }
        list_destroy(&g_book);
        return 0;
    }

    /* Command line mode: export */
    if (export_mode) {
        fileio_export(&g_book, export_file);
        printf("  Exported %d contacts to %s\n", g_book.count, export_file);
        list_destroy(&g_book);
        return 0;
    }

    /* Command line mode: validate */
    if (validate_mode) {
        char report[4096];
        int issues = fileio_validate_report(&g_book, report, sizeof(report));
        printf("%s\n", report);
        list_destroy(&g_book);
        return issues > 0 ? 1 : 0;
    }

    /* Interactive mode */
    util_pause();

    int choice;
    while (1) {
        print_main_menu();
        choice = util_get_int();
        switch (choice) {
            case 1:  menu_add();       break;
            case 2:  menu_batch_add(); break;
            case 3:  menu_browse();    break;
            case 4:  menu_search();    break;
            case 5:  menu_modify();    break;
            case 6:  menu_delete();    break;
            case 7:  menu_save();      break;
            case 8:  menu_import();    break;
            case 9:  menu_export();    break;
            case 10: menu_stats();     break;
            case 11: menu_trash();     break;
            case 12: menu_validate();  break;
            case 0:
                if (g_dirty) {
                    printf("\n  Unsaved changes. Save? (Y/n): ");
                    char buf[16];
                    util_get_line(buf, sizeof(buf));
                    if (buf[0] != 'n' && buf[0] != 'N') menu_save();
                }
                list_destroy(&g_book);
                printf("\n  Goodbye!\n\n");
                return 0;
            default:
                printf("\n  Invalid choice.\n");
                util_pause();
                break;
        }
    }
}

static void print_main_menu(void) {
    util_clear_screen();
    util_print_title("Address Book Manager");
    printf("  Contacts: %d | File: %s\n\n", g_book.count, g_data_file);
    printf("    1. Add Contact\n");
    printf("    2. Batch Add\n");
    printf("    3. Browse\n");
    printf("    4. Search\n");
    printf("    5. Modify\n");
    printf("    6. Delete\n");
    printf("    7. Save\n");
    printf("    8. Import\n");
    printf("    9. Export\n");
    printf("   10. Statistics\n");
    printf("   11. Trash (View/Restore)\n");
    printf("   12. Validate Data\n");
    printf("    0. Exit\n");
    util_print_line();
    printf("  Choice [0-12]: ");
}

static void menu_add(void) {
    util_clear_screen();
    util_print_title("Add Contact");
    Contact c;
    contact_input(&c);
    
    /* Duplicate check */
    if (search_is_duplicate(&g_book, &c)) {
        printf("\n  [Warning] Duplicate contact found!\n");
        printf("  Add anyway? (y/N): ");
        char buf[16];
        util_get_line(buf, sizeof(buf));
        if (buf[0] != 'y' && buf[0] != 'Y') {
            printf("  Cancelled.\n");
            util_pause();
            return;
        }
    }

    /* Backup before modify */
    fileio_backup();

    c.id = g_book.next_id;
    Node *node = list_append(&g_book, &c);
    if (node) {
        g_dirty = 1;
        printf("\n  [OK] Added \"%s\" (ID: %d).\n", c.name, c.id);
    } else {
        printf("\n  [Error] Add failed.\n");
    }
    util_pause();
}

static void menu_batch_add(void) {
    util_clear_screen();
    util_print_title("Batch Add Contacts");
    printf("  Enter empty name to stop.\n\n");
    
    /* Backup before modify */
    fileio_backup();
    
    int count = 0;
    while (1) {
        printf("  --- #%d ---\n", count + 1);
        Contact c;
        memset(&c, 0, sizeof(Contact));
        printf("  Name (empty to stop): ");
        util_get_line(c.name, MAX_NAME);
        util_trim(c.name);
        if (c.name[0] == '\0') break;
        int valid = validate_name(c.name);
        if (valid != VALID_OK) {
            printf("  [Error] %s\n\n", validate_error_msg(valid));
            continue;
        }
        printf("  Company: ");
        util_get_line(c.company, MAX_COMPANY);
        util_trim(c.company);
        printf("  Mobile: ");
        util_get_line(c.mobile, MAX_MOBILE);
        util_trim(c.mobile);
        printf("  Category (0=family 1=friend 2=work 3=biz 4=other, default 4): ");
        int cat = util_get_int_default(4);
        if (cat < 0 || cat >= CAT_COUNT) cat = CAT_OTHER;
        c.category = (Category)cat;
        printf("  Email: ");
        util_get_line(c.email, MAX_EMAIL);
        util_trim(c.email);
        printf("  QQ: ");
        util_get_line(c.qq, MAX_QQ);
        util_trim(c.qq);
        printf("  Address: ");
        util_get_line(c.address, MAX_ADDRESS);
        util_trim(c.address);
        printf("  Note: ");
        util_get_line(c.note, MAX_NOTE);
        util_trim(c.note);
        if (search_is_duplicate(&g_book, &c)) {
            printf("  [Warning] Duplicate, skipped.\n\n");
            continue;
        }
        c.create_time = time(NULL);
        c.modify_time = c.create_time;
        c.id = g_book.next_id;
        list_append(&g_book, &c);
        g_dirty = 1;
        count++;
        printf("  [Added] %s\n\n", c.name);
    }
    printf("\n  Batch done: %d added.\n", count);
    util_pause();
}

static void browse_list(LinkedList *list, const char *title) {
    if (list->count == 0) {
        printf("\n  No records.\n");
        util_pause();
        return;
    }
    int page = 1;
    int total = list->count;
    int total_pages = (total + PAGE_SIZE - 1) / PAGE_SIZE;
    while (1) {
        util_clear_screen();
        util_print_title(title);
        printf("  Sort: 1=Name 2=Mobile 3=Company 4=Category | Total: %d\n", total);
        util_print_line();
        int start = (page - 1) * PAGE_SIZE;
        int end = start + PAGE_SIZE;
        if (end > total) end = total;
        contact_print_header();
        Node *node = list_find_by_index(list, start);
        for (int i = start; i < end && node; i++) {
            contact_print_row(&node->data);
            node = node->next;
        }
        util_print_page_info(total, page, PAGE_SIZE);
        printf("\n  N=Next P=Prev F=First L=Last S=Sort V=View Q=Quit\n");
        printf("  Choice: ");
        char cmd[16];
        util_get_line(cmd, sizeof(cmd));
        if (cmd[0] == 'q' || cmd[0] == 'Q') break;
        else if (cmd[0] == 'n' || cmd[0] == 'N') { if (page < total_pages) page++; }
        else if (cmd[0] == 'p' || cmd[0] == 'P') { if (page > 1) page--; }
        else if (cmd[0] == 'f' || cmd[0] == 'F') { page = 1; }
        else if (cmd[0] == 'l' || cmd[0] == 'L') { page = total_pages; }
        else if (cmd[0] == 's' || cmd[0] == 'S') {
            printf("  Sort by (1=Name 2=Mobile 3=Company 4=Category): ");
            int sc = util_get_int();
            CompareFunc cmp = NULL;
            switch (sc) {
                case 1: cmp = cmp_by_name; break;
                case 2: cmp = cmp_by_mobile; break;
                case 3: cmp = cmp_by_company; break;
                case 4: cmp = cmp_by_category; break;
            }
            if (cmp) {
                list_sort(list, cmp);
                page = 1;
                if (list == &g_book) g_dirty = 1;
                printf("  Sorted.\n");
            }
        }
        else if (cmd[0] == 'v' || cmd[0] == 'V') {
            printf("  Enter ID: ");
            int id = util_get_int();
            Node *found = list_find_by_id(list, id);
            if (found) {
                util_clear_screen();
                util_print_title("Contact Detail");
                contact_print(&found->data);
            } else {
                printf("  Not found.\n");
            }
            util_pause();
        }
    }
}

static void menu_browse(void) {
    util_clear_screen();
    browse_list(&g_book, "Browse Contacts");
}

static void menu_search(void) {
    util_clear_screen();
    util_print_title("Search Contacts");
    printf("  1. Keyword search\n");
    printf("  2. Category filter\n");
    printf("  Choice: ");
    int mode = util_get_int();
    LinkedList results;
    list_init(&results);
    int count = 0;
    if (mode == 1) {
        SearchCriteria criteria;
        search_init(&criteria);
        printf("  Keyword: ");
        util_get_line(criteria.keyword, sizeof(criteria.keyword));
        util_trim(criteria.keyword);
        count = search_execute(&g_book, &results, &criteria);
        printf("\n  Found %d matches.\n", count);
        if (count > 0) browse_list(&results, "Search Results");
    } else if (mode == 2) {
        printf("  Category (0-4): ");
        int cat = util_get_int();
        if (cat >= 0 && cat < CAT_COUNT) {
            count = search_by_category(&g_book, &results, (Category)cat);
            printf("\n  %s: %d records.\n", category_name((Category)cat), count);
            if (count > 0) {
                char t[64];
                snprintf(t, sizeof(t), "Category: %s", category_name((Category)cat));
                browse_list(&results, t);
            }
        }
    }
    list_destroy(&results);
    util_pause();
}

static void menu_modify(void) {
    util_clear_screen();
    util_print_title("Modify Contact");
    if (g_book.count == 0) { printf("  No records.\n"); util_pause(); return; }
    printf("  Enter ID: ");
    int id = util_get_int();
    Node *node = list_find_by_id(&g_book, id);
    if (!node) { printf("  Not found.\n"); util_pause(); return; }
    
    /* Backup before modify */
    fileio_backup();
    
    printf("\n  Current info:\n");
    contact_print(&node->data);
    printf("\n  Enter new values:\n");
    contact_modify(&node->data);
    g_dirty = 1;
    printf("\n  Updated.\n");
    util_pause();
}

static void menu_delete(void) {
    util_clear_screen();
    util_print_title("Delete Contact");
    if (g_book.count == 0) { printf("  No records.\n"); util_pause(); return; }
    printf("  Enter ID: ");
    int id = util_get_int();
    Node *node = list_find_by_id(&g_book, id);
    if (!node) { printf("  Not found.\n"); util_pause(); return; }
    printf("\n  Confirm delete:\n");
    contact_print(&node->data);
    printf("  Delete? (y/N): ");
    char buf[16];
    util_get_line(buf, sizeof(buf));
    if (buf[0] == 'y' || buf[0] == 'Y') {
        /* Backup before delete */
        fileio_backup();
        fileio_save_trash(&node->data);
        list_remove_node(&g_book, node);
        node_free(node);
        g_dirty = 1;
        printf("  Deleted (backed up to trash).\n");
    } else {
        printf("  Cancelled.\n");
    }
    util_pause();
}

static void menu_save(void) {
    util_clear_screen();
    util_print_title("Save Data");
    fileio_ensure_dirs();
    if (fileio_save(&g_book, g_data_file)) {
        g_dirty = 0;
        printf("  Saved %d contacts to %s.\n", g_book.count, g_data_file);
    } else {
        printf("  Save failed!\n");
    }
    util_pause();
}

static void menu_import(void) {
    util_clear_screen();
    util_print_title("Import Data");
    
    /* Backup before import */
    fileio_backup();
    
    printf("  Enter file path: ");
    char path[256];
    util_get_line(path, sizeof(path));
    util_trim(path);
    if (path[0] == '\0') { printf("  Cancelled.\n"); util_pause(); return; }
    
    ImportResult result;
    int imported = fileio_import(&g_book, path, &result);
    
    printf("\n  Import Report:\n");
    printf("  Total lines: %d\n", result.total_lines);
    printf("  Imported: %d\n", result.success_count);
    printf("  Duplicates skipped: %d\n", result.duplicate_count);
    printf("  Errors: %d\n", result.error_count);
    if (result.error_msg[0]) {
        printf("  Error details: %s\n", result.error_msg);
    }
    
    if (imported > 0) {
        g_dirty = 1;
    }
    util_pause();
}

static void menu_export(void) {
    util_clear_screen();
    util_print_title("Export Data");
    printf("  Export path (default: data/export.csv): ");
    char path[256];
    util_get_line(path, sizeof(path));
    util_trim(path);
    if (path[0] == '\0') snprintf(path, sizeof(path), "data/export.csv");
    if (fileio_export(&g_book, path)) {
        printf("  Exported %d to %s.\n", g_book.count, path);
    } else {
        printf("  Export failed!\n");
    }
    util_pause();
}

static void menu_stats(void) {
    util_clear_screen();
    util_print_title("Category Statistics");
    int stats[CAT_COUNT];
    search_category_stats(&g_book, stats);
    printf("  Total: %d\n\n", g_book.count);
    util_print_line();
    printf("  %-10s  %s\n", "Category", "Count");
    printf("  %-10s  %s\n", "----------", "------");
    for (int i = 0; i < CAT_COUNT; i++) {
        printf("  %-10s  %d\n", category_name((Category)i), stats[i]);
    }
    util_print_line();
    printf("  %-10s  %d\n", "Total", g_book.count);
    util_pause();
}

static void menu_trash(void) {
    util_clear_screen();
    util_print_title("Trash (Recycle Bin)");
    
    LinkedList trash;
    list_init(&trash);
    int count = fileio_load_trash(&trash);
    
    if (count == 0) {
        printf("  Trash is empty.\n");
        util_pause();
        list_destroy(&trash);
        return;
    }
    
    printf("  %d contacts in trash.\n\n", count);
    
    /* Show trash */
    contact_print_header();
    Node *cur = trash.head;
    while (cur) {
        contact_print_row(&cur->data);
        cur = cur->next;
    }
    
    printf("\n  Enter ID to restore, or 0 to cancel: ");
    int id = util_get_int();
    
    if (id > 0) {
        int new_id = fileio_restore_from_trash(&g_book, &trash, id);
        if (new_id > 0) {
            g_dirty = 1;
            printf("  Restored with new ID: %d\n", new_id);
        } else {
            printf("  ID not found in trash.\n");
        }
    }
    
    list_destroy(&trash);
    util_pause();
}

static void menu_validate(void) {
    util_clear_screen();
    util_print_title("Data Validation Report");
    
    char report[4096];
    int issues = fileio_validate_report(&g_book, report, sizeof(report));
    printf("%s\n", report);
    
    if (issues > 0) {
        printf("  Found %d issues. Please fix them.\n", issues);
    } else {
        printf("  All data is valid.\n");
    }
    util_pause();
}
