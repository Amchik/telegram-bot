#include <stdarg.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include "json-c/json.h"

#include "include/telegram.h"
#include "include/matestr.h"

char *TELEGRAM_TOKEN = 0;

topts_t
topt_new(const char *key, const char *value, topts_t next) {
  topts_t result;

  result = malloc(sizeof(struct _telegram_otps));
  result->key = key;
  result->value = value;
  result->next = next;

  return(result);
}

void
tg__curlwritter(void *ptr, size_t size, size_t nmemb, matestr *str) {
  matestr_append(str, (char*)ptr, size * nmemb);
}

void
tg__makeurl(matestr *str, char *method, topts_t opts) {
  topts_t buff, fbuff;
  char *escaped;

  matestr_append(str, "https://api.telegram.org/bot", 0);
  /* token, etc... */
  matestr_append(str, TELEGRAM_TOKEN, 0);
  matestr_append(str, "/", 0);
  matestr_append(str, method, 0);
  if (opts) {
    matestr_append(str, "?", 0);
    for (buff = opts; buff != 0; ) {
      escaped = curl_escape(buff->key, 0);
      matestr_append(str, escaped, 0);
      matestr_append(str, "=", 0);
      curl_free(escaped);
      escaped = curl_escape(buff->value, 0);
      matestr_append(str, escaped, 0);
      matestr_append(str, "&", 0);
      fbuff = buff;
      buff = buff->next;
      free(fbuff);
      curl_free(escaped);
    }
    str->length -= 1;
    matestr_normalize(str);
  }
}

json_object*
tg_request(char *method, topts_t opts) {
  CURL *curl;
  matestr str, url;
  json_object *result;

  str = matestralloc(1);
  url = matestralloc(1);

  tg__makeurl(&url, method, opts);

  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url.cstr);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, tg__curlwritter);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl/7.79.1 AmchikTelegramBot/1.0");
  /* i know this program cannot be compiled (or executed) on windows */
  curl_easy_perform(curl);
  curl_easy_cleanup(curl);

  result = json_tokener_parse(str.cstr);
  free(str.cstr);
  free(url.cstr);
  return(result);
}

json_object*
tg_json_getpathobject(json_object *json, ...) {
  va_list lst;
  json_object *answer;
  char *buffer;

  answer = json;
  va_start(lst, json);

  for (buffer = va_arg(lst, char*); buffer != 0; buffer = va_arg(lst, char*)) {
    answer = json_object_object_get(answer, buffer);
  }

  return(answer);
}

