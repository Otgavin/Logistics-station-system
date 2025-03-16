#include <stdio.h>
#include <string.h>
#include "packages.h"

//Save a package to file
void save_package_to_file(Package *new_package) {
    FILE *file = fopen("./data/packages.csv", "a");
    if (file != NULL) {
        // 如果文件是空的，先写表头
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        if (file_size == 0) {
            // 写表头
            fprintf(file,
                "package_id,sender_name,sender_address,sender_phone,"
                "recipient_name,recipient_address,recipient_phone,"
                "item_type,item_name,item_weight,item_volume,item_is_fragile,"
                "status,claim_code\n");
        }
        // 写入一行订单数据
        fprintf(file, "%ld,%s,%s,%s,%s,%s,%s,%s,%s,%lf,%lf,%d,%d,%s\n",
                new_package->package_id,
                new_package->sender.name,
                new_package->sender.address,
                new_package->sender.phone_number,
                new_package->recipient.name,
                new_package->recipient.address,
                new_package->recipient.phone_number,
                new_package->item.type,
                new_package->item.name,
                new_package->item.weight,
                new_package->item.volume,
                new_package->item.is_fragile,
                new_package->status,
                new_package->claim_code);
        fclose(file);
        printf("订单信息已保存（CSV）。\n");
    } else {
        perror("无法保存订单信息");
    }
}

//Print package info
void print_short_package_info(Package *new_package) {
    printf("---------------------------------------------------------------");
    printf("订单编号:  %ld\n", new_package->package_id);
    printf("寄件人姓名:  %s\n", new_package->sender.name);
    printf("寄件人电话:  %s\n", new_package->sender.phone_number);
    printf("收件人姓名:  %s\n", new_package->recipient.name);
    printf("收件人电话:  %s\n", new_package->recipient.phone_number);
    switch (new_package->status) {
      case 1: printf("快递状态:  运输中\n"); break;
      case 2: printf("快递状态:  待入库\n"); break;
      case 3: printf("快递状态:  待取件\n"); break;
      case 4: printf("快递状态:  已取件\n"); break;
      case 5: printf("快递状态:  异常\n"); break;
        default: printf("快递状态:  不明\n"); break;
    };
    if (new_package->claim_code[0] != ' ' ) printf("取件码:  %s\n", new_package->claim_code);
    else printf("取件码:  暂无\n");
    printf("---------------------------------------------------------------");
}

void print_full_package_info(Package *new_package) {
    printf("---------------------------------------------------------------");
    printf("订单编号:   %ld\n", new_package->package_id);
    printf("寄件人姓名:  %s\n", new_package->sender.name);
    printf("寄件人电话:  %s\n", new_package->sender.phone_number);
    printf("收件人姓名:  %s\n", new_package->recipient.name);
    printf("收件人电话:  %s\n", new_package->recipient.phone_number);
    printf("物品类型:   %s\n", new_package->item.type);
    printf("物品名称:  %s\n", new_package->item.name);
    printf("物品重量:  %.2lf 千克\n", new_package->item.weight);
    printf("物品体积:  %.2lf 立方米\n", new_package->item.volume);
    switch (new_package->item.is_fragile) {
        case 1: printf("是否易碎:  是\n"); break;
        case 0: printf("是否易碎:  否\n"); break;
        default: printf("是否易碎:  未知\n"); break;
    }
    switch (new_package->status) {
        case 1: printf("快递状态:  运输中\n"); break;
        case 2: printf("快递状态:  待入库\n"); break;
        case 3: printf("快递状态:  待取件\n"); break;
        case 4: printf("快递状态:  已取件\n"); break;
        case 5: printf("快递状态:  异常\n"); break;
        default: printf("快递状态:  不明\n"); break;
    };
    if (strlen(new_package->claim_code) > 0)
        printf("取件码:  %s\n", new_package->claim_code);
    else
        printf("取件码:  暂无\n");
    printf("---------------------------------------------------------------");
}
