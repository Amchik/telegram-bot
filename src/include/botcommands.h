/* vim: ft=c tabstop=2 shiftwidth=2
 * 
 * Defines base bot commands utils.
 */
#ifndef __ASltCd_botcommands_h
#define __ASltCd_botcommands_h

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
#define TelegramEvent _TgEvent struct TelegramCallbackFunction

/*
 * Macro for creating new TelegramEvent declaration
 */
#define TelegramEvent$new(_name, _type, _fn) {.name = _name, .type = _type, .func = _fn}

/*
 * Macro for creating new bot command value.
 */
#define TelegramEvent$command(_name, _fn) TelegramEvent$new(_name, TGCB_COMMAND, _fn)

/*
 * Macro for creating new bot command value.
 */
#define TelegramEvent$text(_name, _fn) TelegramEvent$new(_name, TGCB_TEXT, _fn)

/* FUTURE */

/*
 * (Future)
 * Declarate telegram event.
 * TELEGRAM_EVENT(unique id, "name", TGCB_TYPE, on_trigger)
 */
#define TELEGRAM_EVENT(_id, _name, _type, _fn) \
  __attribute__((section(tgevents))) \
  struct TelegramCallbackFunction _id = { .name = _name, .type = _type, .func = _fn }

#endif

