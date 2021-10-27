# makefile
#
# -PROJECT--------------VER----FEATURES------
# tasks                 v1     
#  -> snus              v2     utests,pthread
#   -> autumn           v3-pre utests,pthread
#    -> tableflip       v3     
#     -> ai4            v4-pre pthread
#      -> telegram-bot  v4     pthread
#
# Current makefile version: v4
#
# _mytarget_CFLAGS=-O3
# _mytarget_CFLAGS_DEFINES=-D_DEFAULT_SOURCE
# _mytarget_LDFLAGS=-lncursesw
# _mytarget_CC=my-cool-clang
# _mytarget_STD=gnu99
# TARGET=mytarget
# -> % make TARGET=CLANG info clean all
# Target list ($TARGET):
#  - RELEASE -- with optimizations (-O3 and native arch)
#  - CLANG   -- with optimizations and ThinLTO
#  - DEBUG   -- without optimizations + debug symbols
# Note: if you change target please run `make clean`

NAME = telegram-bot

BIND=bin
SRCD=src
OBJD=obj

SRC = $(wildcard $(SRCD)/*.c) $(wildcard $(SRCD)/**/*.c)
OBJ = $(patsubst $(SRCD)/%.c,$(OBJD)/%.o,$(SRC))

ifdef TARGET
	TARGET := $(TARGET)
else
	TARGET := RELEASE
endif

_LDFLAGS = -lcurl -ljson-c
_CFLAGS_LOG = -Wall -Wextra -pedantic -Wpedantic
_CFLAGS_DEFINES = -Isrc/ -D _DEFAULT_SOURCE
_STD=c99
ifeq ($(USE_THREADS), 1)
	_CFLAGS_THREADS := -DMULTITHREADING -pthread
	_LDFLAGS_THREADS := -pthread
else
	_CFLAGS_THREADS :=
	_LDFLAGS_THREADS :=
endif

_RELEASE_CFLAGS = -O3 -march=native -pipe
_RELEASE_CFLAGS_DEFINES =
_RELEASE_LDFLAGS = 
_RELEASE_CC = $(CC)
_RELEASE_STD = $(_STD)

_CLANG_CFLAGS = -O3 -flto=thin -march=native -pipe
_CLANG_CFLAGS_DEFINES =
_CLANG_LDFLAGS = -Wl,-O2 -Wl,--as-needed
_CLANG_CC = clang
_CLANG_STD = $(_STD)

_DEBUG_CFLAGS = -pipe -O0 -g
_DEBUG_CFLAGS_DEFINES =
_DEBUG_LDFLAGS = 
_DEBUG_CC = $(CC)
_DEBUG_STD = $(_STD)

__DONE := \e[0;32m[DONE]\e[0m
__UNDONE := ...

$(OBJD)/%.o: src/%.c
	@mkdir -p $(@D)
	@printf "\e[0;32mCompiling\e[0m \e[0;40m$@\e[0m$(__UNDONE)\r"
	@$(_$(TARGET)_CC) -std=$(_$(TARGET)_STD) $(_CFLAGS_LOG) $(_CFLAGS_DEFINES) $(_CFLAGS_THREADS) $(_$(TARGET)_CFLAGS) $(_$(TARGET)_CFLAGS_DEFINES) $(CFLAGS) -c -o $@ $<
	@printf "\e[0;32mCompiling\e[0m \e[0;40m$@\e[0m $(__DONE)\n"

$(BIND)/$(NAME): $(OBJ)
	@mkdir -p $(@D)
	@printf "\e[0;35mLinking\e[0m   \e[0;40m$@\e[0m$(__UNDONE)\r"
	@$(_$(TARGET)_CC) -o $@ $^ $(_LDFLAGS) $(_LDFLAGS_THREADS) $(_$(TARGET)_LDFLAGS) $(LDFLAGS)
	@printf "\e[0;35mLinking\e[0m   \e[0;40m$@\e[0m $(__DONE)\n"

all: $(BIND)/$(NAME)
debug:
	@TARGET=DEBUG make all

clean:
	@printf "\e[0;34mRemoving obj files\e[0m$(__UNDONE)\r"
	@rm -f $(OBJ)
	@printf "\e[0;34mRemoving obj files\e[0m $(__DONE)\n"
	@printf "\e[0;34mRemoving binary files\e[0m$(__UNDONE)\r"
	@rm -f $(BIND)/$(NAME)
	@printf "\e[0;34mRemoving binary files\e[0m $(__DONE)\n"

info:
	@printf "\e[0;37m# \e[1;35m$(NAME)\e[0;37m: bin: \e[0;40m$(BIND)\e[0;37m, objects: \e[0;40m$(OBJD)\e[0;37m, sources: \e[0;40m$(SRCD)\e[0m\n"
	@printf "\e[0;37m# Compiling for target \e[0;32m$(TARGET)\e[0m\n"
	@printf "\e[0;37m#  Target special CFLAGS:  \e[0;40m$(_$(TARGET)_CFLAGS)\e[0m\n"
	@printf "\e[0;37m#  Target special LDFLAGS: \e[0;40m$(_$(TARGET)_LDFLAGS)\e[0m\n"
	@printf "\e[0;37m#  Target special defines: \e[0;40m$(_$(TARGET)_CFLAGS_DEFINES)\e[0m\n"
	@printf "\e[0;37m# Using C standard \e[0;32m$(_$(TARGET)_STD)\e[0;37m via \e[0;32m$(_$(TARGET)_CC)\e[0m\n"
	@printf "\e[0;37m# Default log level: \e[0;40m$(_CFLAGS_LOG)\e[0m\n"
	@printf "\e[0;37m# Default defines:   \e[0;40m$(_CFLAGS_DEFINES)\e[0m\n"
	@printf "\e[0;37m# Threads policy:    \e[0;40m$(_CFLAGS_THREADS)\e[0m\n"
	@printf "\e[0;37m#   * note: for use threads set \e[0;40mUSE_THREADS=1\e[0m\n"
	@printf "\e[0;37m# Full CFLAGS:  \e[0;40m-std=$(_$(TARGET)_STD) $(_CFLAGS_LOG) $(_CFLAGS_DEFINES) $(_CFLAGS_THREADS) $(_$(TARGET)_CFLAGS) $(_$(TARGET)_CFLAGS_DEFINES) $(CFLAGS)\e[0m\n"
	@printf "\e[0;37m# Full LDFLAGS: \e[0;40m$(_LDFLAGS) $(_LDFLAGS_THREADS) $(_$(TARGET)_LDFLAGS) $(LDFLAGS)\e[0m\n"

