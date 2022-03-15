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

void
handlesegfault(int code) {
  fprintf(stderr, "\r\033[1;31m[FATAL]\033[0m Recieved signal \033[1m%d\033[0m,"
      " \033[1m%s\033[0m. Exiting...\n",
      code, strsignal(code));
  exit(3);
}

void
inittelegramtoken(char **token) {
  char tokenbuff[64];
  int64_t botid;
  size_t tokenoffset;
  FILE *fp;

  *token = getenv("TELEGRAM_TOKEN");
  if (!*token) {
    fp = fopen(".telegram_token", "r");
    if (!fp) {
      fprintf(stderr, "\r\033[31m[ERROR]\033[0m No token provided. Please set $TELEGRAM_TOKEN"
          " enviroment variable or create file \033[1m.telegram_token\033[0m\n");
      if (errno) {
        fprintf(stderr, "\r\033[33m[NOTICE]\033[0m Failed to read file "
            "\033[1m.telegram_token\033[0m: (errno %d) %s\n", errno, strerror(errno));
      }
      exit(1);
    }
    *token = calloc(128, 1);
    tokenoffset = fread(*token, 1, 127, fp);
    fclose(fp);
  } else {
    tokenoffset = strlen(*token);
  }

  if ((*token)[tokenoffset - 1] == '\n') {
    (*token)[tokenoffset - 1] = '\0';
  }

  sscanf(*token, "%ld:%63s", &botid, tokenbuff);
  if (/*errno
      ||*/ (tokenoffset = strlen(tokenbuff)) <= 4) {
    fprintf(stderr, "\r\033[31m[ERROR]\033[0m Invalid token provided\n");
    if (errno) puts("FFF");
    /*if (errno) {
      fprintf(stderr, "\r\033[33m[NOTICE]\033[0m Failed to parse token string: "
          "(errno %d) %s\n", errno, strerror(errno));
    }*/
    exit(1);
  }
  tokenoffset -= 4;
  printf("\r\033[32m[OK]\033[0m Found token for user \033[1m%ld\033[0m "
      "ends for …\033[1m%s\033[0m\n",
      botid, tokenbuff + tokenoffset);
}

void
handle_event_error(tgbot_event ev, int code) {
  if (code != 0) {
    printf("\033[31m[ERROR]\033[0m Failed to execute event "
        "\033[1m%s\033[0m (type 0x%X): %s %d (%s)\n",
        ev.handler->name, ev.handler->type, 
        code < 0 ? "recieved signal" : "error",
        code < 0 ? -code : code,
        code < 0 ? strsignal(-code) : strerror(code));
  }
}

int
main(void) {
  tgbot_context ctx;
  char *error;
  json_object *updates, *update;
  tgbot_event ev;
  int updates_count, i;

  signal(SIGSEGV, handlesegfault);
  signal(SIGABRT, handlesegfault);
  signal(SIGIOT,  handlesegfault);
  signal(SIGFPE,  handlesegfault);
  signal(SIGINT,  handlesegfault);
  signal(SIGTERM, handlesegfault);

  inittelegramtoken(&ctx.telegram_token);

  if ((i = tgbot_init(&ctx)) != 0) {
    printf("\033[31m[ERROR]\033[0m Failed to init tgbot context (error code %d).\n", i);
    exit(1);
  }
  printf("\033[34m[INFO]\033[0m Logged as \033[1m@%s\033[0m\n", ctx.selfname);
  while (1) {
    updates_count = tgbot_readupdates(&ctx, &updates, &error);
    if (updates_count == 0) {
      continue;
    } else if (updates_count < 0) {
      fprintf(stderr, "\033[33m[WARN]\033[0m An error occured while fetching "
          "updates: %d: %s\n", -updates_count, error);
      sleep(5);
      continue;
    }

    for (i = 0; i < updates_count; i++) {
      update = json_object_array_get_idx(updates, i);
      
      ev = tgbot_parseupdate(&ctx, update);
      if (!ev.handler)
        continue;
      
      tgbot_event_run(ev, &handle_event_error);
    }

    json_object_put(updates);
  }

  return(0);
}
