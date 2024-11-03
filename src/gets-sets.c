#include "../include/gets-sets.h"



CURLcode get_auth_token(struct curl_slist **list, CURL *curl_handle,
                        char *request_buffer, struct MemoryStruct *chunk) {
  *list =
      curl_slist_append(*list, "Content-Type: application/json;charset=utf-8");

  (void)curl_easy_setopt(curl_handle, CURLOPT_URL,
                         "https://biblioteca.utfpr.edu.br/oauth/token");
  (void)curl_easy_setopt(curl_handle, CURLOPT_CAINFO, PATH "intermediate.pem");
  (void)curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, *list);
  (void)curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, request_buffer);
  (void)curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)chunk);
  (void)curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  return curl_easy_perform(curl_handle);
}

CURLcode get_pendentes(struct curl_slist **list, CURL *curl_handle,
                       char *request_buffer, struct MemoryStruct *chunk,
                       char *auth_token) {
  curl_easy_reset(curl_handle);

  *list = curl_slist_append(*list, "Accept: application/json, text/plain, */*");
  *list = curl_slist_append(*list, "Accept-Language: en-US,en;q=0.5");
  *list = curl_slist_append(*list, "Accept-Encoding: gzip, deflate, br, zstd");
  *list = curl_slist_append(*list, "lang: 1");
  *list = curl_slist_append(*list, "Connection: keep-alive");

  (void)sprintf(request_buffer, "Authorization: Bearer %s", auth_token);

  *list = curl_slist_append(*list, request_buffer);

  (void)curl_easy_setopt(curl_handle, CURLOPT_URL,
                         "https://biblioteca.utfpr.edu.br/api/emprestimo/"
                         "titulos-pendentes?showDetails=1");
  (void)curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, *list);
  (void)curl_easy_setopt(curl_handle, CURLOPT_CAINFO, PATH "intermediate.pem");
  (void)curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION,
                         WriteMemoryCallback);
  (void)curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)chunk);
  (void)curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  return curl_easy_perform(curl_handle);
}


CURLcode get_nome(struct curl_slist **list, CURL *curl_handle,
                       char *request_buffer, struct MemoryStruct *chunk,
                       char *auth_token) {
  curl_easy_reset(curl_handle);

  *list = curl_slist_append(*list, "Accept: application/json, text/plain, */*");
  *list = curl_slist_append(*list, "Accept-Language: en-US,en;q=0.5");
  *list = curl_slist_append(*list, "Accept-Encoding: gzip, deflate, br, zstd");
  *list = curl_slist_append(*list, "lang: 1");
  *list = curl_slist_append(*list, "Connection: keep-alive");

  (void)sprintf(request_buffer, "Authorization: Bearer %s", auth_token);

  *list = curl_slist_append(*list, request_buffer);

  (void)curl_easy_setopt(curl_handle, CURLOPT_URL,
                         "https://biblioteca.utfpr.edu.br/api/pessoas");
  (void)curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, *list);
  (void)curl_easy_setopt(curl_handle, CURLOPT_CAINFO, PATH "intermediate.pem");
  (void)curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION,
                         WriteMemoryCallback);
  (void)curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)chunk);
  (void)curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  return curl_easy_perform(curl_handle);
}

void set_headers_renovacao(struct curl_slist **list, char* request_buffer){
  *list = curl_slist_append(*list, "Accept: application/json");
  *list = curl_slist_append(*list, "Accept-Language: en-US,en;q=0.5");
  *list = curl_slist_append(*list, "Accept-Encoding: gzip, deflate, br, zstd");
  *list = curl_slist_append(*list, "lang: 1");
  *list =
      curl_slist_append(*list, "Content-Type: application/json;charset=utf-8");
  *list = curl_slist_append(*list, "Connection: keep-alive");
  *list = curl_slist_append(*list, "Priority: u=0");
  *list = curl_slist_append(*list, request_buffer);
  *list = curl_slist_append(*list, "Sec-Fetch-Dest: empty");
  *list = curl_slist_append(*list, "Sec-Fetch-Mode: cors");
  *list = curl_slist_append(*list, "Sec-Fetch-Site: same-origin");
  *list = curl_slist_append(
      *list, "Referer: https://biblioteca.utfpr.edu.br/meupergamum");
  *list = curl_slist_append(*list, "Origin: https://biblioteca.utfpr.edu.br");
}
