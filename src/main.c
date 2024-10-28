#include "../include/cJSON.h"
#include "../include/gets-sets.h"
#include "../include/utils.h"
#include <curl/curl.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REQUEST_AUTH_TOKE_JSON                                                 \
  "{\"grant_type\":\"password\",\"client_secret\":"                            \
  "\"H2oPoUivqUvbHXaYg4TBURKfh1UaFWx2Vz8F8Mpl\",\"client_id\":\"4\","          \
  "\"scope\":\"\",\"username\":\"%s\",\"password\":\"%s\"}"

#define CHECK_CJSON_PARSE(X)                                                   \
  if (!X) {                                                                    \
    const char *error_ptr = cJSON_GetErrorPtr();                               \
    if (error_ptr) {                                                           \
      (void)fprintf(stderr, "Error before: %s\n", error_ptr);                  \
    } else {                                                                   \
      (void)fprintf(stderr, "Error: cJSON_Parse return NULL\n");               \
    }                                                                          \
    return 1;                                                                  \
  }

int main() {
  cJSON *cjson_buf = NULL;
  cJSON *data_dev = NULL;
  cJSON *titulos = NULL;
  cJSON *titulo = NULL;
  char *auth_token = NULL;
  char *temp = NULL;
  char data_atual[16] = "";
  char request_buffer[2048] = "";
  char nome_Aluno[256] = "";
  char rAcademico[9] = "";
  char senha[8] = "";
  CURL *curl_handle = NULL;
  CURLcode res;
  struct curl_slist *list = NULL;
  struct MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;
  (void)setlocale(LC_ALL, "");

  FILE *userdata = fopen(PATH "userdata.txt", "r");
  int ret = get_user_data(senha, rAcademico, userdata);
  while (ret == SUCESS_GET_USER_DATA) {

    if (sprintf(request_buffer, REQUEST_AUTH_TOKE_JSON, rAcademico, senha) <
        0) {
      (void)fprintf(stderr, "Failed to load userdata to request buffer\n");
      return 1;
    }

    curl_handle = curl_easy_init();
    if (!curl_handle) {
      (void)fprintf(stderr, "Falha ao alocar memória: Curl\n");
      return 1;
    }
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

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
    CHECK_CJSON_PARSE(cjson_buf)

    if (cJSON_GetObjectItem(cjson_buf, "message")) {
      (void)fprintf(stderr, "ERROR: %s\n",
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

    chunk.memory = realloc(chunk.memory, 1);
    chunk.size = 0;
    curl_slist_free_all(list);
    list = NULL;
    cJSON_Delete(cjson_buf);
    cjson_buf = NULL;
    if (chunk.memory == NULL) {
      (void)fprintf(stderr, "Falha ao realocar memória, errno: %d\n", errno);
      curl_easy_cleanup(curl_handle);
      return 1;
    }

    res = get_pendentes(&list, curl_handle, request_buffer, &chunk, auth_token);
    while (res == CURLE_COULDNT_RESOLVE_HOST) {
      sleep(10);
      res = curl_easy_perform(curl_handle);
    }

    if (res != CURLE_OK) {
      (void)fprintf(stderr, "error: %s\n", curl_easy_strerror(res));
      curl_easy_cleanup(curl_handle);
      curl_slist_free_all(list);
      free(auth_token);
      free(chunk.memory);
      return 1;
    } else {
      //(void)printf("Size: %lu\n", (unsigned long)chunk.size);
      //(void)printf("Data: %s\n", chunk.memory);
    }

    //(void)printf("\nretorno dos pendentes:\n%s\n\n", chunk.memory);

    cjson_buf = cJSON_Parse(chunk.memory);
    CHECK_CJSON_PARSE(cjson_buf)

    titulos =
        cJSON_Duplicate(cJSON_GetObjectItem(cjson_buf, "titulos"), cJSON_True);
    if (titulos == NULL) {
      (void)fprintf(stderr, "Falha ao alocar memória: cJSON\n");
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

    chunk.size = 0;
    curl_slist_free_all(list);
    list = NULL;
    chunk.memory = realloc(chunk.memory, 1);
    cJSON_Delete(cjson_buf);
    cjson_buf = NULL;
    if (chunk.memory == NULL) {
      (void)fprintf(stderr, "Falha ao realocar memória, errno: %d\n", errno);
      curl_easy_cleanup(curl_handle);
      return 1;
    }
    curl_easy_reset(curl_handle);

    res = get_nome(&list, curl_handle, request_buffer, &chunk, auth_token);
    while (res == CURLE_COULDNT_RESOLVE_HOST) {
      sleep(10);
      res = curl_easy_perform(curl_handle);
    }

    if (res != CURLE_OK) {
      (void)fprintf(stderr, "error: %s\n", curl_easy_strerror(res));
      curl_easy_cleanup(curl_handle);
      curl_slist_free_all(list);
      free(auth_token);
      free(chunk.memory);
      cJSON_Delete(titulos);
      return 1;
    } else {
      //(void)printf("Size: %lu\n", (unsigned long)chunk.size);
      //(void)printf("Data: %s\n", chunk.memory);
    }

    cjson_buf = cJSON_Parse(chunk.memory);
    CHECK_CJSON_PARSE(cjson_buf)
    (void)strcpy(nome_Aluno,
                 cJSON_GetObjectItem(cJSON_GetObjectItem(cjson_buf, "user"),
                                     "nome_pessoa")
                     ->valuestring);

    chunk.memory = realloc(chunk.memory, 1);
    chunk.size = 0;
    curl_slist_free_all(list);
    list = NULL;
    cJSON_Delete(cjson_buf);
    cjson_buf = NULL;
    if (chunk.memory == NULL) {
      (void)fprintf(stderr, "Falha ao realocar memória, errno: %d\n", errno);
      curl_easy_cleanup(curl_handle);
      return 1;
    }

    set_headers_renovacao(&list, request_buffer);

    (void)sprintf(request_buffer, PATH "logs/log-%02d-%02d-%d-%02dh%02d.json",
                  date.tm_mday, date.tm_mon + 1, date.tm_year + 1900,
                  date.tm_hour, date.tm_min);
    FILE *logFile = fopen(request_buffer, "a+");
    if (logFile) {
      (void)fprintf(logFile, "{\n   \"%s\":", nome_Aluno);
    }

    cJSON_ArrayForEach(titulo, titulos) {

      cJSON *dataDev = cJSON_GetObjectItem(titulo, "dataDevolucaoPrevista");

      (void)fprintf(stdout, "dataDev: %s\n", dataDev->valuestring);

      if (strcmp(data_atual, dataDev->valuestring) == 0) {
        if ((char)*cJSON_GetObjectItem(titulo, "qtdeRenovacoes")->valuestring >=
                '5' ||
            cJSON_GetObjectItem(titulo, "reservado")->valueint) {

          (void)fprintf(stdout, "Dia de devolver o livro: %s\n",
                        cJSON_GetObjectItem(titulo, "descricao")->valuestring);
          system("feh --bg-scale " PATH "alert_red.png");// 
          // Isto é uma maneira de me alertar sobre algum livro
          // que precisa ser devolvido.
          // Você pode colocar o que preferir, como e-mail,
          // ou algum outro metodo de te avisar.

        } else {

          cJSON *renov_arr = cJSON_CreateArray();
          cJSON *temp = cJSON_CreateObject();
          cJSON *aux = cJSON_CreateString(
              cJSON_GetObjectItem(titulo, "codExemplar")->valuestring);

          cJSON_AddItemToObject(temp, "codExamplar", aux);

          aux = cJSON_CreateString(
              cJSON_GetObjectItem(titulo, "codBibliotecaExemplar")
                  ->valuestring);

          cJSON_AddItemToObject(temp, "codBibliotecaExemplar", aux);

          cJSON_AddItemToArray(renov_arr, temp);
          temp = NULL;
          aux = NULL;

          char *data_raw = cJSON_PrintUnformatted(renov_arr);

          (void)fprintf(stdout, "Dia de devolver o livro: %s\n",
                        cJSON_GetObjectItem(titulo, "descricao")->valuestring);

          (void)curl_easy_setopt(
              curl_handle, CURLOPT_URL,
              "https://biblioteca.utfpr.edu.br/api/emprestimo/renovacao");
          (void)curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, list);
          (void)curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, data_raw);
          (void)curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION,
                                 WriteMemoryCallback);
          (void)curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,
                                 (void *)&chunk);
          (void)curl_easy_setopt(curl_handle, CURLOPT_USERAGENT,
                                 "libcurl-agent/1.0");

          res = curl_easy_perform(curl_handle);
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
            // printf("Size: %lu\n", (unsigned long)chunk.size);
            // printf("Data: %s\n", chunk.memory);
          }
          if (logFile) {
            (void)fprintf(logFile, "%s", chunk.memory);
          }
          free(data_raw);
          cJSON_Delete(renov_arr);
        }
      } else
        (void)fprintf(stdout, "Não é dia de devolver o livro: %s\n",
                      cJSON_GetObjectItem(titulo, "descricao")->valuestring);
    }

    if (logFile) {
      (void)fprintf(logFile, "\n}");
      fclose(logFile);
    }
    (void)sprintf(request_buffer, PATH "logs/log-%02d-%02d-%d-%02dh%02d.txt",
                  date.tm_mday, date.tm_mon + 1, date.tm_year + 1900,
                  date.tm_hour, date.tm_min);
    logFile = fopen(request_buffer, "a+");
    if (logFile) {
      (void)fprintf(logFile, "Aluno: %s\n", nome_Aluno);
      (void)fprintf(logFile, "data: %s\n", data_atual);
      (void)fprintf(logFile, "titulos-testados--\n");
      cJSON_ArrayForEach(titulo, titulos) {
        (void)fprintf(logFile, "titulo: %s\n",
                      cJSON_GetObjectItem(titulo, "descricao")->valuestring);
      }
      fclose(logFile);
    }
    ret = get_user_data(senha, rAcademico, userdata);
    /* KILL'EM ALL */
    curl_slist_free_all(list);
    list = NULL;
    curl_easy_cleanup(curl_handle);
    cJSON_Delete(titulos);
    chunk.memory = realloc(chunk.memory, 1);
    chunk.size = 0;
    cJSON_Delete(cjson_buf);
    free(auth_token);
    if (chunk.memory == NULL) {
      (void)fprintf(stderr, "Falha ao realocar memória, errno: %d\n", errno);
      return 1;
    }
  }
  (void)user_data_handle_ret(ret);

  if (userdata)
    fclose(userdata);
  free(chunk.memory);
  return 0;
}
