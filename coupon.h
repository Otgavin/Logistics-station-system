#ifndef COUPON_H
#define COUPON_H

//优惠券结构体
typedef struct coupon {
    char code[20];           // 优惠券代码
    double discount_rate;    // 折扣比例（如 0.10 表示打 9 折）
    int is_used;             // 是否已使用（0 = 未使用，1 = 已使用）
} Coupon;


//优惠券相关
int get_available_coupons_for_user(const char *username, Coupon *out_coupons, int *count);
int prompt_user_choose_coupon(Coupon *available, int count);
void mark_coupon_as_used(const char *code);
int create_coupon(const char *code, double rate, const char *username);
int create_bulk_coupon(const char *code_prefix, double rate, const char *username, int count);
int get_user_coupon_count(const char *username);
void list_user_coupons(const char *username);
int delete_coupon(const char *code);
#endif // COUPON_H