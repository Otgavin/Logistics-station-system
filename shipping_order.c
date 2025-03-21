#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shipping_order.h"
#include "packages.h"
#include "price.h"

extern sqlite3 *db;

// 读取当前最大订单编号（从 SQL 读取）
long read_max_order_id() {
    sqlite3_stmt *stmt;
    long max_id = 1;  // 默认最小订单编号

    if (db) {  // 直接使用已打开的数据库连接
        const char *query = "SELECT MAX(package_id) FROM packages;";
        if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                max_id = sqlite3_column_int64(stmt, 0);
            }
        }
        sqlite3_finalize(stmt);
    }

    return (max_id > 0) ? max_id : 1;  // 确保最小订单编号
}


// 输入寄件人信息
void input_sender_info(Sender *sender) {
    select_province(sender->province, sizeof(sender->province)); // 选择省份

    printf("请输入寄件人姓名: ");
    fgets(sender->name, sizeof(sender->name), stdin);
    sender->name[strcspn(sender->name, "\n")] = '\0';

    printf("请输入寄件人地址: ");
    fgets(sender->address, sizeof(sender->address), stdin);
    sender->address[strcspn(sender->address, "\n")] = '\0';

    printf("请输入寄件人手机号: ");
    fgets(sender->phone_number, sizeof(sender->phone_number), stdin);
    sender->phone_number[strcspn(sender->phone_number, "\n")] = '\0';
}

// 输入收件人信息
void input_recipient_info(Recipient *recipient) {
    select_province(recipient->province, sizeof(recipient->province)); // 选择省份

    printf("请输入收件人姓名: ");
    fgets(recipient->name, sizeof(recipient->name), stdin);
    recipient->name[strcspn(recipient->name, "\n")] = '\0';

    printf("请输入收件人地址: ");
    fgets(recipient->address, sizeof(recipient->address), stdin);
    recipient->address[strcspn(recipient->address, "\n")] = '\0';

    printf("请输入收件人手机号: ");
    fgets(recipient->phone_number, sizeof(recipient->phone_number), stdin);
    recipient->phone_number[strcspn(recipient->phone_number, "\n")] = '\0';
}


void select_province(char *province, size_t size) {
    printf("📍 请选择省份：\n");
    for (int i = 1; i < province_count+1; i++) {
        printf("%2d. %s    ", i, province_coords[i-1].province);
        if(i % 5 == 0 && i != 0) printf("\n");
    }

    int choice;
    printf("\n请输入省份编号 (1-%d): ", province_count);
    scanf("%d", &choice);
    getchar();  // 清除回车

    if (choice >= 1 && choice <= province_count) {
        strncpy(province, province_coords[choice - 1].province, size);
    } else {
        printf("❌ 无效选择，默认设置为“未知”。\n");
        strncpy(province, "未知", size);
    }
}


// 输入物品信息（重量，体积，特殊属性等）
void input_item_info(Item *item) {
    printf("请输入物品类型: ");
    fgets(item->type, sizeof(item->type), stdin);
    item->type[strcspn(item->type, "\n")] = 0;

    printf("请输入物品名称: ");
    fgets(item->name, sizeof(item->name), stdin);
    item->name[strcspn(item->name, "\n")] = 0;

    printf("请输入物品重量 (kg): ");
    scanf("%lf", &item->weight);

    printf("请输入物品体积 (立方米): ");
    scanf("%lf", &item->volume);

    char fragile_input;
    printf("该物品是否易碎? (Y/N): ");
    getchar();
    scanf("%c", &fragile_input);
    getchar();
    item->is_fragile = (fragile_input == 'Y' || fragile_input == 'y') ? 1 : 0;

    select_special_property(item->special_property, sizeof(item->special_property));
}

// 选择取件方式
int select_pickup_method() {
    char input[20];
    int choice = -1;

    while (1) {
        printf("请选择取件方式：\n");
        printf("  1. 上门取件\n");
        printf("  2. 自寄\n");
        printf("  Q. 退出\n");
        printf("请输入选项（1/2/Q）：");

        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';  // 去除换行

        // 判断是否是退出
        if (strcasecmp(input, "q") == 0) {
            printf("已选择退出。\n");
            return 0;
        }

        // 尝试解析为数字
        if (sscanf(input, "%d", &choice) == 1) {
            if (choice == 1 || choice == 2) {
                return choice;
            }
        }

        printf("❌ 无效输入，请重新输入。\n\n");
    }
}



// 显示订单总结并确认
int display_order_summary(Package *pkg, const Users *user, int pickup_method) {
    printf("\n📦 订单信息总结：\n");

    printf("寄件人：%s（%s）\n", pkg->sender.name, pkg->sender.province);
    printf("地址：%s\n电话：%s\n\n", pkg->sender.address, pkg->sender.phone_number);

    printf("收件人：%s（%s）\n", pkg->recipient.name, pkg->recipient.province);
    printf("地址：%s\n电话：%s\n\n", pkg->recipient.address, pkg->recipient.phone_number);

    printf("物品名称：%s\n类型：%s\t特殊属性：%s\n", pkg->item.name, pkg->item.type, pkg->item.special_property);
    printf("重量：%.2f kg\t体积：%.2f m³\n是否易碎：%s\n\n",
           pkg->item.weight,
           pkg->item.volume,
           pkg->item.is_fragile ? "是" : "否");

    if (pickup_method == 1) {
        printf("📌 取件方式：上门取件（将加收 5 元）\n");
    } else if (pickup_method == 2) {
        printf("📌 取件方式：自寄\n");
    }

    // 运费预估
    Price p = calculate_price(&pkg->item, user, pkg->sender.province, pkg->recipient.province);

    if (pickup_method == 1) {
        p.original_price += 5.0;
        p.price += 5.0;
    }

    printf("💰 运费明细：\n");
    printf("原价：%.2f 元\n", p.original_price);
    printf("会员折扣：%.2f 元\n", p.member_discount);
    printf("优惠券抵扣：%.2f 元\n", p.coupon_discount);
    printf("应付金额：%.2f 元\n", p.price);

    printf("\n✅ 是否确认订单？ (Y: 确认, C: 更改, Q: 退出): ");
    char input[10];
    fgets(input, sizeof(input), stdin);

    if (input[0] == 'Y' || input[0] == 'y') return 1;
    if (input[0] == 'C' || input[0] == 'c') return 2;
    return 0;
}







// 寄快递主程序
void handle_shipping_order(Users *user) {
    Package pkg;
    pkg.status = 1;
    memset(pkg.claim_code, 0, sizeof(pkg.claim_code));

    int pickup_method = 0;

    while (1) {
        printf("\n📦 请输入寄件人信息：\n");
        input_sender_info(&pkg.sender);

        printf("\n📦 请输入收件人信息：\n");
        input_recipient_info(&pkg.recipient);

        printf("\n📦 请输入物品信息：\n");
        input_item_info(&pkg.item);

        pickup_method = select_pickup_method();
        if (pickup_method == 0) {
            printf("❌ 取消订单，返回主界面。\n");
            return;
        }

        int confirm = display_order_summary(&pkg, user, pickup_method);
        if (confirm == 0) {
            printf("❌ 订单已取消。\n");
            return;
        } else if (confirm == 2) {
            printf("🔁 返回重新编辑订单信息。\n");
            continue;
        } else {
            break;
        }
    }

    // 生成订单号
    pkg.package_id = read_max_order_id() + 1;
    printf("✅ 订单已创建，订单编号为：%ld\n", pkg.package_id);

    // 运费计算（基础）
    Price final_price = calculate_price(&pkg.item, user, pkg.sender.province, pkg.recipient.province);

    // ✅ 上门取件加价
    if (pickup_method == 1) {
        final_price.price += 5.0;
        final_price.original_price += 5.0;
        printf("🚚 上门取件服务已加收 5 元。\n");
    }

    // ✅ 查询并使用优惠券
    Coupon available[10];
    int coupon_count = 0;
    if (get_available_coupons_for_user(user->username, available, &coupon_count)) {
        int selected = prompt_user_choose_coupon(available, coupon_count);
        if (selected >= 0) {
            double discount = final_price.price * available[selected].discount_rate;
            final_price.coupon_discount = discount;
            final_price.price -= discount;
            mark_coupon_as_used(available[selected].code);
            printf("🎫 使用优惠券 \"%s\"，优惠 %.2f 元\n", available[selected].code, discount);
        }
    }

    // 模拟支付
    printf("应付金额为：%.2f 元\n", final_price.price);
    printf("请输入支付金额（模拟支付）: ");
    char input[20];
    fgets(input, sizeof(input), stdin);

    // 积分处理
    int points = (int)final_price.price;
    user->members.points += points;
    printf("🎉 支付成功，获得积分：%d，当前总积分：%d\n", points, user->members.points);

    save_package_to_db(&pkg);
}
