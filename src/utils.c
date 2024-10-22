#include "../include/utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>



int get_user_data(char* passwd, char* rA) {
  FILE *userdata = fopen("userdata.txt", "r");
  (void)fgets(rA, 9, userdata);
  (void)fgets(passwd, 1, userdata);
  rA[strlen(rA) - 1] = '\0';
  int lenRA = strlen(rA);
  (void)fgets(passwd, 8, userdata);
  passwd[strlen(passwd) - 1] = '\0';
  int lenPSWD = strlen(passwd);
  fclose(userdata);
  if (lenRA != 7 || (lenPSWD != 4 && lenPSWD != 6))
    return 0;

  int i = 0;
  while(rA[i] != '\0') {
    if (!isdigit(rA[i]))
      return 0;
    i++;
  }
  i = 0;
  while(passwd[i] != '\0') {
    if (!isdigit(passwd[i]))
      return 0;
    i++;
  }
  return 1;
}

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb,
                           void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if (ptr == NULL) {
    printf("error: not enough memory\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}
