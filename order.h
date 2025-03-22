#ifndef ORDER_H
#define ORDER_H

#include "sqlite3.h"
#include "packages.h"
#include "users.h"
#include "price.h"

//订单结构体
typedef struct {
    int order_id;
    long package_id;
    char username[50];
    int pickup_method;
    int delivery_method;

    double original_price;
    double member_discount;
    double coupon_discount;
    double final_price;

    char coupon_code[20];
    char created_at[32];
} Order;

extern sqlite3 *db; //数据库连接，实际定义在database.c中

// 保存订单（创建订单）
void create_order_for_package(const Package *pkg, const Users *user, int pickup_method, int delivery_method, Price price, const char *coupon_code);

// 查询订单（按订单号、包裹号、用户名）
void fill_order_from_stmt(Order *order, sqlite3_stmt *stmt);
Order find_order_by_order_id(int order_id);
Order find_order_by_package_id(long package_id);
Order* find_orders_by_username(const char *username, int *count);

// 通用内部接口
Order execute_order_query_single(sqlite3 *db, const char *sql, const char *param);
Order* execute_order_query_multiple(sqlite3 *db, const char *sql, const char *param, int *count);










#endif// ORDER_H