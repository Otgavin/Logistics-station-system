#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "shipping_order.h"
#include "packages.h"
#include "price.h"
#include "sqlite3.h"
#include "order.h"

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

// 检查输入的合法性
int validate_input(const char *input, int min_length, int max_length, int numeric_only) {
    // Check for NULL input
    if (input == NULL) {
        return 0;
    }
    
    int len = strlen(input);
    
    // Check length constraints
    if (len < min_length || len > max_length) {
        return 0;
    }
    
    // Check if input only contains whitespace
    if (min_length > 0) {
        int only_whitespace = 1;
        for (int i = 0; i < len; i++) {
            if (!isspace(input[i])) {
                only_whitespace = 0;
                break;
            }
        }
        if (only_whitespace) {
            return 0;
        }
    }
    
    // Check numeric validation if required
    if (numeric_only) {
        // Allow leading '+' for phone numbers
        int start_idx = (len > 0 && input[0] == '+') ? 1 : 0;
        
        for (int i = start_idx; i < len; i++) {
            if (!isdigit(input[i])) {
                return 0;
            }
        }
    }
    
    return 1;
}

// 单独输入并验证姓名
int input_and_validate_name(char *name, size_t size, const char *person_type) {
    printf("请输入%s姓名: ", person_type);
    fgets(name, size, stdin);
    name[strcspn(name, "\n")] = '\0';
    if (!validate_input(name, 1, size - 1, 0)) {
        printf("❌ 姓名不能为空或过长，请重新输入。\n");
        return 0;
    }
    return 1;
}

// 单独输入并验证地址
int input_and_validate_address(char *address, size_t size) {
    printf("请输入具体地址: ");
    fgets(address, size, stdin);
    address[strcspn(address, "\n")] = '\0';
    if (!validate_input(address, 1, size - 1, 0)) {
        printf("❌ 地址不能为空或过长，请重新输入。\n");
        return 0;
    }
    return 1;
}

// 单独输入并验证电话号码
int input_and_validate_phone(char *phone, size_t size) {
    printf("请输入手机号: ");
    fgets(phone, size, stdin);
    phone[strcspn(phone, "\n")] = '\0';
    if (!validate_input(phone, 7, 11, 1)) {
        printf("❌ 手机号无效，请输入7-11位数字。\n");
        return 0;
    }
    return 1;
}

// 输入寄件人信息（改进版）
void input_sender_info(Sender *sender) {
    int name_valid = 0;
    int province_valid = 0;
    int address_valid = 0;
    int phone_valid = 0;
    
    while (!name_valid) {
        name_valid = input_and_validate_name(sender->name, sizeof(sender->name), "寄件人");
    }
    
    while (!province_valid) {
        select_province(sender->province, sizeof(sender->province)); // 选择省份
        province_valid = 1; // 省份选择总是有效的，因为有默认值
    }
    
    while (!address_valid) {
        address_valid = input_and_validate_address(sender->address, sizeof(sender->address));
    }
    
    while (!phone_valid) {
        phone_valid = input_and_validate_phone(sender->phone_number, sizeof(sender->phone_number));
    }
}

// 输入收件人信息（改进版）
void input_recipient_info(Recipient *recipient) {
    int name_valid = 0;
    int province_valid = 0;
    int address_valid = 0;
    int phone_valid = 0;
    
    while (!name_valid) {
        name_valid = input_and_validate_name(recipient->name, sizeof(recipient->name), "收件人");
    }
    
    while (!province_valid) {
        select_province(recipient->province, sizeof(recipient->province)); // 选择省份
        province_valid = 1; // 省份选择总是有效的，因为有默认值
    }
    
    while (!address_valid) {
        address_valid = input_and_validate_address(recipient->address, sizeof(recipient->address));
    }
    
    while (!phone_valid) {
        phone_valid = input_and_validate_phone(recipient->phone_number, sizeof(recipient->phone_number));
    }
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

// 输入并验证物品类型
int input_and_validate_item_type(char *type, size_t size) {
    printf("请输入物品类型: ");
    fgets(type, size, stdin);
    type[strcspn(type, "\n")] = '\0';
    if (strlen(type) == 0) {
        printf("❌ 物品类型不能为空，请重新输入。\n");
        return 0;
    }
    return 1;
}

// 输入并验证物品名称
int input_and_validate_item_name(char *name, size_t size) {
    printf("请输入物品名称: ");
    fgets(name, size, stdin);
    name[strcspn(name, "\n")] = '\0';
    if (strlen(name) == 0) {
        printf("❌ 物品名称不能为空，请重新输入。\n");
        return 0;
    }
    return 1;
}

// 输入并验证物品重量
int input_and_validate_item_weight(double *weight) {
    printf("请输入物品重量 (kg): ");
    if (scanf("%lf", weight) != 1 || *weight <= 0 || *weight > 1000) {
        printf("❌ 重量输入无效，请重新输入。\n");
        while (getchar() != '\n'); // 清除缓冲区
        return 0;
    }
    while (getchar() != '\n'); // 清除缓冲区
    return 1;
}

// 输入并验证物品体积
int input_and_validate_item_volume(double *volume) {
    printf("请输入物品体积 (立方米): ");
    if (scanf("%lf", volume) != 1 || *volume <= 0 || *volume > 100) {
        printf("❌ 体积输入无效，请重新输入。\n");
        while (getchar() != '\n'); // 清除缓冲区
        return 0;
    }
    while (getchar() != '\n'); // 清除缓冲区
    return 1;
}

// 输入并验证物品是否易碎
int input_and_validate_fragile(int *is_fragile) {
    char fragile_input;
    printf("该物品是否易碎? (Y/N): ");
    scanf("%c", &fragile_input);
    while (getchar() != '\n'); // 清除缓冲区
    
    if (fragile_input != 'Y' && fragile_input != 'y' && fragile_input != 'N' && fragile_input != 'n') {
        printf("❌ 输入无效，请输入 Y 或 N。\n");
        return 0;
    }
    
    *is_fragile = (fragile_input == 'Y' || fragile_input == 'y') ? 1 : 0;
    return 1;
}

// 输入物品信息（改进版）
void input_item_info(Item *item) {
    int type_valid = 0;
    int name_valid = 0;
    int weight_valid = 0;
    int volume_valid = 0;
    int fragile_valid = 0;
    
    while (!type_valid) {
        type_valid = input_and_validate_item_type(item->type, sizeof(item->type));
    }
    
    while (!name_valid) {
        name_valid = input_and_validate_item_name(item->name, sizeof(item->name));
    }
    
    while (!weight_valid) {
        weight_valid = input_and_validate_item_weight(&item->weight);
    }
    
    while (!volume_valid) {
        volume_valid = input_and_validate_item_volume(&item->volume);
    }
    
    while (!fragile_valid) {
        fragile_valid = input_and_validate_fragile(&item->is_fragile);
    }
    
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
int display_order_summary(Package *pkg, int pickup_method) {
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

        int confirm = display_order_summary(&pkg, pickup_method);
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

    pkg.package_id = read_max_order_id() + 1;
    printf("✅ 订单已创建，订单编号为：%ld\n", pkg.package_id);

    Price final_price = calculate_price(&pkg.item, user, pkg.sender.province, pkg.recipient.province);

    if (pickup_method == 1) {
        final_price.price += 5.0;
        final_price.original_price += 5.0;
        printf("🚚 上门取件服务已加收 5 元。\n");
    }

    Coupon available[10];
    int coupon_count = 0;
    char used_coupon_code[20] = "";

    if (get_available_coupons_for_user(user->username, available, &coupon_count)) {
        int selected = prompt_user_choose_coupon(available, coupon_count);
        if (selected >= 0) {
            double discount = final_price.price * available[selected].discount_rate;
            final_price.coupon_discount = discount;
            final_price.price -= discount;
            mark_coupon_as_used(available[selected].code);
            strncpy(used_coupon_code, available[selected].code, sizeof(used_coupon_code));
            printf("🎫 使用优惠券 \"%s\"，优惠 %.2f 元\n", available[selected].code, discount);
        }
    }

    printf("\n💰 运费明细：\n");
    printf("原价：%.2f 元\n", final_price.original_price);
    printf("会员折扣：%.2f 元\n", final_price.member_discount);
    printf("优惠券抵扣：%.2f 元\n", final_price.coupon_discount);
    printf("应付金额：%.2f 元\n", final_price.price);

    printf("\n请输入支付金额（模拟支付）: ");
    char input[20];
    fgets(input, sizeof(input), stdin);

    int points = (int)final_price.price;
    user->members.points += points;
    printf("🎉 支付成功，获得积分：%d，当前总积分：%d\n", points, user->members.points);

    save_package_to_db(&pkg);
    create_order_for_package(&pkg, user, pickup_method, 0, final_price, used_coupon_code);
}
