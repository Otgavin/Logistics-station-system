#include "order.h"
#include "sqlite3.h"
#include "packages.h"
#include "users.h"
#include "price.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 保存订单（创建订单）
void create_order_for_package(const Package *pkg, const Users *user, int pickup_method, int delivery_method, Price price, const char *coupon_code) {
    const char *sql =
        "INSERT INTO orders (package_id, username, pickup_method, delivery_method, "
        "original_price, member_discount, coupon_discount, final_price, coupon_code) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, pkg->package_id);
        sqlite3_bind_text(stmt, 2, user->username, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, pickup_method);
        sqlite3_bind_int(stmt, 4, delivery_method);
        sqlite3_bind_double(stmt, 5, price.original_price);
        sqlite3_bind_double(stmt, 6, price.member_discount);
        sqlite3_bind_double(stmt, 7, price.coupon_discount);
        sqlite3_bind_double(stmt, 8, price.price);
        sqlite3_bind_text(stmt, 9, coupon_code ? coupon_code : "", -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            printf("❌ 保存订单失败: %s\n", sqlite3_errmsg(db));
        }
        sqlite3_finalize(stmt);
    } else {
        printf("❌ SQL 预处理失败: %s\n", sqlite3_errmsg(db));
    }
}

// 查询单个订单 by order_id
Order find_order_by_order_id(int order_id) {
    const char *sql = "SELECT * FROM orders WHERE order_id = ?;";
    sqlite3_stmt *stmt;
    Order order = {0};

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, order_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            fill_order_from_stmt(&order, stmt);
        }
        sqlite3_finalize(stmt);
    }
    return order;
}

// 查询单个订单 by package_id
Order find_order_by_package_id(long package_id) {
    const char *sql = "SELECT * FROM orders WHERE package_id = ?;";
    sqlite3_stmt *stmt;
    Order order = {0};

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, package_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            fill_order_from_stmt(&order, stmt);
        }
        sqlite3_finalize(stmt);
    }
    return order;
}

// 查询多个订单 by username
Order* find_orders_by_username(const char *username, int *count) {
    const char *sql = "SELECT * FROM orders WHERE username = ? ORDER BY created_at DESC;";
    sqlite3_stmt *stmt;
    Order *orders = malloc(sizeof(Order) * 100);
    *count = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW && *count < 100) {
            fill_order_from_stmt(&orders[*count], stmt);
            (*count)++;
        }
        sqlite3_finalize(stmt);
    }
    return orders;
}

// 从 sqlite3_stmt 填充 Order 结构体
void fill_order_from_stmt(Order *order, sqlite3_stmt *stmt) {
    order->order_id = sqlite3_column_int(stmt, 0);
    order->package_id = sqlite3_column_int64(stmt, 1);
    strcpy(order->username, (const char *)sqlite3_column_text(stmt, 2));
    order->pickup_method = sqlite3_column_int(stmt, 3);
    order->delivery_method = sqlite3_column_int(stmt, 4);
    order->original_price = sqlite3_column_double(stmt, 5);
    order->member_discount = sqlite3_column_double(stmt, 6);
    order->coupon_discount = sqlite3_column_double(stmt, 7);
    order->final_price = sqlite3_column_double(stmt, 8);
    strcpy(order->coupon_code, (const char *)sqlite3_column_text(stmt, 9));
    strcpy(order->created_at, (const char *)sqlite3_column_text(stmt, 10));
}

// 通用：单个 Order 查询（param 是字符串）
Order execute_order_query_single(sqlite3 *db, const char *sql, const char *param) {
    sqlite3_stmt *stmt;
    Order order = {0};
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, param, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            fill_order_from_stmt(&order, stmt);
        }
        sqlite3_finalize(stmt);
    }
    return order;
}

// 通用：多个 Order 查询（param 是字符串）
Order* execute_order_query_multiple(sqlite3 *db, const char *sql, const char *param, int *count) {
    sqlite3_stmt *stmt;
    Order *orders = malloc(sizeof(Order) * 100);
    *count = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, param, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW && *count < 100) {
            fill_order_from_stmt(&orders[*count], stmt);
            (*count)++;
        }
        sqlite3_finalize(stmt);
    }
    return orders;
}



