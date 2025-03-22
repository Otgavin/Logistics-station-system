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

// 主函数
int main() {
    Users* current_user = NULL;
    int choice;
    
    // 初始化数据库
    if (!open_database()) {
        fprintf(stderr, "数据库初始化失败，程序退出\n");
        return 1;
    }
    
    // 初始化所有表
    initialize_all_tables();
    
    while (1) {
        clear_screen();
        printf("===== 快递管理系统 =====\n");
        printf("1. 登录\n");
        printf("2. 注册\n");
        printf("0. 退出系统\n");
        printf("请选择: ");
        
        scanf("%d", &choice);
        getchar(); // 清除输入缓冲区中的换行符
        
        switch (choice) {
            case 1:
                current_user = login_system();
                if (current_user != NULL) {
                    int menu_result = 0;
                    
                    if (current_user->type == 1) {
                        menu_result = admin_menu(current_user);
                    } else if (current_user->type == 2) {
                        menu_result = employee_menu(current_user);
                    } else {
                        menu_result = user_menu(current_user);
                    }
                    
                    // Don't free current_user as it causes crashes
                    current_user = NULL;
                    
                    if (menu_result == 1) {
                        printf("返回登录界面...\n");
                        pause_screen();
                    }
                } else {
                    printf("登录失败，请检查用户名和密码。\n");
                    pause_screen();
                }
                break;
                
            case 2:
                register_system();
                break;
                
            case 0:
                // 关闭数据库连接
                close_database();
                printf("感谢使用，再见！\n");
                printf("\n====================================\n");
                printf("         系统已安全退出            \n");
                printf("====================================\n\n");
                return 0;
                
            default:
                printf("无效选择，请重试。\n");
                pause_screen();
                break;
        }
    }
    
    // 这里不会被执行到，但保留为安全措施
    close_database();
    return 0;
}
