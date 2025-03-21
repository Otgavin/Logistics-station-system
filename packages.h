#ifndef PACKAGES_H
#define PACKAGES_H

#include "sqlite3.h"

#define PAGE_SIZE 10 //列举时每页展示数量

//寄件人和收件人信息结构体
typedef struct {
    char name[20];
    char province[20];   // ✅ 新增：选择的省份
    char address[50];    // 用户自填详细地址
    char phone_number[20];
} Sender, Recipient;

//物品信息结构体
typedef struct item_t {
    char type[20];
    char name[50];
    double weight;
    double volume;
    int is_fragile;
    char special_property[30]; // 特殊属性（如 液体、生鲜、易燃、自定义）
} Item;


// 包裹信息结构体
typedef struct package_t {
    long package_id;
    Sender sender;     // 寄件人信息
    Recipient recipient; // 收件人信息
    Item item;
    int status;               // 订单状态: 1 = 运输中, 2 = 待入库, 3 = 待取件, 4 = 已取件， 5 = 异常
    char claim_code[50];      // 取件码
} Package;

//函数声明
void save_package_to_db(Package *new_package);
void print_short_package_info(Package *new_package);
void print_full_package_info(Package *new_package);


//包裹查找相关
Package find_package_by_id(const long id);
Package* find_package_by_recipient_phone_number(const char *phone_number);
Package* find_package_by_recipient_name(const char *name);
Package* find_package_by_sender_name(const char *address);
Package find_package_by_claim_code(const char *code);
Package execute_single_result_query_int(sqlite3 *db, const char *query, long param);
Package execute_single_result_query_text(sqlite3 *db, const char *query, const char *param);
Package* execute_multiple_result_query(sqlite3 *db, const char *query, const char *param, int *count);

//包裹列举相关
const char* get_status_text(int status);
void list_packages(int page, const char *order_by);
void update_package_info(long package_id);

//主函数
void list_and_change_packages();
void search_and_modify_package();
#endif //PACKAGES_H
