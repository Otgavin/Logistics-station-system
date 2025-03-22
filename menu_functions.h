#ifndef MENU_FUNCTIONS_H
#define MENU_FUNCTIONS_H

#include "users.h"

// Login and register
Users* login_system();
void register_system();

// Main menus - changed return type from void to int
int admin_menu(Users* user);
int employee_menu(Users* user);
int user_menu(Users* user);

// Admin functions
void admin_user_management();
void admin_shelf_management();
void admin_shipping_management();
void admin_query_history();

// Employee functions
void employee_user_management();
void employee_shelf_management();
void employee_shipping_management();
void employee_complaint_management();

// User functions
void user_receive_package();
void user_member_center();
void user_notification_center();
void user_appeal_center();

#endif // MENU_FUNCTIONS_H
