#ifndef USERS_H
#define USERS_H

//会员信息结构体 
typedef struct members {
    int level;   //等级 5个等级  0-4
    int points;  // 会员积分 
} Members;
 
// 用户结构体 
typedef struct users {
	char username[20];        // 用户名
	char password[20];        // 密码 
	int type;                 //管理员0 用户1
    char phonenumber[20];     // 电话号码  
    Members members;   
    struct user* next;        // 链表指针
} Users;

Users* search_user_by_name(const char* target_name);
Users* search_user_by_phonenumber(const char* target_phonenumber);
Users* add(check x,check y);
void  delete_username(check x);
void  delete_phonenumber(check x);

#endif

