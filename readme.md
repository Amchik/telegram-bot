# `telegram-bot`

Простой телеграм бот, написанный на си.

## Сборка и запуск

## Зависимости

Для компиляции лучше всего использовать компилятор `clang`.
Так же работает `gcc`.

Библиотеки:

* [`json-c`](https://github.com/json-c/json-c) для парсинга JSON
* `libcurl` для запросов к API telegram

Стандарт:

* `c99` так как используется моментальное определение значения переменной (констант).
* `c89` если не смущает `warning: designated initializers are a C99 feature`

### Сборка

Рецепты make:

* `info` выводит информацию о среде, в которой будет собираться программа
* `clean` удаляет `obj/` и `bin/` директории
* `all` (по умолчанию) собирает программу

Сборку можно корректировать под определённую среду при
помощи переменной среды `TARGET`. Вот её возможные значения:

* `RELEASE` (по умолчанию) сборка с оптимизацией (`-O3`) и под текущую архитектуру (`-march=native`)
* `CLANG` вместе с обычными оптимизациями из `RELEASE` использует ThinLTO (доступно только для clang)
* `DEBUG` сборка без оптимизаций с debug символами (`-g`)

Так же можно определись свой `TARGET`:

```console
$ export _mytarget_CFLAGS="-O3" \
         _mytarget_CFLAGS_DEFINES="-D_DEFAULT_SOURCE" \
         _mytarget_LDFLAGS="-lncursesw" \
         _mytarget_CC="clang" \
         _mytarget_STD="gnu99"
$ TARGET=mytarget make info
telegram-bot: bin: bin, objects: obj, sources: src
Compiling for target mytarget
 Target special CFLAGS:  -O3
 Target special LDFLAGS: -lncursesw
 Target special defines: -D_DEFAULT_SOURCE
Using C standard gnu99 via clang
Default log level: -Wall -Wextra -pedantic -Wpedantic
Default defines:   -Isrc/ -D _DEFAULT_SOURCE
Threads policy:
  * note: for use threads set USE_THREADS=1
Full CFLAGS:  -std=gnu99 -Wall -Wextra -pedantic -Wpedantic -Isrc/ -D _DEFAULT_SOURCE  -O3 -D_DEFAULT_SOURCE
Full LDFLAGS: -lcurl -ljson-c  -lncursesw
```

После сборки будет получен файл `bin/telegram-bot`.

### Запуск

```console
$ echo -n "12345:telegram_token" > .telegram_token
$ bin/telegram-bot
[OK] Found token for user 12345 ends for …oken
[LOGGED IN] As @examplebot
```

Переменные среды:

* `TOKEN` токен бота. Будет иметь приоритет выше, чем `./.telegram_token`

## Самая краткая документация

Смотрите `src/include/%s.h`.

### Добавление новых команд

Команды разделены по модулям, находящимся в `src/bot.modules/module.c`. Модули
не требуют заголовочных файлов.

Пример модуля:

```c
#include <json-c/json.h>

#include "../include/botcommands.h"
#include "../include/telegram.h"

/* сигнатура функции на любое событие должна выглядеть
   как void (json_object*) */
void
cmdimpl_start(json_object *event) {
	const char* chat_id;

	/* chat_id получается через json-c */
	chat_id = json_object_get_string(
			/* ниже функция просто заменяет аналогичные json_object_object_get(...) */
			tg_json_getpathobject(event, "chat", "id", 0)
			);
	tg_request("sendMessage", 
			topt_new("chat_id", chat_id,
				topt_new("text", "Мой автор дегенерат! https://github.com/Amchik", 0)));
}

/* регистрируем команду */
TelegramEvent cmdstart = TelegramEvent$command("start", cmdimpl_start);

/* или так */
TelegramEvent eventcmdstart = TelegramEvent$new("start", TGCB_COMMAND, cmdimpl_start);
```

