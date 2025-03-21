#ifndef DATABASE_H
#define DATABASE_H

#include "sqlite3.h"

extern sqlite3 *db;


int open_database();
void close_database();
void initialize_all_tables();  // 初始化所有表

#endif
