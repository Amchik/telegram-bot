#include <json-c/json.h>
#include <json-c/json_object.h>
#include <stdio.h>
#include <string.h>

#include "../include/botcommands.h"
#include "../include/telegram.h"
#include "../include/matestr.h"

void
cmdimpl_msginfo(json_object *event) {
  const char *chat_id, *message_id;
  json_object *reply_to_message;
  char text[8192];

  chat_id = json_object_get_string(
      tg_json_getpathobject(event, "chat", "id", 0)
      );
  message_id = json_object_get_string(
      tg_json_getpathobject(event, "message_id", 0)
      );

  reply_to_message = tg_json_getpathobject(event, "reply_to_message", 0);

  tg_request("sendMessage", 
      topt_new("chat_id", chat_id,
        topt_new("reply_to_message_id", message_id,
          topt_new("text", reply_to_message ? json_object_to_json_string(reply_to_message) : "Reply to message for get data", 0))));
}

void
cmdimpl_memdump(json_object *event) {
  const char *chat_id, *message_id, *msgtext;
  unsigned long user_id;
  char text[512];
  size_t idx;
  unsigned int count, i;

  chat_id = json_object_get_string(
      tg_json_getpathobject(event, "chat", "id", 0)
      );
  user_id = json_object_get_uint64(
      tg_json_getpathobject(event, "from", "id", 0)
      );
  message_id = json_object_get_string(
      tg_json_getpathobject(event, "message_id", 0)
      );
  msgtext = json_object_get_string(
      tg_json_getpathobject(event, "text", 0)
      );

  if (user_id != 343949606) {
    return;
  }

  for (idx = 0; ' ' != msgtext[idx]
      && 0 != msgtext[idx]; idx++);
  if (msgtext[idx] == 0) {
    return;
  }
  if (!sscanf(msgtext + idx + 1, "%lx %u", &idx, &count) || count > 150) {
    return;
  }

  sprintf(text, "Result of reading %p: ", (void*)idx);
  
  for (i = 0; i < count; i++) {
    char __buff[3];

    sprintf(__buff, "%.2x", *(char*)(idx + i));
    strcat(text, __buff);
  }

  tg_request("sendMessage", 
      topt_new("chat_id", chat_id,
        topt_new("reply_to_message_id", message_id,
          topt_new("text", text, 0))));
}

TelegramEvent cmdmsginfo = TelegramEvent$command("msginfo", cmdimpl_msginfo);
TelegramEvent cmdmemdump = TelegramEvent$command("memdump", cmdimpl_memdump);

