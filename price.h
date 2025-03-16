#ifndef PRICE_H
#define PRICE_H

#include "packages.h"

struct pricing {
    float base_price;      // 起步价
    float price_per_kg;    // 每公斤加价
    float vol_ratio;       // 抛比系数
};

double calculate_price(const Item *item);
void modify_pricing_rules();  // 修改计价方式的函数声明
struct pricing load_pricing();


#endif //PRICE_H
