#include <json-c/json.h>

#include "botcommands.h"

#ifndef __VjQChN_telegrambot_h
#define __VjQChN_telegrambot_h

/*
 * Telegram bot context.
 */
typedef struct {
  char *telegram_token;

  u_int64_t last_update_id;
  char selfname[33];
} tgbot_context;


/*
 * Telegram event launch context.
 */
typedef struct {
  struct TelegramCallbackFunction *handler;

  json_object *telegram_context;
  tgbot_context *bot;
} tgbot_event;

/*
 * Telegram event callback function types.
 */
enum TelegramCallback {
  TGCB_COMMAND,
  TGCB_TEXT,
  TGCB_MESSAGE
};

/*
 * Telegram event callback function.
 * See botcommands.h
 */
struct TelegramCallbackFunction {
  char name[255];
  enum TelegramCallback type;
  void (*func) (tgbot_event event);
};

/*
 * DEPRECATED
 * Please use struct TelegramCallbackFunction. This
 * typedef will be removed in future.
 */
typedef struct TelegramCallbackFunction tgcbfn_t;

/*
 * Initialize bot context.
 * Returns 0 on success, 1 if fail on /getMe.
 */
__attribute__((nonnull(1)))
int tgbot_init(tgbot_context *ctx);

/*
 * Read updates and writes array of updates to updates.
 * Returns update count on success, or -error_code and
 * error (if not null) will be set.
 */
__attribute__((nonnull(1, 2)))
int tgbot_readupdates(tgbot_context *ctx, json_object **updates, char **error);

/*
 * Parse one update and returns event context.
 */
__attribute__((nonnull(1, 2)))
tgbot_event tgbot_parseupdate(tgbot_context *ctx, const json_object *update);

/*
 * Run event by context.
 * Returns 0 on success, -termination code (like SIGSEGV),
 * or errno.
 */
int tgbot_event_run_sync(tgbot_event event);

/*
 * Run event by context in new thread.
 * Calls then (if not null) and pass code:
 * 0 on success, -termination code (like SIGSEGV), or errno.
 */
void tgbot_event_run(tgbot_event event, void (*then)(tgbot_event ev, int code));

#endif

