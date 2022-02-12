#include <json-c/json.h>
#include <stdio.h>

#include "../include/botcommands.h"
#include "../include/telegram.h"

void
cmdimpl_start(json_object *event) {
  const char* chat_id;

  chat_id = json_object_get_string(
      tg_json_getpathobject(event, "chat", "id", 0)
      );
  tg_request("sendMessage", 
      topt_new("chat_id", chat_id,
        topt_new("text", "Мой автор дегенерат! https://github.com/Amchik", 0)));
}

TelegramEvent cmdstart = TelegramEvent$command("start", cmdimpl_start);

