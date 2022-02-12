#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json-c/json.h"
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "include/botcommands.h"
#include "include/telegram.h"
#include "include/matestr.h"

static u_int64_t last_update_id = 0;

extern tgcbfn_t __start_tgevents;
extern tgcbfn_t __stop_tgevents;

void
handlesegfault(int code) {
  fprintf(stderr, "\r\033[1;31m[FATAL]\033[0m Recieved signal \033[1m%d\033[0m,"
      " \033[1m%s\033[0m. Exiting...\n",
      code, strsignal(code));
  exit(3);
}

void
getcommandname(matestr *str, const char *text) {
  const char *s;

  str->length = 0;
  str->cstr[0] = '\0';

  for (s = text; *s != 0 && *s != ' ' && *s != '@'; s += 1);

  if (s - text == 0) {
    return;
  }
  matestr_append(str, text, s - text);

  if (str->__allocated - str->length > 40) {
    matestr_optimize(str);
  }

  return;
}

void
inittelegramtoken() {
  char *token, tokenbuff[64];
  int64_t botid;
  size_t tokenoffset;
  FILE *fp;

  token = getenv("TELEGRAM_TOKEN");
  if (!token) {
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
    token = calloc(128, 1);
    tokenoffset = fread(token, 1, 127, fp);
    fclose(fp);
  } else {
    tokenoffset = strlen(token);
  }

  if (token[tokenoffset - 1] == '\n') {
    token[tokenoffset - 1] = '\0';
  }

  sscanf(token, "%ld:%63s", &botid, tokenbuff);
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

  TELEGRAM_TOKEN = token;
}

int
main(void) {
  json_object *root, *buff, *buff2;
  char offset[12], selfname[33];
  const char *textbuff;
  matestr mbuff;
  tgcbfn_t *eviter;

  signal(SIGSEGV, handlesegfault);
  signal(SIGABRT, handlesegfault);
  signal(SIGIOT,  handlesegfault);
  signal(SIGFPE,  handlesegfault);
  signal(SIGINT,  handlesegfault);
  signal(SIGTERM, handlesegfault);

  mbuff = matestralloc(1);
  /*TELEGRAM_TOKEN = "1688715853:AAGpg3oBIE4ARSsGwuvKia3dTDnaQsp7Tus";*/
  inittelegramtoken();

  root = tg_request("getMe", 0);
  if (!json_object_get_boolean(json_object_object_get(root, "ok"))) {
    fprintf(stderr, "\r\033[31m[ERROR]\033[0m Failed to get self user object. "
        "Are token right?\n");
    return(2);
  }
  buff = json_object_object_get(root, "result");
  textbuff = json_object_get_string(json_object_object_get(buff, "username"));
  strncpy(selfname, textbuff, sizeof(selfname) - 1);
  json_object_put(root);
  printf("\r\033[32m[LOGGED IN]\033[0m As \033[1m@%s\033[0m\n", selfname);

  while (1) {
    offset[0] = '\0';
    sprintf(offset, "%lu", last_update_id);

    root = tg_request("getUpdates", 
        topt_new("timeout", "40",
          topt_new("offset", offset,
            topt_new("limit", "1", 0))));
    if (!json_object_get_boolean(json_object_object_get(root, "ok"))) {
      fprintf(stderr, "\r\033[33m[WARN]\033[0m Failed to fetch updates.\n");
      json_object_put(root);
      sleep(5);
      continue;
    }
    buff = json_object_object_get(root, "result");
    buff = json_object_array_get_idx(buff, 0);
    if (buff) {
      last_update_id = json_object_get_uint64(json_object_object_get(buff, "update_id")) + 1;
      buff = json_object_object_get(buff, "message");
      if (buff) {
        buff2 = json_object_object_get(buff, "text");
        if (buff2) {
          textbuff = json_object_get_string(buff2);
          if (*textbuff == '/') {
            textbuff += 1;
            getcommandname(&mbuff, textbuff);

            for (eviter = &__start_tgevents; eviter < &__stop_tgevents; eviter++) {
              switch (eviter->type) {
                case TGCB_COMMAND:
                  if (strcmp(eviter->name, mbuff.cstr) != 0)
                    break;
                  eviter->func(buff);
                  break;
              }
            } // for
          } // startsWith '/'
        } // message has text
      } // event has message
    } // result[] not empty

    json_object_put(root);
  }

  return(0);
}
