#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "database.h"
#include "packages.h"
#include "shipping_order.h"
#include "price.h"
#include "users.h"
#include "order.h"
#include "coupon.h"
#include "part1.h"
#include "utils.h"
#include "menu_functions.h"

// Login function
Users* login_system() {
    char username[20];
    char password[20];
    Users* current_user = NULL;
    
    clear_screen();
    printf("===== 快递管理系统登录 =====\n");
    printf("用户名: ");
    fgets(username, 20, stdin);
    username[strcspn(username, "\n")] = 0;
    
    printf("密码: ");
    fgets(password, 20, stdin);
    password[strcspn(password, "\n")] = 0;
    
    current_user = userLogin(username, password, "users.txt");
    
    return current_user;
}

void register_system() {
    char username[20];
    char password[20];
    char phonenumber[20];
    
    clear_screen();
    printf("===== 快递管理系统注册 =====\n");
    printf("用户名: ");
    fgets(username, 20, stdin);
    username[strcspn(username, "\n")] = 0;
    
    printf("密码: ");
    fgets(password, 20, stdin);
    password[strcspn(password, "\n")] = 0;

    printf("手机号: ");
    fgets(phonenumber, 20, stdin);
    phonenumber[strcspn(phonenumber, "\n")] = 0;
    
    usersRegis(username, password, phonenumber, "users.txt");
    pause_screen();
}

// Main menus - changed return type from void to int to indicate logout status
int admin_menu(Users* user) {
    int choice;
    
    while (1) {
        clear_screen();
        printf("===== 管理员系统 =====\n");
        printf("欢迎, %s (管理员)\n\n", user->username);
        printf("1. 用户管理\n");
        printf("2. 货架管理\n");
        printf("3. 寄送管理\n");
        printf("4. 查询操作历史\n");
        printf("0. 退出登录\n");
        printf("请选择功能: ");
        
        scanf("%d", &choice);
        getchar(); // 清除输入缓冲区中的换行符
        
        switch (choice) {
            case 1:
                admin_user_management();
                break;
            case 2:
                admin_shelf_management();
                break;
            case 3:
                admin_shipping_management();
                break;
            case 4:
                admin_query_history();
                break;
            case 0:
                printf("已退出登录，返回登录界面。\n");
                pause_screen(); // Add a pause here to see the message
                return 1; // Return 1 to indicate successful logout and return to login screen
            default:
                printf("无效选择，请重试。\n");
                pause_screen();
                break;
        }
    }
    
    // Add this to ensure a return value in all cases
    return 0;
}

int employee_menu(Users* user) {
    int choice;
    
    while (1) {
        clear_screen();
        printf("===== 员工系统 =====\n");
        printf("欢迎, %s (员工)\n\n", user->username);
        printf("1. 用户管理\n");
        printf("2. 货架管理\n");
        printf("3. 寄送管理\n");
        printf("4. 投诉管理\n");
        printf("0. 退出登录\n");
        printf("请选择功能: ");
        
        scanf("%d", &choice);
        getchar(); // 清除输入缓冲区中的换行符
        
        switch (choice) {
            case 1:
                employee_user_management();
                break;
            case 2:
                employee_shelf_management();
                break;
            case 3:
                employee_shipping_management();
                break;
            case 4:
                employee_complaint_management();
                break;
            case 0:
                printf("已退出登录，返回登录界面。\n");
                pause_screen(); // Add a pause here to see the message
                return 1; // Return 1 to indicate successful logout and return to login screen
            default:
                printf("无效选择，请重试。\n");
                pause_screen();
                break;
        }
    }
    
    // Add this to ensure a return value in all cases
    return 0;
}

int user_menu(Users* user) {
    int choice;
    
    while (1) {
        clear_screen();
        printf("===== 用户系统 =====\n");
        printf("欢迎, %s\n", user->username);
        printf("会员等级: %d   积分: %d\n\n", user->members.level, user->members.points);
        
        printf("1. 取快递\n");
        printf("2. 寄快递\n");
        printf("3. 会员中心\n");
        printf("4. 通知中心\n");
        printf("5. 申诉中心\n");
        printf("0. 退出登录\n");
        printf("请选择功能: ");
        
        scanf("%d", &choice);
        getchar(); // 清除输入缓冲区中的换行符
        
        switch (choice) {
            case 1:
                user_receive_package();
                break;
            case 2:
                handle_shipping_order(user);
                break;
            case 3:
                user_member_center();
                break;
            case 4:
                user_notification_center();
                break;
            case 5:
                user_appeal_center();
                break;
            case 0:
                printf("已退出登录，返回登录界面。\n");
                pause_screen(); // Add a pause here to see the message
                return 1; // Return 1 to indicate successful logout and return to login screen
            default:
                printf("无效选择，请重试。\n");
                pause_screen();
                break;
        }
    }
    
    // Add this to ensure a return value in all cases
    return 0;
}

// 管理员模块实现
void admin_user_management() {
    int choice;
    char search_input[50];
    
    clear_screen();
    printf("===== 用户管理 =====\n");
    printf("1. 查找用户\n");
    printf("2. 添加员工/管理员\n");
    printf("3. 会员管理\n");
    printf("0. 返回\n");
    printf("请选择功能: ");
    
    scanf("%d", &choice);
    getchar();
    
    switch (choice) {
        case 1:
            printf("请输入用户名或手机号: ");
            fgets(search_input, 50, stdin);
            search_input[strcspn(search_input, "\n")] = 0;
            
            // 根据输入的内容判断是用户名还是手机号
            if (strspn(search_input, "0123456789") == strlen(search_input)) {
                searchphonenumber(search_input);
            } else {
                searchusername(search_input);
            }
            break;
        case 2:
            printf("添加员工/管理员功能尚未实现\n");
            break;
        case 3:
            printf("会员管理功能尚未实现\n");
            break;
        case 0:
            return;
        default:
            printf("无效选择，请重试。\n");
            break;
    }
    pause_screen();
}

void admin_shelf_management() {
    printf("===== 货架管理 =====\n");
    list_and_change_packages();
    pause_screen();
}

void admin_shipping_management() {
    int choice;
    
    clear_screen();
    printf("===== 寄送管理 =====\n");
    printf("1. 查询包裹信息\n");
    printf("2. 计费标准变更\n");
    printf("0. 返回\n");
    printf("请选择功能: ");
    
    scanf("%d", &choice);
    getchar();
    
    switch (choice) {
        case 1:
            search_and_modify_package();
            break;
        case 2:
            modify_pricing_rules();
            break;
        case 0:
            return;
        default:
            printf("无效选择，请重试。\n");
            break;
    }
    pause_screen();
}

void admin_query_history() {
    int choice;
    
    clear_screen();
    printf("===== 操作历史查询 =====\n");
    printf("1. 用户操作记录\n");
    printf("2. 货架操作记录\n");
    printf("3. 寄送快递操作记录\n");
    printf("4. 投诉处理记录\n");
    printf("0. 返回\n");
    printf("请选择功能: ");
    
    scanf("%d", &choice);
    getchar();
    
    printf("该功能尚未实现\n");
    pause_screen();
}

// 员工模块实现
void employee_user_management() {
    int choice;
    char search_input[50];
    
    clear_screen();
    printf("===== 用户管理 =====\n");
    printf("1. 查找用户\n");
    printf("2. 发送通知\n");
    printf("3. 查看会员信息\n");
    printf("0. 返回\n");
    printf("请选择功能: ");
    
    scanf("%d", &choice);
    getchar();
    
    switch (choice) {
        case 1:
            printf("请输入用户名或手机号: ");
            fgets(search_input, 50, stdin);
            search_input[strcspn(search_input, "\n")] = 0;
            
            // 根据输入的内容判断是用户名还是手机号
            if (strspn(search_input, "0123456789") == strlen(search_input)) {
                searchphonenumber(search_input);
            } else {
                searchusername(search_input);
            }
            break;
        case 2:
            printf("发送通知功能尚未实现\n");
            break;
        case 3:
            printf("请输入用户名: ");
            fgets(search_input, 50, stdin);
            search_input[strcspn(search_input, "\n")] = 0;
            searchusername(search_input);
            break;
        case 0:
            return;
        default:
            printf("无效选择，请重试。\n");
            break;
    }
    pause_screen();
}

void employee_shelf_management() {
    int choice;
    
    clear_screen();
    printf("===== 货架管理 =====\n");
    printf("1. 查看库存\n");
    printf("2. 快递入库\n");
    printf("3. 快递出库\n");
    printf("0. 返回\n");
    printf("请选择功能: ");
    
    scanf("%d", &choice);
    getchar();
    
    switch (choice) {
        case 1:
            list_and_change_packages();
            break;
        case 2:
            printf("快递入库功能尚未实现\n");
            break;
        case 3:
            printf("快递出库功能尚未实现\n");
            break;
        case 0:
            return;
        default:
            printf("无效选择，请重试。\n");
            break;
    }
    pause_screen();
}

void employee_shipping_management() {
    int choice;
    
    clear_screen();
    printf("===== 寄送管理 =====\n");
    printf("1. 登记寄送信息\n");
    printf("2. 取件到楼服务管理\n");
    printf("3. 发送通知\n");
    printf("0. 返回\n");
    printf("请选择功能: ");
    
    scanf("%d", &choice);
    getchar();
    
    switch (choice) {
        case 1:
            printf("请输入会员用户名: ");
            char username[50];
            fgets(username, 50, stdin);
            username[strcspn(username, "\n")] = 0;
            
            // 临时创建一个用户对象用于测试
            Users tempUser;
            strncpy(tempUser.username, username, 20);
            tempUser.members.level = 1;
            tempUser.members.points = 100;
            
            handle_shipping_order(&tempUser);
            break;
        case 2:
            printf("取件到楼服务管理功能尚未实现\n");
            break;
        case 3:
            printf("发送通知功能尚未实现\n");
            break;
        case 0:
            return;
        default:
            printf("无效选择，请重试。\n");
            break;
    }
    pause_screen();
}

void employee_complaint_management() {
    printf("===== 投诉管理 =====\n");
    printf("投诉管理功能尚未实现\n");
    pause_screen();
}

// 用户模块实现
void user_receive_package() {
    int choice;
    char search_input[50];
    
    clear_screen();
    printf("===== 取快递 =====\n");
    printf("1. 包裹状态查询\n");
    printf("2. 取件\n");
    printf("3. 取件记录\n");
    printf("4. 送件到楼申请\n");
    printf("0. 返回\n");
    printf("请选择功能: ");
    
    scanf("%d", &choice);
    getchar();
    
    switch (choice) {
        case 1:
            printf("请输入手机号: ");
            fgets(search_input, 50, stdin);
            search_input[strcspn(search_input, "\n")] = 0;
            
            // 查询该手机号下的所有包裹
            int count;
            Package* packages = find_packages_by_recipient_phone_number(search_input, &count);
            
            if (count > 0) {
                printf("找到 %d 个包裹\n", count);
                for (int i = 0; i < count; i++) {
                    printf("包裹 %d: ID = %ld, 状态 = %s\n", 
                           i+1, packages[i].package_id, get_status_text(packages[i].status));
                }
                free(packages);
            } else {
                printf("未找到包裹\n");
            }
            break;
        case 2:
            printf("请输入取件码: ");
            fgets(search_input, 50, stdin);
            search_input[strcspn(search_input, "\n")] = 0;
            
            Package package = find_package_by_claim_code(search_input);
            if (package.package_id > 0) {
                print_full_package_info(&package);
                
                if (package.status == 3) {  // 待取件状态
                    printf("是否确认取件? (Y/N): ");
                    char confirm;
                    scanf(" %c", &confirm);
                    getchar();
                    
                    if (confirm == 'Y' || confirm == 'y') {
                        // 更新包裹状态为已取件（4）
                        if (update_package_status(package.package_id, 4)) {
                            printf("✅ 取件成功，状态已更新为已取件。\n");
                        } else {
                            printf("❌ 更新状态失败，请稍后重试。\n");
                        }
                    }
                } else {
                    printf("该包裹当前状态不是'待取件'，无法取件\n");
                }
            } else {
                printf("未找到匹配的包裹\n");
            }
            break;
        case 3:
            printf("请输入手机号: ");
            fgets(search_input, 50, stdin);
            search_input[strcspn(search_input, "\n")] = 0;
            
            printf("取件记录查询功能尚未实现\n");
            break;
        case 4:
            printf("送件到楼申请功能尚未实现\n");
            break;
        case 0:
            return;
        default:
            printf("无效选择，请重试。\n");
            break;
    }
    pause_screen();
}

void user_member_center() {
    int choice;
    
    clear_screen();
    printf("===== 会员中心 =====\n");
    printf("1. 查看会员等级\n");
    printf("2. 查看会员积分\n");
    printf("3. 查看代金券\n");
    printf("0. 返回\n");
    printf("请选择功能: ");
    
    scanf("%d", &choice);
    getchar();
    
    printf("会员中心功能尚未实现\n");
    pause_screen();
}

void user_notification_center() {
    printf("===== 通知中心 =====\n");
    printf("通知中心功能尚未实现\n");
    pause_screen();
}

void user_appeal_center() {
    int choice;
    
    clear_screen();
    printf("===== 申诉中心 =====\n");
    printf("1. 包裹损坏申诉\n");
    printf("2. 包裹丢失申诉\n");
    printf("0. 返回\n");
    printf("请选择功能: ");
    
    scanf("%d", &choice);
    getchar();
    
    printf("申诉中心功能尚未实现\n");
    pause_screen();
}
