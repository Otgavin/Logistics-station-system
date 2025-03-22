#include<stdio.h>
#include<stdlib.h>
#include<string.h> 
#include"users.h"
#include"Packages.h"        //取快递结构体文件
//增加功能：根据用户名直接关联到其名下的快递（取、寄） 
 
/* 全局链表头指针 */
Users* userList = NULL;

/* 将单个用户节点写入文件 */
void saveUser(FILE* fp, Users* user) {
    /* 写入用户基础数据（定长写入，不包含next指针） */
    fwrite(user->username, sizeof(char), 20, fp);
    fwrite(user->password, sizeof(char), 20, fp);
    fwrite(&user->type, sizeof(int), 1, fp);
    fwrite(user->phonenumber, sizeof(char), 20, fp);
    fwrite(&user->members, sizeof(Members), 1, fp);
}

// 将链表写回文件
void saveUsersToFile(const char* filename, Users* head) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror("无法打开文件");
        return;
    }

    // 文件中的用户数据不包含链表指针
    typedef struct {
        char username[20];
        char password[20];
        int type;
        char phonenumber[20];
        Members members;
    } FileUser;

    Users* current = head;
    FileUser fuser;

    while (current) {
        // 复制数据到临时结构体
        strcpy(fuser.username, current->username);
        strcpy(fuser.password, current->password);
        fuser.type = current->type;
        strcpy(fuser.phonenumber, current->phonenumber);
        fuser.members = current->members;

        // 写入文件
        if (fwrite(&fuser, sizeof(FileUser), 1, fp) != 1) {
            perror("写入文件失败");
            fclose(fp);
            return;
        }

        current = current->next;
    }

    fclose(fp);
    printf("用户数据已保存到文件: %s\n", filename);
}

/* 从文件读取用户数据并重建链表 */
void loadUsers(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("无法打开文件");
        return;
    }

    /* 清空现有链表 */
    Users* current = userList;
    while (current != NULL) {
        Users* temp = current;
        current = current->next;
        free(temp);
    }
    userList = NULL;

    Users** nextPtr = &userList;  // 用于链接新节点
    Users tempUser;

    /* 循环读取直到文件结束 */
    while (1) {
        /* 读取基础数据 */
        size_t readSize = fread(tempUser.username, sizeof(char), 20, fp);
        if (readSize != 20) break;  // 数据不完整时退出

        fread(tempUser.password, sizeof(char), 20, fp);
        fread(&tempUser.type, sizeof(int), 1, fp);
        fread(tempUser.phonenumber, sizeof(char), 20, fp);
        fread(&tempUser.members, sizeof(Members), 1, fp);

        /* 创建新节点并拷贝数据 */
        Users* newUser = (Users*)malloc(sizeof(Users));
        memcpy(newUser, &tempUser, sizeof(Users));
        newUser->next = NULL;

        /* 链接到链表 */
        *nextPtr = newUser;
        nextPtr = &(newUser->next);
    }

    fclose(fp);
    printf("用户数据已从 %s 加载\n", filename);
}

/* 添加用户到链表（头插法） */
void addUser(const char* name, const char* pwd, int type, const char* phone, Members mem) {
    Users* newUser = (Users*)malloc(sizeof(Users));
    
    // 安全拷贝字符串并保证终止符
    strncpy(newUser->username, name, 19);
    newUser->username[19] = '\0';
    
    strncpy(newUser->password, pwd, 19);
    newUser->password[19] = '\0';
    
    newUser->type = type;
    
    strncpy(newUser->phonenumber, phone, 19);
    newUser->phonenumber[19] = '\0';
    
    newUser->members = mem;
    newUser->next = userList;
    userList = newUser;
}

/*根据给出用户名查找指定用户*/ 
void searchusername(const char* username) {
    if (username == NULL) {
        printf("错误：用户名为空\n");
        return;
    } 
    Users* current = userList;
    int found = 0;

    while (current != NULL) { 
        if (strcmp(current->username, username) == 0) {
            printf("\n========= 用户信息 =========\n");
            printf("用户名:    %s\n", current->username);
            printf("密码:      %s\n", current->password);
            printf("电话:      %s\n", current->phonenumber); 
            printf("会员等级:  %d\n", current->members.level);
            printf("积分:      %d\n", current->members.points);
            printf("============================\n");
            found = 1;
            break;
        }
        current = current->next;
    }

    if (!found) {
        printf("未找到用户名为 [%s] 的用户\n", username);
    }
}

/*根据给出电话号码查找指定用户*/ 
void searchphonenumber(const char* phonenumber) {
    if (phonenumber == NULL) {
        printf("错误：电话号码为空\n");
        return;
    } 
    Users* current = userList;
    int found = 0;

    while (current != NULL) { 
        if (strcmp(current->phonenumber, phonenumber) == 0) {
            printf("\n========= 用户信息 =========\n");
            printf("用户名:    %s\n", current->username);
            printf("密码:      %s\n", current->password);
            printf("电话:      %s\n", current->phonenumber); 
            printf("会员等级:  %d\n", current->members.level);
            printf("积分:      %d\n", current->members.points);
            printf("============================\n");
            found = 1;
            break;
        }
        current = current->next;
    }

    if (!found) {
        printf("未找到电话号码为 [%s] 的用户\n", phonenumber);
    }
}

/* 
    loadUsers("users.dat"); // 初始化加载
    // ... 其他操作（添加/删除/搜索用户）
    saveUsers("users.dat"); // 退出前保存
*/
 
