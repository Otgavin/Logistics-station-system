#include <stdio.h>
#include <string.h>
#include <math.h>
#include "price.h"
#include "packages.h"
#include "users.h"
#include "sqlite3.h"
#include "coupon.h"


extern sqlite3 *db; //数据库连接，实际定义在database.c中

// 计算价格，不考虑优惠券
Price calculate_price(const Item *item, const Users *user, const char *sender_province, const char *recipient_province) {
    struct pricing pricing = load_pricing(); //加载计价方式
    double volume_weight = item->volume * 10000 / pricing.vol_ratio;
    double final_weight = (item->weight > volume_weight) ? item->weight : volume_weight; //根据抛比系数觉得用体积还是重量计价

    double base = pricing.base_price + final_weight * pricing.price_per_kg;
    if (item->is_fragile) base *= 1.2; //易碎加价

    base *= get_surcharge_rate(item->special_property); //基础价格乘以特殊属性的加价倍率

    // 加上地址距离费用
    double distance = calculate_distance_between_provinces(sender_province, recipient_province);
    double distance_km = distance * 111.0; // 坐标中使用了维度，这里1度约为111公里，更加直观
    base += distance_km * pricing.price_per_km;

    // 会员折扣
    double member_rate = 1.0;
    switch (user->members.level) {
        case 1: member_rate = 0.95; break;
        case 2: member_rate = 0.90; break;
        case 3: member_rate = 0.85; break;
        case 4: member_rate = 0.80; break;
        default: break;
    }

    double after_member = base * member_rate;
    double member_discount = base - after_member;

    Price result = {
        .original_price = base,
        .member_discount = member_discount,
        .coupon_discount = 0.0,
        .price = after_member
    };
    return result;
}

//修改计价方式
void modify_pricing_rules() {
    double base_price, price_per_kg, vol_ratio, price_per_km;
    printf("\n--- 修改计价方式 ---\n");

    printf("请输入新的起步价 (元): ");
    scanf("%lf", &base_price);

    printf("请输入每公斤的加价 (元): ");
    scanf("%lf", &price_per_kg);

    printf("请输入新的抛比系数: ");
    scanf("%lf", &vol_ratio);

    printf("请输入每公里加价 (元): ");
    scanf("%lf", &price_per_km);

    const char *replace_sql =
        "REPLACE INTO pricing_rules (id, base_price, price_per_kg, vol_ratio, price_per_km) "
        "VALUES (1, ?, ?, ?, ?);";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, replace_sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, base_price);
        sqlite3_bind_double(stmt, 2, price_per_kg);
        sqlite3_bind_double(stmt, 3, vol_ratio);
        sqlite3_bind_double(stmt, 4, price_per_km);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            printf("✅ 计价规则已保存至数据库。\n");
        } else {
            printf("❌ 保存失败: %s\n", sqlite3_errmsg(db));
        }
        sqlite3_finalize(stmt);
    } else {
        printf("❌ SQL 预处理失败: %s\n", sqlite3_errmsg(db));
    }
}


// 读取计价方式，返回 Pricing 结构体
Pricing load_pricing() {
    Pricing pricing = {
        .base_price = 8.0,
        .price_per_kg = 3.0,
        .vol_ratio = 8000.0,
        .price_per_km = 0.05   // 默认每公里价格
    };

    const char *query_sql =
        "SELECT base_price, price_per_kg, vol_ratio, price_per_km FROM pricing_rules WHERE id = 1;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query_sql, -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            pricing.base_price = sqlite3_column_double(stmt, 0);
            pricing.price_per_kg = sqlite3_column_double(stmt, 1);
            pricing.vol_ratio = sqlite3_column_double(stmt, 2);
            pricing.price_per_km = sqlite3_column_double(stmt, 3);
        }
        sqlite3_finalize(stmt);
    } else {
        printf("⚠️ 无法读取计价规则，使用默认值。\n");
    }

    return pricing;
}


// 获取特殊属性的加价倍率
double get_surcharge_rate(const char *property) {
    for (int i = 0; i < special_property_count; ++i) {
        if (strcmp(property, special_properties[i].name) == 0) {
            return special_properties[i].surcharge_rate;
        }
    }
    return 1.00; //默认不加价
}

// 用户选择特殊属性
void select_special_property(char *out_property, size_t size) {
    printf("\n📦 请选择物品的特殊属性（可选）：\n");
    for (int i = 0; i < special_property_count; i++) {
        printf("%d. %s\n", i + 1, special_properties[i].name);
    }
    printf("%d. 自定义输入\n", special_property_count + 1);
    printf("请输入选项 (1-%d): ", special_property_count + 1);

    int option;
    scanf("%d", &option);
    getchar();

    if (option >= 1 && option <= special_property_count) {
        strncpy(out_property, special_properties[option - 1].name, size);
    } else if (option == special_property_count + 1) {
        printf("请输入自定义特殊属性: ");
        fgets(out_property, (int)size, stdin);
        out_property[strcspn(out_property, "\n")] = 0;
    } else {
        printf("无效选项，默认设置为“无”。\n");
        strncpy(out_property, "无", size);
    }
}

//省份坐标的具体定义（经纬度）
const ProvinceCoordinate province_coords[] = {
    {"北京", 116.4, 39.9},
    {"天津", 117.2, 39.1},
    {"上海", 121.5, 31.2},
    {"重庆", 106.5, 29.6},
    {"黑龙江", 126.6, 45.7},
    {"吉林", 125.3, 43.9},
    {"辽宁", 123.4, 41.8},
    {"河北", 114.5, 38.0},
    {"山西", 112.6, 37.9},
    {"陕西", 108.9, 34.3},
    {"甘肃", 103.8, 36.1},
    {"青海", 101.8, 36.6},
    {"山东", 117.0, 36.6},
    {"河南", 113.7, 34.8},
    {"江苏", 118.8, 32.0},
    {"安徽", 117.2, 31.8},
    {"湖北", 114.3, 30.6},
    {"浙江", 120.2, 29.1},
    {"福建", 119.3, 26.1},
    {"江西", 115.9, 28.7},
    {"湖南", 112.9, 28.2},
    {"贵州", 106.7, 26.6},
    {"四川", 104.1, 30.6},
    {"云南", 102.7, 25.0},
    {"广东", 113.3, 23.1},
    {"广西", 108.3, 22.8},
    {"海南", 110.3, 20.0},
    {"西藏", 91.1, 29.6},
    {"宁夏", 106.2, 38.5},
    {"新疆", 87.6, 43.8},
    {"香港", 114.2, 22.3},
    {"澳门", 113.5, 22.2},
    {"台湾", 121.5, 25.0},
    {"内蒙古", 111.7, 40.8}
};
const int province_count = sizeof(province_coords) / sizeof(ProvinceCoordinate); //省份数量

//根据省份名称获取坐标
int get_province_coords(const char *name, double *x, double *y) {
    for (int i = 0; i < province_count; i++) {
        if (strcmp(province_coords[i].province, name) == 0) {
            *x = province_coords[i].x;
            *y = province_coords[i].y;
            return 1;
        }
    }
    return 0;
}

//计算两个省份之间的距离（实际返回的是经纬度的差距，后续转化为公里）
double calculate_distance_between_provinces(const char *prov1, const char *prov2) {
    double x1, y1, x2, y2;
    if (!get_province_coords(prov1, &x1, &y1) || !get_province_coords(prov2, &x2, &y2)) {
        return 0.0;
    }
    double dx = x1 - x2;
    double dy = y1 - y2;
    return sqrt(dx * dx + dy * dy);
}

//特殊属性的具体定义
const SpecialProperty special_properties[] = {
    {"生鲜", 1.00},
    {"液体", 1.05},
    {"易燃", 1.15},
    {"易腐", 1.00},
    {"贵重物品", 1.10},
    {"危险品", 1.20},
    {"医疗用品", 1.08},
    {"易爆", 1.20},
    {"无", 1.00}
};
const int special_property_count = sizeof(special_properties) / sizeof(SpecialProperty); //特殊属性数量