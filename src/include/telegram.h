/* vim: ft=c tabstop=2 shiftwidth=2
 * 
 * Telegram base API client implementation
 */
#ifndef __oFcZJH_telegram_h
#define __oFcZJH_telegram_h

#include <json-c/json_types.h>

__attribute__((deprecated("See tg_request")))
extern char *TELEGRAM_TOKEN;

/*
 * Telegram request option. No need to format to URL.
 * Use topt_new to create one
 */
typedef struct _telegram_otps* topts_t;
/*
 * Telegram request option. No need to format to URL.
 * Use topt_new to create one
 */
struct _telegram_otps {
  const char *key;
  const char *value;
  topts_t next;
};

typedef struct {
  char *telegram_token;

  topts_t query;
} tgreq_context;

/*
 * Allocs topts_t in memory
 */
topts_t topt_new(const char *key, const char *value, topts_t next);

/*
 * Make request to telegram api. opts after request will be free.
 */
__attribute__((deprecated("tg_request uses global context. "
        "Please use tg_makereq with local context")))
json_object* tg_request(char *method, topts_t opts);

/*
 * Make request to telegram api.
 */
json_object* tg_makereq(const char *telegram_token, const char *method, topts_t opts);

/*
 * Getting json_object by path, like ("chat", "id")
 */
json_object* tg_json_getpathobject(json_object *json, ...);

#endif

