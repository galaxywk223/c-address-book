# Address Book Manager

[中文文档](./README.md)

A robust C-based console address book management system with CSV storage, designed for reliability and long-term use.

## Features

### Core Operations
- **Add Contact**: Interactive input with validation and duplicate detection
- **Batch Add**: Add multiple contacts at once
- **Browse**: Paginated display with sorting (by name, mobile, company, category)
- **Search**: Keyword search and category filter
- **Modify**: Update contact with auto-backup
- **Delete**: Move to trash with backup (recoverable)

### Data Management
- **CSV Storage**: UTF-8 with BOM, handles Chinese, commas, quotes, newlines
- **Auto Backup**: Automatic backup before delete/modify/import operations
- **Recycle Bin**: Deleted contacts stored in trash.csv, can be restored
- **Import/Export**: Robust CSV import with error reporting
- **Validation Report**: Check for empty fields, format errors, duplicates

### Command Line Interface
```bash
# Show help
address_book --help

# Specify data file
address_book --data my_contacts.csv

# Import and exit
address_book --import new_contacts.csv

# Export and exit
address_book --export backup.csv

# Validate and exit
address_book --validate
```

## Building

### Requirements
- Windows 10/11
- Visual Studio 2019/2022 (with C++ desktop development)
- CMake 3.10+

### Build with CMake
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 18 2026" -A x64
cmake --build . --config Release
```

### Run
```bash
cd build
.\Release\address_book.exe
```

## Menu Options

```
  Address Book Manager
  Contacts: 12 | File: data/contacts.csv

    1. Add Contact
    2. Batch Add
    3. Browse
    4. Search
    5. Modify
    6. Delete
    7. Save
    8. Import
    9. Export
   10. Statistics
   11. Trash (View/Restore)
   12. Validate Data
    0. Exit
```

## Browse Mode Keys
- `N` - Next page
- `P` - Previous page
- `F` - First page
- `L` - Last page
- `S` - Sort (by name, mobile, company, or category)
- `V` - View contact details
- `Q` - Return to main menu

## Data Format

CSV file with UTF-8 BOM encoding:
```
id,name,company,phone,mobile,category,email,qq,address,note,create_time,modify_time
1,John Doe,Acme Inc,010-12345678,13800138000,2,john@example.com,123456,123 Main St,Colleague,1717000000,1717000000
```

Category codes:
- `0` = Family
- `1` = Friend
- `2` = Work
- `3` = Business
- `4` = Other

## Data Recovery

### Automatic Backups
The system automatically creates backups in `data/backup/` before:
- Deleting contacts
- Modifying contacts
- Importing data
- Program startup

Backup files are named with timestamps: `contacts_20240101_120000.csv`

### Trash/Recycle Bin
Deleted contacts are moved to `data/trash.csv`. Use menu option 11 to view and restore them.

### Manual Recovery
If the main data file is corrupted:
1. Check `data/backup/` for recent backups
2. Copy a backup to `data/contacts.csv`
3. Restart the program

## Validation Report

Menu option 12 checks for:
- Empty required fields (name, phone numbers)
- Invalid phone/email/QQ formats
- Duplicate contacts (same name + mobile)

## Project Structure

```
06-c-address-book/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── contact.h
│   ├── list.h
│   ├── fileio.h
│   ├── search.h
│   ├── validate.h
│   └── utils.h
├── src/
│   ├── main.c
│   ├── contact.c
│   ├── list.c
│   ├── fileio.c
│   ├── search.c
│   ├── validate.c
│   └── utils.c
├── data/
│   ├── contacts.csv
│   ├── trash.csv
│   └── backup/
└── docs/
    └── data_format.md
```

## Technical Details

- **Data Structure**: Doubly linked list for efficient insert/delete/sort
- **Sorting**: Merge sort (stable, O(n log n))
- **CSV Parsing**: Handles quoted fields, escaped quotes, embedded commas
- **Memory**: Dynamic allocation with proper cleanup on exit
- **Encoding**: UTF-8 with BOM for Excel compatibility

## License

This is a coursework project for educational purposes.
