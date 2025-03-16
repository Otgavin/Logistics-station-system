#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "users.h"

// 全局链表头指针
Users* userList = NULL;

// 将单个用户节点写入文件
void saveUser(FILE* fp, Users* user) {
    // 写入用户基础数据（不包含next指针）
    fwrite(user->username, sizeof(char), 20, fp);
    fwrite(user->password, sizeof(char), 20, fp);
    fwrite(&user->type, sizeof(int), 1, fp);
    fwrite(user->phonenumber, sizeof(char), 20, fp);
    fwrite(&user->members, sizeof(Members), 1, fp);
}

// 将整个链表保存到文件
void saveUsers(const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror("无法打开文件");
        return;
    }

    Users* current = userList;
    while (current != NULL) {
        saveUser(fp, current);
        current = current->next;
    }

    fclose(fp);
    printf("用户数据已保存至 %s\n", filename);
}

// 从文件读取用户数据并重建链表
void loadUsers(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("无法打开文件");
        return;
    }

    // 清空现有链表
    Users* current = userList;
    while (current != NULL) {
        Users* temp = current;
        current = current->next;
        free(temp);
    }
    userList = NULL;

    Users** nextPtr = &userList; // 用于链接新节点
    Users tempUser;

    // 循环读取直到文件结束
    while (1) {
        // 读取基础数据
        size_t readSize = fread(tempUser.username, sizeof(char), 20, fp);
        if (readSize == 0) break;

        fread(tempUser.password, sizeof(char), 20, fp);
        fread(&tempUser.type, sizeof(int), 1, fp);
        fread(tempUser.phonenumber, sizeof(char), 20, fp);
        fread(&tempUser.members, sizeof(Members), 1, fp);

        // 创建新节点
        Users* newUser = (Users*)malloc(sizeof(Users));
        memcpy(newUser, &tempUser, sizeof(Users));
        newUser->next = NULL;

        // 链接到链表
        *nextPtr = newUser;
        nextPtr = &(newUser->next);
    }

    fclose(fp);
    printf("用户数据已从 %s 加载\n", filename);
}

// 示例：添加用户到链表
void addUser(const char* name, const char* pwd, int type, const char* phone, Members mem) {
    Users* newUser = (Users*)malloc(sizeof(Users));
    strncpy(newUser->username, name, 20);
    strncpy(newUser->password, pwd, 20);
    newUser->type = type;
    strncpy(newUser->phonenumber, phone, 20);
    newUser->members = mem;
    newUser->next = userList;
    userList = newUser;
}

// 示例：打印链表
void printUsers() {
    Users* current = userList;
    while (current != NULL) {
        printf("用户名: %s\n电话: %s\n类型: %s\n积分: %d\n---\n",
               current->username,
               current->phonenumber,
               current->type == 0 ? "管理员" : "用户",
               current->members.points);
        current = current->next;
    }
}

int main() {
    // 添加示例用户
    Members mem1 = {1, 100};
    addUser("Admin", "admin123", 0, "13800138000", mem1);

    Members mem2 = {2, 200};
    addUser("User1", "user123", 1, "13900139000", mem2);

    // 保存到文件
    saveUsers("users.dat");

    // 从文件加载并打印
    loadUsers("users.dat");
    printUsers();

    return 0;
}

