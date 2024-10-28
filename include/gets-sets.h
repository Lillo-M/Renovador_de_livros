#ifndef _GETS_SETS_
#define _GETS_SETS_
#include <curl/curl.h>
#include "utils.h"
CURLcode get_auth_token(struct curl_slist **list, CURL *curl_handle,
                        char *request_buffer, struct MemoryStruct *chunk);
CURLcode get_pendentes(struct curl_slist **list, CURL *curl_handle,
                       char *request_buffer, struct MemoryStruct *chunk,
                       char *auth_token);
CURLcode get_nome(struct curl_slist **list, CURL *curl_handle,
                       char *request_buffer, struct MemoryStruct *chunk,
                       char *auth_token);
void set_headers_renovacao(struct curl_slist **list, char* request_buffer);
#endif // !_GETS_SETS_

