#ifndef SHIPPING_ORDER_H
#define SHIPPING_ORDER_H

#include "packages.h"
// 函数声明
void input_sender_info(Sender *sender);
void input_recipient_info(Recipient *recipient);
void input_item_info(Item *item);
int select_pickup_method();
int display_order_summary(Package *new_package);
void handle_shipping_order();
long read_max_order_id();  // 读取当前最大订单编号
void update_max_order_id(long new_max_id);  // 更新最大订单编号

#endif // SHIPPING_ORDER_H
