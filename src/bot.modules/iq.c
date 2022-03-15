#include <json-c/json.h>
#include <json-c/json_object.h>
#include <stdio.h>

#include "../include/botcommands.h"
#include "../include/telegram.h"
#include "../include/telegrambot.h"

void
cmdimpl_iq(tgbot_event ctx) {
  const char *chat_id, *user_id, *message_id;
  json_object *event;
  char text[1024];

  event = ctx.telegram_context;

  chat_id = json_object_get_string(
      tg_json_getpathobject(event, "chat", "id", 0)
      );
  user_id = json_object_get_string(
      tg_json_getpathobject(event, "from", "id", 0)
      );
  message_id = json_object_get_string(
      tg_json_getpathobject(event, "message_id", 0)
      );

  sprintf(text, "Ваш IQ: %d. Вы сэкономили %d IQ!", 120, 69);

  tg_makereq(ctx.bot->telegram_token, "sendMessage",
      topt_new("chat_id", chat_id,
        topt_new("text", text,
          topt_new("reply_to_message_id", message_id, 0))));
}

//TelegramEvent cmdiq    = TelegramEvent$command("iq",    cmdimpl_iq);

TELEGRAM_EVENT(cmdiq, "iq", TGCB_COMMAND, cmdimpl_iq);

