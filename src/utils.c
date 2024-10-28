#include "../include/utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

int get_user_data(char *passwd, char *rA, FILE *userdata) {
  if (userdata == NULL) {
    printf("Failed to open userdata.txt\n");
    return FAIL_GET_USER_DATA;
  }
  (void)fgets(rA, 9, userdata);
  rA[strlen(rA) - 1] = '\0';
  if (strcasecmp(rA, "end") == 0)
    return ENDED_USERS;
  (void)fgets(passwd, 1, userdata); // joga fora o separador de RA:SENHA
  int lenRA = strlen(rA);
  (void)fgets(passwd, 8, userdata);
  passwd[strlen(passwd) - 1] = '\0';
  int lenPSWD = strlen(passwd);
  if (lenRA != 7 || (lenPSWD != 4 && lenPSWD != 6))
    return FAIL_GET_USER_DATA;

  int i = 0;
  while (rA[i] != '\0') {
    if (!isdigit(rA[i]))
      return FAIL_GET_USER_DATA;
    i++;
  }
  i = 0;
  while (passwd[i] != '\0') {
    if (!isdigit(passwd[i]))
      return FAIL_GET_USER_DATA;
    i++;
  }
  return SUCESS_GET_USER_DATA;
}

int user_data_handle_ret(int ret) {
  switch (ret) {
  case SUCESS_GET_USER_DATA:
    printf("User data gathered sucessfully.\n");
    break;
  case FAIL_GET_USER_DATA:
    (void)printf("Falha em obter usuario e senha\n"
                 "cheque userdata.txt, o formato deve ser\n"
                 "RA:SENHA\n");
      return FAIL;
    break;
  case ENDED_USERS:
    (void)printf("Foram testados todos os alunos.\n");
    break;
  default:
    (void)printf("FATAL ERROR: Codigo de retorno Invalido (%d)\n", ret);
    return FAIL;
    break;
  }
  return SUCESS;
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
