#ifndef USERS_H
#define USERS_H
//��Ա��Ϣ�ṹ�� 

#include <stdio.h>

typedef struct members {
    int level;  //�ȼ� 5���ȼ�  0-4
    int points;  // ��Ա���� 
}Members; 
 
// �û��ṹ�� 
typedef struct users {
	char username[20];        // �û���
	char password[20];        // ���� 
	int type;  //����Ա0 �û�1 
    char phonenumber[20];     // �绰����  
    Members members;          //��Ա�ṹ�� 
    struct user* next;        // ����ָ��
} Users;

void saveUser(FILE* fp, Users* user);/* �������û��ڵ�д���ļ� */
void saveUsers(const char* filename);/* �������������浽�ļ� */
void loadUsers(const char* filename);/* ���ļ���ȡ�û����ݲ��ؽ����� */
void addUser(const char* name, const char* pwd, int type, const char* phone, Members mem);/* �����û���������ͷ�壩 */
void searchusername(const char* username);/*���ݸ����û�������ָ���û�*/ 
void searchphonenumber(const char* phonenumber);/*���ݸ����绰�������ָ���û�*/ 
#endif

