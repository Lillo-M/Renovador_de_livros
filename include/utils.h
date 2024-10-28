#ifndef _UTILS_
#define _UTILS_
#include <stdio.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
//PATH to data
#define PATH "/home/lillo/Documents/renovador/"
#define SUCESS_GET_USER_DATA 1
#define FAIL_GET_USER_DATA 0
#define ENDED_USERS -1
#define FAIL 0
#define SUCESS 1

struct MemoryStruct {
  char *memory;
  size_t size;
};

int get_user_data(char* passwd, char* rA, FILE* userdata);
int user_data_handle_ret(int ret);
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
                           void *userp);
#endif
