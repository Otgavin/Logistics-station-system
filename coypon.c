#include <stdio.h>
#include <string.h>
#include <math.h>
#include "sqlite3.h"
#include "coupon.h"

extern sqlite3 *db;


// 查询用户绑定的所有未使用优惠券
int get_available_coupons_for_user(const char *username, Coupon *out_coupons, int *count) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT code, discount_rate FROM coupons WHERE username = ? AND is_used = 0;";
    int i = 0;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW && i < 10) {
            strncpy(out_coupons[i].code, (const char *)sqlite3_column_text(stmt, 0), sizeof(out_coupons[i].code));
            out_coupons[i].discount_rate = sqlite3_column_double(stmt, 1);
            out_coupons[i].is_used = 0;
            i++;
        }
        sqlite3_finalize(stmt);
        *count = i;
        return i > 0;
    }
    return 0;
}

// 提示用户选择优惠券
int prompt_user_choose_coupon(Coupon *available, int count) {
    printf("\n🎁 您有 %d 张可用优惠券：\n", count);
    for (int i = 0; i < count; i++) {
        printf("%d. %s (%.0f%% 折扣)\n", i + 1, available[i].code, available[i].discount_rate * 100);
    }
    printf("是否使用优惠券？(Y/N): ");
    char yn;
    scanf(" %c", &yn);
    getchar();

    if (yn != 'Y' && yn != 'y') return -1;

    printf("请输入要使用的优惠券编号 (1-%d): ", count);
    int index;
    scanf("%d", &index);
    getchar();

    if (index < 1 || index > count) return -1;
    return index - 1;
}


// 标记优惠券为已使用
void mark_coupon_as_used(const char *code) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE coupons SET is_used = 1 WHERE code = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, code, -1, SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

//创建一张优惠券
int create_coupon(const char *code, double rate, const char *username) {
    const char *sql = "INSERT INTO coupons (code, discount_rate, is_used, username) VALUES (?, ?, 0, ?);";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, code, -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 2, rate);
        sqlite3_bind_text(stmt, 3, username, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return 1;  // success
        }
    }
    sqlite3_finalize(stmt);
    return 0;  // fail
}

//批量创建优惠券
int create_bulk_coupon(const char *code_prefix, double rate, const char *username, int count) {
    char full_code[32];
    int success = 0;
    for (int i = 1; i <= count; i++) {
        snprintf(full_code, sizeof(full_code), "%s_%03d", code_prefix, i);
        if (create_coupon(full_code, rate, username)) {
            success++;
        }
    }
    return success;
}

//获取用户优惠券数量
int get_user_coupon_count(const char *username) {
    const char *sql = "SELECT COUNT(*) FROM coupons WHERE username = ? AND is_used = 0;";
    sqlite3_stmt *stmt;
    int count = 0;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return count;
}

//列举用户优惠券
void list_user_coupons(const char *username) {
    const char *sql = "SELECT code, discount_rate FROM coupons WHERE username = ? AND is_used = 0;";
    sqlite3_stmt *stmt;

    printf("📋 用户 [%s] 的优惠券列表：\n", username);
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *code = (const char *)sqlite3_column_text(stmt, 0);
            double rate = sqlite3_column_double(stmt, 1);
            printf("- %s （%.0f%%折扣）\n", code, rate * 100);
        }
    }
    sqlite3_finalize(stmt);
}

//删除优惠券
int delete_coupon(const char *code) {
    const char *sql = "DELETE FROM coupons WHERE code = ?;";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, code, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return 1;
        }
    }
    sqlite3_finalize(stmt);
    return 0;
}
