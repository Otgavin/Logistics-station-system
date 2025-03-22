#include "part1.h"
#include <stdio.h>
#include<string.h>
#include <stdlib.h>

struct users* loadUsersFromFile(const char* filename) {
    FILE*file=fopen(filename,"r");
    if (file==NULL) {
        return NULL;
    }
    struct users*head=NULL;
    struct users*tail=NULL;
    char name[20];
    char pass[20];
    int ty;//类型
    char phone[20];
    int le;//等级
    int pos;//积分
    while (fscanf(file,"%19s  %19s  %d  %s  %d  %d",name,pass,&ty,phone,&le,&pos)==6) {
        struct users*olduser=(struct users*)malloc(sizeof(struct users));
        if (olduser==NULL) {
            printf("内存分配失败\n");
            fclose(file);
            free(olduser);
            return NULL;
        }
        strcpy(olduser->username,name);
        strcpy(olduser->password,pass);
        olduser->type=ty;
        strcpy(olduser->phonenumber,phone);
        olduser->members.level=le;
        olduser->members.points=pos;
        olduser->next=NULL;
        if (head==NULL) {
            head=olduser;
        } else {
            tail->next=olduser;
        }
        tail=olduser;
    }
    fclose(file);
    return head;
}
void freeUsers(struct users* users) {
    if (users==NULL) {
        return;
    }
    struct users*temp=users;
    while (temp!=NULL) {
        struct users*temp2=temp->next;
        free(temp);
        temp=temp2;
    }
}
void usersRegis(char usersname[10],char password[20],char phoneNumber[20],const char*filename) {
   Users*head=loadUsersFromFile(filename);
    Users*temp=head;
    while (temp!=NULL) {
        if (strcmp(temp->username,usersname)==0) {
            printf("该用户名已存在\n");
            freeUsers(temp);
            return;
        }else {
            if (strcmp(phoneNumber,temp->phonenumber)==0) {
                printf("该电话号码已经注册过，请勿重复使用\n");
                freeUsers(temp);
                return;
            }
        }
        temp=temp->next;
    }
    freeUsers(head);
    FILE *file=fopen(filename,"a");
    if (file==NULL) {
        printf("文件打开失败\n");
        return;
    }
    fprintf(file,"%s  %s  3  %s  0  0\n",usersname,password,phoneNumber);
     fclose(file);
    printf("用户注册成功\n");
}
Users* userLogin(char username[20],char password[20],const char*filename) {
    Users*head=loadUsersFromFile(filename);
    Users*temp=head;
    while (temp!=NULL) {
        if (strcmp(temp->username,username)==0) {
            if (strcmp(password,temp->password)==0) {
                freeUsers(temp);
                return temp;
            }
        }
        temp=temp->next;
    }
    freeUsers(head);
    printf("登录失败\n");
    return 0;
}

