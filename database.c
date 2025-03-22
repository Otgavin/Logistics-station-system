#include <stdio.h>
#include "database.h"

sqlite3 *db = NULL;

int open_database() {
    if (sqlite3_open("packages.db", &db) != SQLITE_OK) {
        printf("无法打开数据库: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    // 确保表已经创建
    const char *create_table_sql =
        "CREATE TABLE IF NOT EXISTS packages ("
        "package_id INTEGER PRIMARY KEY, "
        "sender_name TEXT, sender_address TEXT, sender_phone TEXT, "
        "recipient_name TEXT, recipient_address TEXT, recipient_phone TEXT, "
        "item_type TEXT, item_name TEXT, item_weight REAL, item_volume REAL, "
        "item_is_fragile INTEGER, status INTEGER, claim_code TEXT);";
    
    if (sqlite3_exec(db, create_table_sql, 0, 0, 0) != SQLITE_OK) {
        printf("创建表失败: %s\n", sqlite3_errmsg(db));
        return 0;
    }
    
    return 1;
}

void close_database() {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

void initialize_all_tables() {
    char *err = NULL;

    // 1. 用户表
    const char *create_users =
        "CREATE TABLE IF NOT EXISTS users ("
        "username TEXT PRIMARY KEY,"
        "password TEXT NOT NULL,"
        "type INTEGER NOT NULL,"
        "phonenumber TEXT,"
        "member_level INTEGER DEFAULT 0,"
        "member_points INTEGER DEFAULT 0"
        ");";

    // 2. 优惠券表
    const char *create_coupons =
        "CREATE TABLE IF NOT EXISTS coupons ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "code TEXT UNIQUE NOT NULL,"
        "discount_rate REAL NOT NULL,"
        "is_used INTEGER DEFAULT 0,"
        "username TEXT NOT NULL"
        ");";

    // 3. 包裹表
    const char *create_packages =
        "CREATE TABLE IF NOT EXISTS packages ("
        "package_id INTEGER PRIMARY KEY,"
        "sender_name TEXT, sender_address TEXT, sender_phone TEXT,"
        "recipient_name TEXT, recipient_address TEXT, recipient_phone TEXT,"
        "item_type TEXT, item_name TEXT, item_weight REAL, item_volume REAL,"
        "item_is_fragile INTEGER, status INTEGER, claim_code TEXT"
        ");";

    // 4. 计价规则表
    const char *create_pricing =
        "CREATE TABLE IF NOT EXISTS pricing_rules ("
        "id INTEGER PRIMARY KEY CHECK(id = 1),"
        "base_price REAL NOT NULL,"
        "price_per_kg REAL NOT NULL,"
        "vol_ratio REAL NOT NULL,"
        "price_per_km REAL NOT NULL"
        ");";

    // 5. 订单表（新增）
    const char *create_orders =
        "CREATE TABLE IF NOT EXISTS orders ("
        "order_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "package_id INTEGER UNIQUE NOT NULL,"
        "username TEXT NOT NULL,"
        "pickup_method INTEGER,"
        "delivery_method INTEGER,"
        "original_price REAL,"
        "member_discount REAL,"
        "coupon_discount REAL,"
        "final_price REAL,"
        "coupon_code TEXT,"
        "created_at TEXT DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(package_id) REFERENCES packages(package_id)"
        ");";

    // 所有建表语句与名称
    const char *tables[] = {
        create_users, create_coupons, create_packages, create_pricing, create_orders
    };
    const char *table_names[] = {
        "users", "coupons", "packages", "pricing_rules", "orders"
    };

    // 创建执行
    for (int i = 0; i < 5; i++) {
        if (sqlite3_exec(db, tables[i], 0, 0, &err) != SQLITE_OK) {
            printf("❌ 创建表 %s 失败: %s\n", table_names[i], err);
            sqlite3_free(err);
        } else {
            printf("✅ 表 %s 初始化完成。\n", table_names[i]);
        }
    }
}
