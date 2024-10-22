#include "../include/cJSON.h"
#include "../include/gets-sets.h"
#include "../include/utils.h"
#include <curl/curl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  cJSON *cjson_buf = NULL;
  cJSON *data_dev = NULL;
  cJSON *titulos = NULL;
  cJSON *titulo = NULL;
  char *auth_token = NULL;
  char *temp = NULL;
  char data_atual[16] = "";
  char request_buffer[2048] = "";
  char rAcademico[9] = "";
  char senha[8] = "";
  CURL *curl_handle = NULL;
  CURLcode res;
  struct MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;
  (void)setlocale(LC_ALL, "");

  if (get_user_data(senha, rAcademico) == 0) {
    (void)printf("Falha em obter usuario e senha\n"
                 "cheque userdata.txt, o formato deve ser\n"
                 "RA:SENHA\n");
    return 1;
  }

  (void)sprintf(
      request_buffer,
      "{\"grant_type\":\"password\",\"client_secret\":"
      "\"H2oPoUivqUvbHXaYg4TBURKfh1UaFWx2Vz8F8Mpl\",\"client_id\":\"4\","
      "\"scope\":\"\",\"username\":\"%s\",\"password\":\"%s\"}",
      rAcademico, senha);

  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  if (!curl_handle) {
    (void)printf("Falha ao alocar memória\n");
    return 1;
  }

  struct curl_slist *list = NULL;

  res = get_auth_token(&list, curl_handle, request_buffer, &chunk);
  while (res == CURLE_COULDNT_RESOLVE_HOST) {
    sleep(10);
    res = curl_easy_perform(curl_handle);
  }
  if (res != CURLE_OK) {
    (void)fprintf(stderr, "error: %s\n", curl_easy_strerror(res));
    curl_easy_cleanup(curl_handle);
    curl_slist_free_all(list);
    free(chunk.memory);
    return 1;
  } else {
    //(void)printf("Size: %lu\n", (unsigned long)chunk.size);
    //(void)printf("Data: %s\n", chunk.memory);
  }

  cjson_buf = cJSON_Parse(chunk.memory);
  if (!cjson_buf) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr) {
      (void)fprintf(stderr, "Error before: %s\n", error_ptr);
    }
  }

  if (cJSON_GetObjectItem(cjson_buf, "message")) {
    (void)printf("ERROR: %s\n",
                 cJSON_GetObjectItem(cjson_buf, "message")->valuestring);
    curl_easy_cleanup(curl_handle);
    cJSON_Delete(cjson_buf);
    curl_slist_free_all(list);
    free(chunk.memory);
    return 1;
  }

  cJSON *authJson = cJSON_GetObjectItem(cjson_buf, "access_token");
  auth_token =
      (char *)malloc(sizeof(char) * (1 + strlen(authJson->valuestring)));
  if (auth_token == NULL) {
    (void)printf("Falha ao alocar memória\n");
    curl_easy_cleanup(curl_handle);
    cJSON_Delete(cjson_buf);
    curl_slist_free_all(list);
    free(chunk.memory);
    return 1;
  }
  (void)strcpy(auth_token, authJson->valuestring);
  authJson = NULL;

  temp = realloc(chunk.memory, 1);
  chunk.size = 0;
  curl_slist_free_all(list);
  list = NULL;
  if (temp) {
    chunk.memory = (char *)temp;
  } else {
    (void)printf("Falha ao realocar memória\n");
    curl_easy_cleanup(curl_handle);
    cJSON_Delete(cjson_buf);
    return 1;
  }
  cJSON_Delete(cjson_buf);
  cjson_buf = NULL;

  res = get_pendentes(&list, curl_handle, request_buffer, &chunk, auth_token);

  if (res != CURLE_OK) {
    (void)fprintf(stderr, "error: %s\n", curl_easy_strerror(res));
    curl_easy_cleanup(curl_handle);
    curl_slist_free_all(list);
    free(chunk.memory);
    return 1;
  } else {
    //(void)printf("Size: %lu\n", (unsigned long)chunk.size);
    //(void)printf("Data: %s\n", chunk.memory);
  }

  (void)printf("\nretorno dos pendentes:\n%s\n\n", chunk.memory);

  cjson_buf = cJSON_Parse(chunk.memory);
  if (!cjson_buf) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr) {
      (void)fprintf(stderr, "Error before: %s\n", error_ptr);
    }
  }

  titulos =
      cJSON_Duplicate(cJSON_GetObjectItem(cjson_buf, "titulos"), cJSON_True);
  if (titulos == NULL) {
    (void)printf("Falha ao alocar memória\n");
    curl_easy_cleanup(curl_handle);
    cJSON_Delete(cjson_buf);
    curl_slist_free_all(list);
    free(chunk.memory);
    return 1;
  }

  time_t t = time(NULL);
  struct tm date = *localtime(&t);
  (void)sprintf(data_atual, "%02d/%02d/%d", date.tm_mday, date.tm_mon + 1,
                date.tm_year + 1900);

  (void)printf("data: %s\n", data_atual);

  temp = realloc(chunk.memory, 1);
  chunk.size = 0;
  curl_slist_free_all(list);
  list = NULL;
  if (temp) {
    chunk.memory = (char *)temp;
  } else {
    (void)printf("Falha ao realocar memória\n");
    curl_easy_cleanup(curl_handle);
    cJSON_Delete(cjson_buf);
    return 1;
  }
  cJSON_Delete(cjson_buf);
  cjson_buf = NULL;
  curl_easy_reset(curl_handle);

  set_headers_renovacao(&list, request_buffer);

  FILE *f = fopen("log.json", "a+");

  cJSON_ArrayForEach(titulo, titulos) {

    cJSON *dataDev = cJSON_GetObjectItem(titulo, "dataDevolucaoPrevista");

    (void)printf("dataDev: %s\n", dataDev->valuestring);

    if (strcmp(data_atual, dataDev->valuestring) == 0) {
      if ((char)*cJSON_GetObjectItem(titulo, "qtdeRenovacoes")->valuestring >=
              '5' ||
          cJSON_GetObjectItem(titulo, "reservado")->valueint) {

        printf("Dia de devolver o livro: %s\n",
               cJSON_GetObjectItem(titulo, "descricao")->valuestring);
      } else {

        cJSON *renov_arr = cJSON_CreateArray();
        cJSON *temp = cJSON_CreateObject();
        cJSON *aux = cJSON_CreateString(
            cJSON_GetObjectItem(titulo, "codExemplar")->valuestring);

        cJSON_AddItemToObject(temp, "codExamplar", aux);

        aux = cJSON_CreateString(
            cJSON_GetObjectItem(titulo, "codBibliotecaExemplar")->valuestring);

        cJSON_AddItemToObject(temp, "codBibliotecaExemplar", aux);

        cJSON_AddItemToArray(renov_arr, temp);
        temp = NULL;
        aux = NULL;

        char *data_raw = cJSON_PrintUnformatted(renov_arr);

        printf("Dia de devolver o livro: %s\n",
               cJSON_GetObjectItem(titulo, "descricao")->valuestring);

        curl_easy_setopt(
            curl_handle, CURLOPT_URL,
            "https://biblioteca.utfpr.edu.br/api/emprestimo/renovacao");
        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, list);
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, data_raw);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION,
                         WriteMemoryCallback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

        res = curl_easy_perform(curl_handle);

        if (res != CURLE_OK) {
          (void)fprintf(stderr, "error: %s\n", curl_easy_strerror(res));
          curl_easy_cleanup(curl_handle);
          curl_slist_free_all(list);
          free(chunk.memory);
          return 1;
        } else {
          //printf("Size: %lu\n", (unsigned long)chunk.size);
          //printf("Data: %s\n", chunk.memory);
        }
        fprintf(f, "\n");
        fprintf(f, "%s", chunk.memory);
        fprintf(f, "\n");

        free(data_raw);
        cJSON_Delete(renov_arr);
      }
    } else
      printf("Não é dia de devolver o livro: %s\n",
             cJSON_GetObjectItem(titulo, "descricao")->valuestring);
  }
  fclose(f);

  /* KILL'EM ALL */
  curl_slist_free_all(list);
  curl_easy_cleanup(curl_handle);
  cJSON_Delete(titulos);
  free(chunk.memory);
  free(auth_token);
  /* KILL'EM ALL */
  return 0;
}
