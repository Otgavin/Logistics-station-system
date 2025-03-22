#pragma once

#include "users.h"

struct users*loadUsersFormFile(const char* filename);
void freeUsers(struct users* users);
void usersRegis(char usersname[10],char password[20],char phoneNumber[20],const char*filename);
Users* userLogin(char username[20],char password[20],const char*filename);
