#include<stdio.h>
#include<stdlib.h>
#include<string.h> 
#include<users.h>
#include<goods.h>           //�Ŀ�ݽṹ���ļ� 
#include<Packages.h>           //ȡ��ݽṹ���ļ� 

// ���ļ���ȡ�û����ݲ���������
Users* loadUsersFromFile(const char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        perror("�޷����ļ�");
        return NULL;
    }

    // �ļ��е��û����ݲ���������ָ��
    typedef struct {
        char username[20];
        char password[20];
        int type;
        char phonenumber[20];
        Members members;
    } FileUser;

    Users* head = NULL;
    Users* tail = NULL;
    FileUser fuser;

    while (fread(&fuser, sizeof(FileUser), 1, fp) == 1) {
        Users* newUser = (Users*)malloc(sizeof(Users));
        if (!newUser) {
            perror("�ڴ����ʧ��");
            fclose(fp);
            return NULL;
        }

        // �������ݵ������ڵ�
        strcpy(newUser->username, fuser.username);
        strcpy(newUser->password, fuser.password);
        newUser->type = fuser.type;
        strcpy(newUser->phonenumber, fuser.phonenumber);
        newUser->members = fuser.members;
        newUser->next = NULL;

        // ��������
        if (!head) {
            head = tail = newUser;
        } else {
            tail->next = newUser;
            tail = newUser;
        }
    }

    fclose(fp);
    return head;
}

// ���»�Ա���ֺ͵ȼ�
void updateMemberLevel(Users* user, int pointsToAdd) {
    // ���ӻ��֣�������500��
    if (user->members.points + pointsToAdd > 500) {
        user->members.points = 500;
    } else {
        user->members.points += pointsToAdd;
    }

    // ����ȼ���0-4����
    user->members.level = user->members.points / 100;
    if (user->members.level > 4) {
        user->members.level = 4;
    }
}

// �����û����޸Ļ�Ա��Ϣ
void modifyUserMembers(Users* head, const char* username) {
    Users* current = head;
    while (current) {
        if (strcmp(current->username, username) == 0) {
            printf("�ҵ��û�: %s\n", username);
            printf("��ǰ�ȼ�: %d, ����: %d\n", current->members.level, current->members.points);

            int new_level, new_points;
            do {
                printf("�������µȼ� (0-4): ");
                scanf("%d", &new_level);
            } while (new_level < 0 || new_level > 4);

            printf("�������»���: ");
            scanf("%d", &new_points);

            current->members.level = new_level;
            current->members.points = new_points;
            printf("�޸ĳɹ���\n");
            return;
        }
        current = current->next;
    }
    printf("δ�ҵ��û�: %s\n", username);
}

// ������д���ļ�
void saveUsersToFile(const char* filename, Users* head) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror("�޷����ļ�");
        return;
    }

    // �ļ��е��û����ݲ���������ָ��
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
        // �������ݵ���ʱ�ṹ��
        strcpy(fuser.username, current->username);
        strcpy(fuser.password, current->password);
        fuser.type = current->type;
        strcpy(fuser.phonenumber, current->phonenumber);
        fuser.members = current->members;

        // д���ļ�
        if (fwrite(&fuser, sizeof(FileUser), 1, fp) != 1) {
            perror("д���ļ�ʧ��");
            fclose(fp);
            return;
        }

        current = current->next;
    }

    fclose(fp);
    printf("�û������ѱ��浽�ļ�: %s\n", filename);
}

// �ͷ������ڴ�
void freeUsers(Users* head) {
    Users* current = head;
    while (current) {
        Users* temp = current;
        current = current->next;
        free(temp);
    }
}


