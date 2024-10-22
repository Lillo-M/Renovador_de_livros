#ifndef _UTILS_
#define _UTILS_
#include <unistd.h>

#define TRUE 1
#define FALSE 0

struct MemoryStruct {
  char *memory;
  size_t size;
};

int get_user_data(char* passwd, char* rA);
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
                           void *userp);
#endif
