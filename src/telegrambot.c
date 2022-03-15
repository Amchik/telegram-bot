#include <json-c/json_object.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#include "include/botcommands.h"
#include "include/telegram.h"
#include "include/telegrambot.h"
#include "include/matestr.h"

typedef struct {
  tgcbfn_t *eviter;
  json_object *buff;
} runcmd_context;
typedef struct {
  tgbot_event context;
} tgbot_runcontext;
typedef struct {
  tgbot_event ctx;
  void (*then)(tgbot_event ev, int code);
} _tgbot__adapter_ctx_then;

extern tgcbfn_t __start_tgevents;
extern tgcbfn_t __stop_tgevents;

void
_tgbot__getcommandname(matestr *str, const char *text, const char *selfname) {
  const char *s;

  str->length = 0;
  str->cstr[0] = '\0';

  for (s = text; *s != 0 && *s != ' ' && *s != '@'; s += 1);

  if (s - text == 0) {
    return;
  }
  if (*s == '@' && strcmp(s + 1, selfname)) {
    return;
  }
  matestr_append(str, text, s - text);

  if (str->__allocated - str->length > 40) {
    matestr_optimize(str);
  }
}

void
_tgbot__run_command_newthread_signal(int signal) {
  pthread_exit((void*)(size_t)signal);
}

int
tgbot_init(tgbot_context *ctx) {
  json_object *root;
  const char *textbuff;
  
  ctx->last_update_id = 0;

  root = tg_makereq(ctx->telegram_token, "getMe", 0);
  if (!json_object_get_boolean(json_object_object_get(root, "ok"))) {
    return(1);
  }
  textbuff = json_object_get_string(
      tg_json_getpathobject(root, "result", "username", 0)
      );
  strncpy(ctx->selfname, textbuff, sizeof(ctx->selfname) - 1);
  json_object_put(root);

  return(0);
}

int
tgbot_readupdates(tgbot_context *ctx, json_object **updates, char **error) {
  json_object *buff, *buff2;
  char offset[12];
  int updates_count;

  offset[0] = '\0';
  sprintf(offset, "%lu", ctx->last_update_id);

  *updates = tg_makereq(ctx->telegram_token, "getUpdates", 
      topt_new("timeout", "20",
        topt_new("limit", "10",
        topt_new("offset", offset, 0))));
  if (!json_object_get_boolean(
        json_object_object_get(*updates, "ok"))) {
    int error_code, desclen;
    const char *description;

    error_code = json_object_get_int(
        tg_json_getpathobject(*updates, "error_code", 0));

    if (error) {
      description = json_object_get_string(
          tg_json_getpathobject(*updates, "description", 0));
      desclen = strlen(description);
      *error = malloc(desclen + 1);
      memcpy(*error, description, desclen + 1);
    }

    json_object_put(*updates);
    *updates = 0;
    return(-error_code);
  }
  buff = json_object_object_get(*updates, "result");
  updates_count = json_object_array_length(buff);
  if (updates_count > 0) {
    buff2 = json_object_array_get_idx(buff,
        json_object_array_length(buff) - 1);
    ctx->last_update_id = json_object_get_uint64(
        json_object_object_get(buff2, "update_id")
        ) + 1;
  }
  buff2 = *updates;
  *updates = 0;
  if (0 != json_object_deep_copy(buff, updates, 0)) {
    json_object_put(buff2);
    if (error)
      *error = 0; /* saphety */
    return(-1);
  }
  json_object_put(buff2);
  return(updates_count);
}

tgbot_event
tgbot_parseupdate(tgbot_context *ctx, const json_object *update) {
  json_object *buff, *buff2;
  const char *textbuff;
  matestr mbuff;
  tgcbfn_t *iter;
  tgbot_event event;

  mbuff = matestralloc(1);

  event.bot = ctx;
  event.telegram_context = 0;
  event.handler = 0;

  for (iter = &__start_tgevents;
      iter < &__stop_tgevents; 
      iter = (void*)((size_t)iter + 0x120))
    switch (iter->type) {
      /* on message */
      case TGCB_MESSAGE:
      case TGCB_TEXT:
      case TGCB_COMMAND:
        {
          if (!(buff = json_object_object_get(update, "message"))) {
            break;
          }
          if (iter->type != TGCB_MESSAGE && /* text | command */
              !(buff2 = json_object_object_get(buff, "text")))
            break;
          textbuff = json_object_get_string(buff2);
          if (iter->type == TGCB_COMMAND) {
            if ('/' != *textbuff)
              break;
            _tgbot__getcommandname(&mbuff, textbuff + 1, ctx->selfname);
            if (strcmp(iter->name, mbuff.cstr) != 0)
              break;
          }
          json_object_deep_copy(buff, &event.telegram_context, 0);
          event.handler = iter;
          break;
        }

      default:
        break;
    }

  free(mbuff.cstr);

  return event; /* TODO: more than one update */
}

void*
_tgbot__event_thread(void *_ctx) {
  tgbot_runcontext ctx;
  register char i;

  ctx = *(tgbot_runcontext*)_ctx;

  for (i = 0; i < 35; i++)
    signal(i, _tgbot__run_command_newthread_signal);

  ctx.context.handler->func(ctx.context);

  return(0);
}

void*
_tgbot__event_run_async_adapter(void *_arg) {
  tgbot_event ctx;
  void (*then)(tgbot_event, int);
  int result;

  ctx = ((_tgbot__adapter_ctx_then*)_arg)->ctx;
  then = ((_tgbot__adapter_ctx_then*)_arg)->then;

  free(_arg);

  result = tgbot_event_run_sync(ctx);
  if (then) {
    then(ctx, result);
  } else {
    free(ctx.telegram_context);
  }

  return(0);
}

int
tgbot_event_run_sync(tgbot_event event) {
  pthread_t thread;
  tgbot_runcontext runctx;
  int err, code;

  runctx.context = event;

  pthread_create(&thread, 0, &_tgbot__event_thread, &runctx);

  err = pthread_join(thread, (void**)&code);

  return(err ? err : -code);
}

void
tgbot_event_run(tgbot_event event, void (*then)(tgbot_event ev, int code)) {
  pthread_t thread;
  _tgbot__adapter_ctx_then *_arg;

  _arg = malloc(sizeof(_tgbot__adapter_ctx_then)); /* because of me */
  _arg->ctx = event;
  _arg->then = then;

  pthread_create(&thread, 0, &_tgbot__event_run_async_adapter, _arg);
}

