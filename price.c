#include "price.h"
#include "packages.h"
#include <stdio.h>


// 计算价格，根据重量和体积
double calculate_price(const Item *item) {
    const struct pricing pricing = load_pricing();
    const double volume_weight = item->volume * 10000 / pricing.vol_ratio;
    // 取实际重量和体积重量中的较大值
    const double final_weight = (item->weight > volume_weight) ? item->weight : volume_weight;

    // 按新的计价方式计算
    double price = pricing.base_price + (final_weight * pricing.price_per_kg);

    // 如果物品易碎，价格*1.5
    if (item->is_fragile) {
        price *= 1.5f;
    }

    return price;
}

// 修改计价方式并保存到 CSV 文件
void modify_pricing_rules() {
    double base_price, price_per_kg, vol_ratio;
    printf("\n--- 修改计价方式 ---\n");

    // 输入新的起步价
    printf("请输入新的起步价 (元): ");
    scanf("%lf", &base_price);

    // 输入每公斤的价格
    printf("请输入每公斤的加价 (元): ");
    scanf("%lf", &price_per_kg);

    // 输入新的抛比系数
    printf("请输入新的抛比系数: ");
    scanf("%lf", &vol_ratio);

    // 保存计价方式到 CSV 文件
    FILE *file = fopen("./data/pricing.csv", "w");
    if (file != NULL) {
        // 写表头
        fprintf(file, "base_price,price_per_kg,vol_ratio\n");
        // 写数据
        fprintf(file, "%.2f,%.2f,%.2f\n",
                base_price,
                price_per_kg,
                vol_ratio);
        fclose(file);
        printf("\n计价方式已更新并保存。\n");
    } else {
        perror("无法保存计价方式");
    }
}

// 读取计价方式（从 CSV 文件读取）
struct pricing load_pricing() {
    struct pricing pricing;
    FILE *file = fopen("./data/pricing.csv", "r");
    if (file != NULL) {
        // 先读一行表头（不做检查，可以直接略过）
        char header[128];
        fgets(header, sizeof(header), file);

        // 再读一行实际数据
        float base = 0.0f, per_kg = 0.0f, ratio = 0.0f;
        const int count = fscanf(file, "%f,%f,%f",
                                 &base, &per_kg, &ratio);
        fclose(file);

        if (count == 3) {
            pricing.base_price = base;
            pricing.price_per_kg = per_kg;
            pricing.vol_ratio = ratio;
        } else {
            // 如果格式不对，使用默认值
            pricing.base_price = 8.0;
            pricing.price_per_kg = 3.0;
            pricing.vol_ratio = 8000.0;
        }
    } else {
        // 如果文件不存在，使用默认值
        pricing.base_price = 8.0;
        pricing.price_per_kg = 3.0;
        pricing.vol_ratio = 8000.0;
    }
    return pricing;
}