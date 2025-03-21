#ifndef PRICE_H
#define PRICE_H

#include "packages.h"
#include "users.h"

//计价方式结构体
typedef struct pricing {
    double base_price;        // 起步价格
    double price_per_kg;      // 每公斤加价
    double vol_ratio;         // 体积换算重量的抛比系数
    double price_per_km;
} Pricing;

//优惠券结构体
typedef struct coupon {
    char code[20];           // 优惠券代码
    double discount_rate;    // 折扣比例（如 0.10 表示打 9 折）
    int is_used;             // 是否已使用（0 = 未使用，1 = 已使用）
} Coupon;

//价格结构体
typedef struct price {
    double original_price;     // 原价（无任何折扣）
    double member_discount;    // 会员折扣金额
    double coupon_discount;    // 优惠券折扣金额
    double price;              // 最终实际支付金额
} Price;

//省份坐标结构体
typedef struct {
    const char *province;
    double x;
    double y;
} ProvinceCoordinate;

//特殊属性结构体
typedef struct {
    const char *name;
    double surcharge_rate;
} SpecialProperty;

extern const ProvinceCoordinate province_coords[]; //省份坐标，实际定义在price.c中
extern const int province_count;

extern const SpecialProperty special_properties[]; //特殊属性，实际定义在price.c中
extern const int special_property_count;

//计价相关
Price calculate_price(const Item *item, const Users *user, const char *sender_province, const char *recipient_province);
void modify_pricing_rules();  // 修改计价方式的函数声明
struct pricing load_pricing();


//优惠券相关
int get_available_coupons_for_user(const char *username, Coupon *out_coupons, int *count);
int prompt_user_choose_coupon(Coupon *available, int count);
void mark_coupon_as_used(const char *code);


//考虑包裹自身属性
double get_surcharge_rate(const char *property);
void select_special_property(char *out_property, size_t size);


//根据省份距离计价
int get_province_coords(const char *name, double *x, double *y);
double calculate_distance_between_provinces(const char *prov1, const char *prov2);

#endif //PRICE_H

