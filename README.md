# C 语言通讯录管理系统

通讯录管理系统是基于 C 语言实现的控制台课程设计项目，使用 CSV 文件保存联系人数据，重点覆盖数据结构、文件读写、输入校验、备份恢复和命令行参数处理。

## 功能特性

### 核心操作

- 新增联系人：交互式录入，包含字段校验和重复检测。
- 批量新增：连续录入多个联系人。
- 浏览联系人：分页展示，支持按姓名、手机号、公司和分类排序。
- 搜索联系人：支持关键词搜索和分类筛选。
- 修改联系人：修改前自动备份。
- 删除联系人：删除后进入回收站，可恢复。

### 数据管理

- CSV 存储：使用 UTF-8 BOM，兼容中文、逗号、引号和换行。
- 自动备份：删除、修改、导入和程序启动前自动备份。
- 回收站：删除记录写入 `trash.csv`，支持查看和恢复。
- 导入导出：支持外部 CSV 导入和数据导出。
- 数据校验：检查空字段、格式错误和重复联系人。

### 命令行模式

```bash
address_book --help
address_book --data my_contacts.csv
address_book --import new_contacts.csv
address_book --export backup.csv
address_book --validate
```

## 构建要求

- Windows 10/11
- Visual Studio 2019/2022，需安装 C++ 桌面开发组件
- CMake 3.10+

## 构建方式

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 18 2026" -A x64
cmake --build . --config Release
```

## 运行方式

```powershell
cd build
.\Release\address_book.exe
```

## 主菜单

```text
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

## 浏览模式快捷键

| 按键 | 功能 |
| --- | --- |
| `N` | 下一页 |
| `P` | 上一页 |
| `F` | 首页 |
| `L` | 尾页 |
| `S` | 排序 |
| `V` | 查看联系人详情 |
| `Q` | 返回主菜单 |

## 数据格式

CSV 文件使用 UTF-8 BOM 编码：

```csv
id,name,company,phone,mobile,category,email,qq,address,note,create_time,modify_time
1,John Doe,Acme Inc,010-12345678,13800138000,2,john@example.com,123456,123 Main St,Colleague,1717000000,1717000000
```

分类编码：

| 编码 | 分类 |
| --- | --- |
| `0` | 家人 |
| `1` | 朋友 |
| `2` | 工作 |
| `3` | 商务 |
| `4` | 其他 |

## 数据恢复

### 自动备份

系统会在以下操作前创建备份：

- 删除联系人。
- 修改联系人。
- 导入数据。
- 程序启动。

备份文件位于 `data/backup/`，文件名格式为 `contacts_YYYYMMDD_HHMMSS.csv`。

### 回收站

删除的联系人会移动到 `data/trash.csv`。主菜单第 11 项用于查看和恢复回收站记录。

### 手动恢复

主数据文件损坏时，可从 `data/backup/` 选择最近备份并复制为 `data/contacts.csv`，随后重新启动程序。

## 数据校验

主菜单第 12 项会检查以下问题：

- 姓名、电话等必填字段为空。
- 电话、邮箱、QQ 格式不符合规则。
- 姓名和手机号重复。

## 项目结构

```text
c-address-book/
├── CMakeLists.txt
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
└── docs/
```

## 技术要点

- 使用双向链表组织联系人，支持高效插入、删除和排序。
- 使用归并排序完成稳定排序，复杂度为 O(n log n)。
- CSV 解析支持引号字段、转义引号和内嵌逗号。
- 动态内存分配配合统一释放流程，降低内存泄漏风险。
- UTF-8 BOM 输出提高 Excel 打开 CSV 的兼容性。

## 许可证

本项目基于 [MIT License](./LICENSE) 开源。
