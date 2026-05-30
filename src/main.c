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
static void print_main_menu(void);
static void browse_list(LinkedList *list, const char *title);

int main(void) {
    system("chcp 65001 > nul");
    list_init(&g_book);
    fileio_ensure_dirs();
    fileio_backup();
    int loaded = fileio_load(&g_book, DATA_FILE);
    if (loaded > 0) {
        printf("  Loaded %d contacts.\n", loaded);
        util_pause();
    }
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
    util_print_title("\xcd\xa8\xd1\xb6\xc2\xbc\xb9\xdc\xc0\xed\xcf\xb5\xcd\xb3");
    printf("  \xb5\xb1\xc7\xb0\xc1\xaa\xcf\xb5\xc8\xcb\xca\xfd\xc1\xbf: %d\n\n", g_book.count);
    printf("    1. \xd0\xc2\xd4\xf6\xc1\xaa\xcf\xb5\xc8\xcb\n");
    printf("    2. \xc5\xfa\xc1\xbf\xc2\xbc\xc8\xeb\n");
    printf("    3. \xc1\xf7\xc0\xad\xcd\xa8\xd1\xb6\xc2\xbc\n");
    printf("    4. \xb2\xe9\xd1\xaf\xc1\xaa\xcf\xb5\xc8\xcb\n");
    printf("    5. \xd0\xde\xb8\xc4\xc1\xaa\xcf\xb5\xc8\xcb\n");
    printf("    6. \xc9\xbe\xb3\xfd\xc1\xaa\xcf\xb5\xc8\xcb\n");
    printf("    7. \xb1\xa3\xb4\xe6\xca\xfd\xbe\xdd\n");
    printf("    8. \xb5\xbc\xc8\xeb\xca\xfd\xbe\xdd\n");
    printf("    9. \xb5\xbc\xb3\xf6\xca\xfd\xbe\xdd\n");
    printf("   10. \xb7\xd6\xc0\xe0\xcd\xb3\xbc\xc6\n");
    printf("    0. \xcd\xcb\xb3\xf6\xcf\xb5\xcd\xb3\n");
    util_print_line();
    printf("  \xc7\xeb\xd1\xa1\xd4\xf1\xb9\xa6\xc4\xdc [0-10]: ");
}

static void menu_add(void) {
    util_clear_screen();
    util_print_title("\xd0\xc2\xd4\xf6\xc1\xaa\xcf\xb5\xc8\xcb");
    Contact c;
    contact_input(&c);
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
    util_print_title("\xc5\xfa\xc1\xbf\xc2\xbc\xc8\xeb\xc1\xaa\xcf\xb5\xc8\xcb");
    printf("  Enter empty name to stop.\n\n");
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
        printf("  Category (0-4, default 4): ");
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
    browse_list(&g_book, "\xc1\xf7\xc0\xad\xcd\xa8\xd1\xb6\xc2\xbc");
}

static void menu_search(void) {
    util_clear_screen();
    util_print_title("\xb2\xe9\xd1\xaf\xc1\xaa\xcf\xb5\xc8\xcb");
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
    util_print_title("\xd0\xde\xb8\xc4\xc1\xaa\xcf\xb5\xc8\xcb");
    if (g_book.count == 0) { printf("  No records.\n"); util_pause(); return; }
    printf("  Enter ID: ");
    int id = util_get_int();
    Node *node = list_find_by_id(&g_book, id);
    if (!node) { printf("  Not found.\n"); util_pause(); return; }
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
    util_print_title("\xc9\xbe\xb3\xfd\xc1\xaa\xcf\xb5\xc8\xcb");
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
    util_print_title("\xb1\xa3\xb4\xe6\xca\xfd\xbe\xdd");
    fileio_ensure_dirs();
    if (fileio_save(&g_book, DATA_FILE)) {
        g_dirty = 0;
        printf("  Saved %d contacts to %s.\n", g_book.count, DATA_FILE);
    } else {
        printf("  Save failed!\n");
    }
    util_pause();
}

static void menu_import(void) {
    util_clear_screen();
    util_print_title("\xb5\xbc\xc8\xeb\xca\xfd\xbe\xdd");
    printf("  Enter file path: ");
    char path[256];
    util_get_line(path, sizeof(path));
    util_trim(path);
    if (path[0] == '\0') { printf("  Cancelled.\n"); util_pause(); return; }
    int imported = fileio_import(&g_book, path);
    if (imported > 0) {
        g_dirty = 1;
        printf("  Imported %d (total: %d).\n", imported, g_book.count);
    } else {
        printf("  No records imported.\n");
    }
    util_pause();
}

static void menu_export(void) {
    util_clear_screen();
    util_print_title("\xb5\xbc\xb3\xf6\xca\xfd\xbe\xdd");
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
    util_print_title("\xb7\xd6\xc0\xe0\xcd\xb3\xbc\xc6");
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
