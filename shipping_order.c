#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shipping_order.h"
#include "packages.h"
#include "price.h"

// 读取当前最大订单编号（改为从 CSV 读取）
// 假设 order_id.csv 的格式只有一行，例如：
//  10000000
long read_max_order_id() {
    FILE *file = fopen("./data/order_id.csv", "r");
    long max_id = 10000000;  // 默认最小的订单编号
    if (file != NULL) {
        fscanf(file, "%ld", &max_id);
        fclose(file);
    }
    return max_id;
}

// 更新最大订单编号（写入 CSV）
void update_max_order_id(long new_max_id) {
    FILE *file = fopen("./data/order_id.csv", "w");
    if (file != NULL) {
        // 只写一个数字到文件
        fprintf(file, "%ld\n", new_max_id);
        fclose(file);
    } else {
        perror("无法更新订单编号文件");
    }
}


// 输入寄件人信息
void input_sender_info(Sender *sender) {
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

// 输入物品信息（重量、体积、是否易碎）
void input_item_info(Item *item) {
    printf("请输入物品类型: ");
    fgets(item->type, sizeof(item->type), stdin);
    item->type[strcspn(item->type, "\n")] = 0;  // 去除换行符

    printf("请输入物品名称: ");
    fgets(item->name, sizeof(item->name), stdin);
    item->name[strcspn(item->name, "\n")] = 0;  // 去除换行符

    printf("请输入物品重量 (kg): ");
    scanf("%lf", &item->weight);

    printf("请输入物品体积 (立方米): ");
    scanf("%lf", &item->volume);

    // 读取是否易碎信息
    char fragile_input;
    printf("该物品是否易碎? (Y/N): ");
    getchar();  // 清除上次输入的换行符
    scanf("%c", &fragile_input);

    if (fragile_input == 'Y' || fragile_input == 'y') {
        item->is_fragile = 1;
    } else {
        item->is_fragile = 0;
    }
}

// 选择取件方式
int select_pickup_method() {
    int choice;
    char input[10];

    printf("请选择取件方式 (1: 上门取件, 2: 自寄, Q: 退出): ");
    fgets(input, sizeof(input), stdin);
    if (input[0] == 'Q' || input[0] == 'q') {
        printf("退出到主界面\n");
        return 0;
    }

    sscanf(input, "%d", &choice);
    return choice;
}


// 显示订单总结并确认
int display_order_summary(Package *new_package) {
    printf("\n订单信息总结:\n");
    printf("寄件人: %s, %s, %s\n",
           new_package->sender.name,
           new_package->sender.address,
           new_package->sender.phone_number);

    printf("收件人: %s, %s, %s\n",
           new_package->recipient.name,
           new_package->recipient.address,
           new_package->recipient.phone_number);

    printf("物品重量: %.2f kg, 物品体积: %.2f 立方米\n",
           new_package->item.weight,
           new_package->item.volume);
    printf("是否易碎: %s\n",
           (new_package->item.is_fragile) ? "是" : "否");


    // 根据新的计价方式计算价格
    const double price = calculate_price(&(new_package->item));
    printf("预计价格: %.2f 元\n", price);

    printf("确认订单 (Y: 确认, C: 更改, Q: 退出): ");
    char input[10];
    fgets(input, sizeof(input), stdin);

    if (input[0] == 'Y' || input[0] == 'y') {
        return 1;  // 确认订单
    } else if (input[0] == 'C' || input[0] == 'c') {
        return 2;  // 更改订单
    } else {
        return 0;  // 退出
    }
}



// 主程序处理函数
void handle_shipping_order() {
    Package new_package;

    // 默认状态为运输中 (1)，并将取件码初始化为空字符串
    new_package.status = 1;  // 运输中
    memset(new_package.claim_code, 0, sizeof(new_package.claim_code));  // 取件码初始化为空字符串


    int continue_editing = 1;
    while (continue_editing) {
        // 输入寄件人信息
        input_sender_info(&new_package.sender);

        // 输入收件人信息
        input_recipient_info(&new_package.recipient);

        // 输入物品信息
        input_item_info(&new_package.item);

        // 选择取件方式
        int pickup_method = select_pickup_method();
        if (pickup_method == 0) {
            return;  // 如果用户选择退出，则返回主界面
        }

        // 显示订单总结并确认
        int confirmation = display_order_summary(&new_package);
        if (confirmation == 0) {
            printf("取消订单，返回主界面。\n");
            return;
        } else if (confirmation == 2) {
            printf("返回修改界面。\n");
            continue_editing = 1;
        } else {
            continue_editing = 0;
        }
    }

    // 获取当前最大订单编号并生成新的订单编号
    const long max_order_id = read_max_order_id();
    new_package.package_id = max_order_id + 1;  // 新订单的ID是当前最大ID加1
    printf("订单创建成功，订单编号: %ld\n", new_package.package_id);

    // 更新最大订单编号
    update_max_order_id(new_package.package_id);

    // 伪支付
    printf("请输入支付金额 (假设支付成功): ");
    char input[10];
    fgets(input, sizeof(input), stdin);

    // 保存订单信息到 CSV 文件
    save_package_to_file(&new_package);
}
