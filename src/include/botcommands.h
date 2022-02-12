/* vim: ft=c tabstop=2 shiftwidth=2
 * 
 * Defines base bot commands utils.
 */
#ifndef __ASltCd_botcommands_h
#define __ASltCd_botcommands_h

#include <json-c/json.h>

enum TelegramCallback {
  TGCB_COMMAND
};

/*
 * Represents a command function declaration in 'botcommands' section.
 */
typedef struct {
  char name[255];
  enum TelegramCallback type;
  void (*func) (json_object *event);
} tgcbfn_t;

/*
 * Macros for section name.
 */
#define tgevents "tgevents"

/*
 * Attribute for bot commands.
 */
#define _TgEvent __attribute__((section(tgevents)))

/*
 * Type for bot commands.
 */
#define TelegramEvent _TgEvent tgcbfn_t

/*
 * Macro for creating new TelegramEvent declaration
 */
#define TelegramEvent$new(_name, _type, _fn) {.name = _name, .type = _type, .func = _fn}

/*
 * Macro for creating new bot command value.
 */
#define TelegramEvent$command(_name, _fn) TelegramEvent$new(_name, TGCB_COMMAND, _fn)

#endif

