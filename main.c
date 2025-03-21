#include "shipping_order.h"
#include "sqlite3.h"
#include <stdio.h>
#include "packages.h"
#include "database.h"
#include <stdlib.h>
#include "users.h"

int main() {
    Users current_user;
    current_user.members.level = 0;
    if (!open_database()) {
        return 1;
    }
    initialize_all_tables();
    
    handle_shipping_order(&current_user);
    list_and_change_packages();
    search_and_modify_package();

    close_database();
    return 0;
}
