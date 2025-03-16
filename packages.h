#ifndef PACKAGES_H
#define PACKAGES_H


// 寄件人信息结构体
typedef struct sender_t {
    char name[100];
    char address[200];
    char phone_number[20];
} Sender;

// 收件人信息结构体
typedef struct recipient_t {
    char name[100];
    char address[200];
    char phone_number[20];
} Recipient;

//物品信息结构体
typedef struct item_t {
    char type[20];
    char name[50];
    double weight;
    double volume;
    int is_fragile;
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
void save_package_to_file(Package *new_package);
void print_short_package_info(Package *new_package);
void print_full_package_info(Package *new_package);
Package find_package_by_id(const long id);
Package find_package_by_recipient_phone_number(const char *phone_number);
Package find_package_by_recipient_name(const char *name);
Package find_package_by_sender_name(const char *address);
Package find_package_by_claim_code(const char *code);


#endif //PACKAGES_H
