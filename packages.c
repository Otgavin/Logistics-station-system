#include <stdio.h>
#include <string.h>
#include "packages.h"
#include "sqlite3.h"
#include "database.h"
#include <stdlib.h>
#include <ctype.h>
#include "utils.h"

//辅助安全输入
void safe_input(char *buffer, int size) {
    if (fgets(buffer, size, stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline
    } else {
        buffer[0] = '\0'; // Clear buffer on failure
    }
}

//保存包裹结构体到db
void save_package_to_db(Package *new_package) {
    char *errMsg = 0;
    int rc;

    if (db == NULL) {
        printf("数据库未打开，无法保存订单。\n");
        return;
    }

    // 使用事务，提高插入效率
    sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, 0);

    // 插入数据的 SQL 语句
    const char *insert_sql =
        "INSERT INTO packages (package_id, sender_name, sender_address, sender_phone, "
        "recipient_name, recipient_address, recipient_phone, "
        "item_type, item_name, item_weight, item_volume, item_is_fragile, status, claim_code) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        printf("SQL 预处理失败: %s\n", sqlite3_errmsg(db));
        return;
    }

    // 绑定参数
    sqlite3_bind_int64(stmt, 1, new_package->package_id);
    sqlite3_bind_text(stmt, 2, new_package->sender.name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, new_package->sender.address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, new_package->sender.phone_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, new_package->recipient.name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, new_package->recipient.address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, new_package->recipient.phone_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, new_package->item.type, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, new_package->item.name, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 10, new_package->item.weight);
    sqlite3_bind_double(stmt, 11, new_package->item.volume);
    sqlite3_bind_int(stmt, 12, new_package->item.is_fragile);
    sqlite3_bind_int(stmt, 13, new_package->status);
    sqlite3_bind_text(stmt, 14, new_package->claim_code, -1, SQLITE_STATIC);

    // 执行插入
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_DONE) {
        printf("订单信息已保存（SQLite）。\n");
    } else {
        printf("订单保存失败: %s\n", sqlite3_errmsg(db));
    }

    // 清理 SQL 语句
    sqlite3_finalize(stmt);

    // 提交事务
    sqlite3_exec(db, "COMMIT;", 0, 0, 0);
}

//打印包裹信息
void print_short_package_info(Package *new_package) {
    clear_screen();
    printf("---------------------------------------------------------------");
    printf("订单编号:  %ld\n", new_package->package_id);
    printf("寄件人姓名:  %s\n", new_package->sender.name);
    printf("寄件人电话:  %s\n", new_package->sender.phone_number);
    printf("收件人姓名:  %s\n", new_package->recipient.name);
    printf("收件人电话:  %s\n", new_package->recipient.phone_number);
    printf("快递状态:    %s\n", get_status_text(new_package->status));
    if (new_package->claim_code[0] != ' ' ) printf("取件码:  %s\n", new_package->claim_code);
    else printf("取件码:  暂无\n");
    printf("---------------------------------------------------------------");
}

//打印完整包裹信息
void print_full_package_info(Package *new_package) {
    clear_screen();
    printf("---------------------------------------------------------------");
    printf("订单编号:   %ld\n", new_package->package_id);
    printf("寄件人姓名:  %s\n", new_package->sender.name);
    printf("寄件人电话:  %s\n", new_package->sender.phone_number);
    printf("收件人姓名:  %s\n", new_package->recipient.name);
    printf("收件人电话:  %s\n", new_package->recipient.phone_number);
    printf("物品类型:   %s\n", new_package->item.type);
    printf("物品名称:  %s\n", new_package->item.name);
    printf("物品重量:  %.2lf 千克\n", new_package->item.weight);
    printf("物品体积:  %.2lf 立方米\n", new_package->item.volume);
    switch (new_package->item.is_fragile) {
        case 1: printf("是否易碎:  是\n"); break;
        case 0: printf("是否易碎:  否\n"); break;
        default: printf("是否易碎:  未知\n"); break;
    }
    printf("快递状态   %s\n", get_status_text(new_package->status));
    if (strlen(new_package->claim_code) > 0)
        printf("取件码:  %s\n", new_package->claim_code);
    else
        printf("取件码:  暂无\n");
    printf("---------------------------------------------------------------");
}

//根据订单编号查询，返回包裹结构体
Package find_package_by_id(const long id) {
    const char *query = "SELECT * FROM packages WHERE package_id = ?;";
    return execute_single_result_query_int(db, query, id);
}

//根据取件码查询，返回包裹结构体
Package find_package_by_claim_code(const char *code) {
    const char *query = "SELECT * FROM packages WHERE claim_code = ?;";
    return execute_single_result_query_text(db, query, code);
}

//根据收件人电话号查询，返回包裹结构体数组
Package* find_packages_by_recipient_phone_number(const char *phone_number, int *count) {
    const char *query = "SELECT * FROM packages WHERE recipient_phone = ?;";
    return execute_multiple_result_query(db, query, phone_number, count);
}

//根据收件人姓名查询，返回包裹结构体数组
Package* find_packages_by_recipient_name(const char *name, int *count) {
    const char *query = "SELECT * FROM packages WHERE recipient_name = ?;";
    return execute_multiple_result_query(db, query, name, count);
}

//根据寄件人姓名查询，返回包裹结构体数组
Package* find_packages_by_sender_name(const char *name, int *count) {
    const char *query = "SELECT * FROM packages WHERE sender_name = ?;";
    return execute_multiple_result_query(db, query, name, count);
}

//将sqlite结构整合为结构体
Package execute_single_result_query_int(sqlite3 *db, const char *query, long param) {
    sqlite3_stmt *stmt;
    Package package = {0};  // 初始化为空结构

    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        printf("❌ SQL 预处理失败: %s\n", sqlite3_errmsg(db));
        return package;
    }

    sqlite3_bind_int64(stmt, 1, param);  // 绑定 package_id 为 INTEGER

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        populate_package_from_stmt(&package, stmt);
    } else {
        printf("❌ 查询失败或无匹配结果。\n");
    }

    sqlite3_finalize(stmt);
    return package;
}

//将sqlite结构整合为结构体
Package execute_single_result_query_text(sqlite3 *db, const char *query, const char *param) {
    sqlite3_stmt *stmt;
    Package package = {0};  // 初始化为空结构

    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        printf("❌ SQL 预处理失败: %s\n", sqlite3_errmsg(db));
        return package;
    }

    sqlite3_bind_text(stmt, 1, param, -1, SQLITE_STATIC);  // 绑定 claim_code 为 TEXT

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        populate_package_from_stmt(&package, stmt);
    } else {
        printf("❌ 查询失败或无匹配结果。\n");
    }

    sqlite3_finalize(stmt);
    return package;
}

//将sqlite结构整合为结构体数组
Package* execute_multiple_result_query(sqlite3 *db, const char *query, const char *param, int *count) {
    sqlite3_stmt *stmt;
    Package *packages = NULL;
    *count = 0;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        printf("❌ SQL 预处理失败: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    sqlite3_bind_text(stmt, 1, param, -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Package *temp = realloc(packages, (*count + 1) * sizeof(Package));
        if (!temp) {
            printf("❌ 内存分配失败。\n");
            free(packages);
            sqlite3_finalize(stmt);
            return NULL;
        }
        packages = temp;

        Package *package = &packages[*count];
        populate_package_from_stmt(package, stmt);
        (*count)++;
    }

    sqlite3_finalize(stmt);
    return packages;
}

// 从SQLite结果集填充Package结构体
void populate_package_from_stmt(Package *package, sqlite3_stmt *stmt) {
    package->package_id = sqlite3_column_int64(stmt, 0);
    strcpy(package->sender.name, (const char *)sqlite3_column_text(stmt, 1));
    strcpy(package->sender.address, (const char *)sqlite3_column_text(stmt, 2));
    strcpy(package->sender.phone_number, (const char *)sqlite3_column_text(stmt, 3));
    strcpy(package->recipient.name, (const char *)sqlite3_column_text(stmt, 4));
    strcpy(package->recipient.address, (const char *)sqlite3_column_text(stmt, 5));
    strcpy(package->recipient.phone_number, (const char *)sqlite3_column_text(stmt, 6));
    strcpy(package->item.type, (const char *)sqlite3_column_text(stmt, 7));
    strcpy(package->item.name, (const char *)sqlite3_column_text(stmt, 8));
    package->item.weight = sqlite3_column_double(stmt, 9);
    package->item.volume = sqlite3_column_double(stmt, 10);
    package->item.is_fragile = sqlite3_column_int(stmt, 11);
    package->status = sqlite3_column_int(stmt, 12);
    strcpy(package->claim_code, (const char *)sqlite3_column_text(stmt, 13));
}

// 获取订单状态的文本表示
const char* get_status_text(int status) {
    switch (status) {
        case 1: return "运输中";
        case 2: return "待入库";
        case 3: return "待取件";
        case 4: return "已取件";
        case 5: return "异常单";
        default: return "未知单";
    }
}

// 分页查询包裹信息
int list_packages(int page, const char *order_by) {
    if (db == NULL) {
        printf("数据库未打开！\n");
        return 0; // Indicate failure
    }

    // 验证 order_by 是否为有效的列名
    char check_query[256];
    snprintf(check_query, sizeof(check_query),
             "PRAGMA table_info(packages);");

    sqlite3_stmt *check_stmt;
    int column_exists = 0;

    if (sqlite3_prepare_v2(db, check_query, -1, &check_stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(check_stmt) == SQLITE_ROW) {
            const char *column_name = (const char *)sqlite3_column_text(check_stmt, 1);
            if (strcmp(column_name, order_by) == 0) {
                column_exists = 1;
                break;
            }
        }
    }
    sqlite3_finalize(check_stmt);

    if (!column_exists) {
        printf("错误: 排序字段 '%s' 无效！\n", order_by);
        return 0; // Indicate failure
    }

    // 计算偏移量
    int offset = (page - 1) * PAGE_SIZE;
    char query[256];
    snprintf(query, sizeof(query), 
        "SELECT package_id, sender_name, recipient_name, status, claim_code FROM packages ORDER BY %s LIMIT %d OFFSET %d;", 
        order_by, PAGE_SIZE, offset);

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        printf("查询失败: %s\n", sqlite3_errmsg(db));
        return 0; // Indicate failure
    }

    clear_screen();
    printf("\n--- 包裹列表 (第 %d 页) ---\n", page);
    printf("+------------+------------------+------------------+----------+------------+\n");
    printf("| 订单编号   | 发件人           | 收件人           | 状态     | 取件码     |\n");
    printf("+------------+------------------+------------------+----------+------------+\n");
    
    int row_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        long package_id = sqlite3_column_int64(stmt, 0);
        const char *sender = (const char *)sqlite3_column_text(stmt, 1);
        const char *recipient = (const char *)sqlite3_column_text(stmt, 2);
        int status = sqlite3_column_int(stmt, 3);
        const char *claim_code = (const char *)sqlite3_column_text(stmt, 4);
    
        printf("| %-10ld | %-19s | %-19s | %-11s | %-10s |\n", 
               package_id, sender, recipient, get_status_text(status), claim_code);
    
        row_count++;
    }
    printf("+------------+------------------+------------------+----------+------------+\n");

    sqlite3_finalize(stmt);

    if (row_count == 0) {
        printf("没有更多数据。\n");
    }

    return 1; // Indicate success
}

//更新包裹状态
int update_package_status(long package_id, int new_status) {
    const char *sql = "UPDATE packages SET status = ? WHERE package_id = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, new_status);
        sqlite3_bind_int64(stmt, 2, package_id);

        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        return rc == SQLITE_DONE;
    }

    return 0;
}

// 更新包裹信息（用户选择要修改的字段）
void update_package_info(long package_id) {
    if (db == NULL) {
        printf("数据库未打开！\n");
        return;
    }

    int choice;
    char new_value[200];
    double new_double_value;
    int new_int_value;
    char query[256];

    while (1) {
        printf("\n选择要更新的字段:\n");
        printf("1. 发件人姓名\n");
        printf("2. 发件人地址\n");
        printf("3. 发件人电话\n");
        printf("4. 收件人姓名\n");
        printf("5. 收件人地址\n");
        printf("6. 收件人电话\n");
        printf("7. 物品类型\n");
        printf("8. 物品名称\n");
        printf("9. 物品重量\n");
        printf("10. 物品体积\n");
        printf("11. 是否易碎 (1: 易碎, 0: 否)\n");
        printf("12. 订单状态 (1:运输中, 2:待入库, 3:待取件, 4:已取件, 5:异常)\n");
        printf("13. 取件码\n");
        printf("0. 退出更新\n");
        printf("请输入你的选择: ");
        if (scanf("%d", &choice) != 1) {
            printf("❌ 输入无效，请输入数字。\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        getchar(); // Clear newline character

        if (choice == 0) {
            printf("更新已取消。\n");
            break;
        }

        switch (choice) {
            case 1: snprintf(query, sizeof(query), "UPDATE packages SET sender_name = ? WHERE package_id = ?;"); break;
            case 2: snprintf(query, sizeof(query), "UPDATE packages SET sender_address = ? WHERE package_id = ?;"); break;
            case 3: snprintf(query, sizeof(query), "UPDATE packages SET sender_phone = ? WHERE package_id = ?;"); break;
            case 4: snprintf(query, sizeof(query), "UPDATE packages SET recipient_name = ? WHERE package_id = ?;"); break;
            case 5: snprintf(query, sizeof(query), "UPDATE packages SET recipient_address = ? WHERE package_id = ?;"); break;
            case 6: snprintf(query, sizeof(query), "UPDATE packages SET recipient_phone = ? WHERE package_id = ?;"); break;
            case 7: snprintf(query, sizeof(query), "UPDATE packages SET item_type = ? WHERE package_id = ?;"); break;
            case 8: snprintf(query, sizeof(query), "UPDATE packages SET item_name = ? WHERE package_id = ?;"); break;
            case 9: snprintf(query, sizeof(query), "UPDATE packages SET item_weight = ? WHERE package_id = ?;"); break;
            case 10: snprintf(query, sizeof(query), "UPDATE packages SET item_volume = ? WHERE package_id = ?;"); break;
            case 11: snprintf(query, sizeof(query), "UPDATE packages SET item_is_fragile = ? WHERE package_id = ?;"); break;
            case 12: snprintf(query, sizeof(query), "UPDATE packages SET status = ? WHERE package_id = ?;"); break;
            case 13: snprintf(query, sizeof(query), "UPDATE packages SET claim_code = ? WHERE package_id = ?;"); break;
            default:
                printf("无效选择，请重新输入。\n");
                continue;
        }

        printf("请输入新的值: ");
        if (choice == 9 || choice == 10) { // Handle numeric input
            if (scanf("%lf", &new_double_value) != 1) {
                printf("❌ 输入无效，请输入数字。\n");
                while (getchar() != '\n'); // Clear input buffer
                continue;
            }
        } else if (choice == 11 || choice == 12) { // Handle integer input
            if (scanf("%d", &new_int_value) != 1) {
                printf("❌ 输入无效，请输入数字。\n");
                while (getchar() != '\n'); // Clear input buffer
                continue;
            }
        } else {
            getchar(); // Clear newline character
            safe_input(new_value, sizeof(new_value));
        }

        sqlite3_stmt *stmt;
        if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
            printf("SQL 预处理失败: %s\n", sqlite3_errmsg(db));
            return;
        }

        if (choice == 9 || choice == 10) {
            sqlite3_bind_double(stmt, 1, new_double_value);
        } else if (choice == 11 || choice == 12) {
            sqlite3_bind_int(stmt, 1, new_int_value);
        } else {
            sqlite3_bind_text(stmt, 1, new_value, -1, SQLITE_STATIC);
        }
        sqlite3_bind_int64(stmt, 2, package_id);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            printf("包裹信息更新成功！\n");
        } else {
            printf("更新失败: %s\n", sqlite3_errmsg(db));
        }

        sqlite3_finalize(stmt);
    }
}

//列举并修改包裹信息主函数
void list_and_change_packages() {
    char sort_option[50];
    printf("请选择排序方式 (package_id / sender_name / status): ");
    scanf("%s", sort_option);

    int page = 1;
    while (1) {
        // Check if the column exists before proceeding
        if (!list_packages(page, sort_option)) {
            printf("排序字段无效，操作已取消。\n");
            pause_screen();
            break;
        }

        printf("\n输入 P 翻到上一页，N 翻到下一页，输入包裹 ID 进行修改 (0 退出): ");
        char input[10];
        scanf("%s", input);

        if (input[0] == 'P' || input[0] == 'p') {
            if (page > 1) page--;
        } else if (input[0] == 'N' || input[0] == 'n') {
            page++;
        } else {
            long package_id = atol(input);
            if (package_id == 0) break;
            
            // 验证包裹ID是否存在
            Package pkg = find_package_by_id(package_id);
            if (pkg.package_id <= 0) {
                printf("❌ 包裹ID %ld 不存在!\n", package_id);
                printf("按回车键继续...");
                getchar(); // 清除回车
                getchar(); // 等待用户按回车
                continue;
            }
            update_package_info(package_id);
        }
    }
}

void display_paginated_packages(Package *packages, int count, int *page) {
    if (packages == NULL || count <= 0) {
        printf("❌ 没有可显示的包裹数据\n");
        return;
    }
    
    char page_command;
    do {
        // 修正页码范围
        int max_page = (count - 1) / PAGE_SIZE;
        if (*page < 0) *page = 0;
        if (*page > max_page) *page = max_page;

        clear_screen();
        printf("\n✅ 找到 %d 条匹配的订单 (第 %d 页 / 共 %d 页)：\n", 
               count, *page + 1, max_page + 1);
        printf("+------------+------------------+------------------+----------+------------+\n");
        printf("| 订单编号   | 发件人           | 收件人           | 状态     | 取件码     |\n");
        printf("+------------+------------------+------------------+----------+------------+\n");

        int start_index = (*page) * PAGE_SIZE;
        int end_index = start_index + PAGE_SIZE;
        if (end_index > count) {
            end_index = count;
        }

        for (int i = start_index; i < end_index; i++) {
            printf("| %-10ld | %-19s | %-19s | %-11s | %-10s |\n",
                   packages[i].package_id,
                   packages[i].sender.name,
                   packages[i].recipient.name,
                   get_status_text(packages[i].status),
                   packages[i].claim_code);
        }

        printf("+------------+------------------+------------------+----------+------------+\n");

        // 改进导航提示，增加按钮状态提示
        if (*page > 0) {
            printf("[P] 上一页 ");
        } else {
            printf("          ");
        }
        
        if (*page < max_page) {
            printf("[N] 下一页 ");
        } else {
            printf("          ");
        }
        
        printf("[Q] 退出 [L] 查看详情 [数字] 输入ID直接查看\n");
        printf("请输入指令: ");
        
        char input[20] = {0};
        if (fgets(input, sizeof(input), stdin) == NULL) {
            continue;
        }
        
        input[strcspn(input, "\n")] = '\0'; // 移除换行符
        page_command = input[0];

        if (page_command == 'N' || page_command == 'n') {
            if (*page < max_page) (*page)++;
        } else if (page_command == 'P' || page_command == 'p') {
            if (*page > 0) (*page)--;
        } else if (page_command == 'L' || page_command == 'l') {
            long package_id;
            printf("请输入要查看的订单编号: ");
            
            char id_input[20] = {0};
            if (fgets(id_input, sizeof(id_input), stdin) != NULL) {
                id_input[strcspn(id_input, "\n")] = '\0';
                package_id = atol(id_input);
                
                if (package_id > 0) {
                    Package package = find_package_by_id(package_id);
                    if (package.package_id > 0) {
                        print_full_package_info(&package);
                        printf("\n按回车键继续...");
                        getchar();
                    } else {
                        printf("❌ 未找到ID为 %ld 的包裹\n", package_id);
                        printf("按回车键继续...");
                        getchar();
                    }
                }
            }
        } else if (isdigit(page_command)) {
            // 直接处理数字输入作为包裹ID
            long package_id = atol(input);
            if (package_id > 0) {
                Package package = find_package_by_id(package_id);
                if (package.package_id > 0) {
                    print_full_package_info(&package);
                    printf("\n按回车键继续...");
                    getchar();
                } else {
                    printf("❌ 未找到ID为 %ld 的包裹\n", package_id);
                    printf("按回车键继续...");
                    getchar();
                }
            }
        }
    } while (page_command != 'Q' && page_command != 'q');
    
    // 注意：确保调用此函数的代码负责释放packages内存
}

//查询并修改包裹信息主函数
void search_and_modify_package() {
    int choice;
    char search_input[50];
    Package package;
    Package *packages;
    int count = 0;
    int page = 0;  // 当前页数
    char page_command;
    int single = 0;

    while (1) {
        printf("\n📦 请选择查询方式:\n");
        printf("1. 按订单编号查询\n");
        printf("2. 按取件码查询\n");
        printf("3. 按收件人电话查询\n");
        printf("4. 按收件人姓名查询\n");
        printf("5. 按寄件人姓名查询\n");
        printf("0. 退出\n");
        printf("请输入你的选择: ");
        if (scanf("%d", &choice) != 1) {
            printf("❌ 输入无效，请输入数字。\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        getchar();  // Clear newline character
        
        count = 0;  // Reset query count
        package.package_id = 0; // Prevent incorrect modification logic
        page = 0;  // Start from the first page

        switch (choice) {
            case 1:
                printf("请输入订单编号: ");
                safe_input(search_input, sizeof(search_input));
                package = find_package_by_id(atol(search_input));
                if (package.package_id > 0) {
                    print_full_package_info(&package);
                    printf("按Enter键继续");
                    getchar();
                    single = 1;
                } else {
                    printf("❌ 未找到该订单。\n");
                    continue;
                }
                break;

            case 2:
                printf("请输入取件码: ");
                safe_input(search_input, sizeof(search_input));
                package = find_package_by_claim_code(search_input);
                if (package.package_id > 0) {
                    print_full_package_info(&package);
                    printf("按Enter键继续");
                    getchar();
                    single = 1;
                } else {
                    printf("❌ 未找到该订单。\n");
                    continue;
                }
                break;

            case 3:
                printf("请输入收件人电话: ");
                safe_input(search_input, sizeof(search_input));
                packages = find_packages_by_recipient_phone_number(search_input, &count);
                break;

            case 4:
                printf("请输入收件人姓名: ");
                safe_input(search_input, sizeof(search_input));
                packages = find_packages_by_recipient_name(search_input, &count);
                break;

            case 5:
                printf("请输入寄件人姓名: ");
                safe_input(search_input, sizeof(search_input));
                packages = find_packages_by_sender_name(search_input, &count);
                break;

            case 0:
                printf("✅ 退出查询系统。\n");
                return;

            default:
                printf("❌ 请输入有效选项。\n");
                continue;
        }

        // 如果查询到多个订单，分页显示
        if (count > 0) {
            display_paginated_packages(packages, count, &page);
        } else {
           if (!single) printf("❌ 未找到符合条件的订单。\n");
           single = 0;
        }
    }
}



