#ifndef USERS_H
#define USERS_H 

#include <stdio.h>
// ��Ա��Ϣ�ṹ��
typedef struct members {
    int level;  // �ȼ� 0-4����Ӧ5���ȼ���
    int points; // ��Ա����
} Members;

// �û��ṹ�壨��������ָ�룩
typedef struct users {
    char username[20];
    char password[20];
    int type;           // 1-����Ա, 2-Ա��, 3-�û�
    char phonenumber[20];
    Members members;
    struct users* next; // ����ָ��
} Users;

void saveUser(FILE* fp, Users* user);/* �������û��ڵ�д���ļ� */
void saveUsers(const char* filename);/* �������������浽�ļ� */
void loadUsers(const char* filename);/* ���ļ���ȡ�û����ݲ��ؽ����� */
void addUser(const char* name, const char* pwd, int type, const char* phone, Members mem);/* �����û���������ͷ�壩 */
void searchusername(const char* username);/*���ݸ����û�������ָ���û�*/ 
void searchphonenumber(const char* phonenumber);/*���ݸ����绰�������ָ���û�*/ 
Users* loadUsersFromFile(const char* filename);
void modifyUserMembers(Users* head, const char* username);
void saveUsersToFile(const char* filename, Users* head);
void freeUsers(Users* head);
void updateMemberLevel(Users* user, int pointsToAdd);
#endif

