CC=gcc
CFLAGS=-Wall -Wextra -Werror -std=gnu99
BUILD_DIR=./build

SERVER_SRC = server.c
SERVER_OBJ = $(SERVER_SRC:%.c=$(BUILD_DIR)/%.o)
SERVER_DEP = $(SERVER_OBJ:%.o=%.d)

CLIENT_SRC = client.c
CLIENT_OBJ = $(CLIENT_SRC:%.c=$(BUILD_DIR)/%.o)
CLIENT_DEP = $(CLIENT_OBJ:%.o=%.d)

#         | Input from system           | Makefile variables        | Preprocessor #defines
#         | $OS           uname         | KERNEL      ENV           |  MINGW   CYGWIN  LINUX    OSX
# ========|=============================|===========================|==============================
# mingw   | Windows_NT    MINGW32_NT-*  | Windows     MINGW32_NT    |  1
# cygwin  | Windows_NT    CYGWIN_NT-*   | Windows     CYGWIN_NT     |          1
# WSL     | (undefined)   Linux         | Linux       Linux         |                  1
# Mac     | (undefined)   Darwin        | Darwin      Darwin        |                           1

UNAME := $(shell uname | grep -oE "MINGW32_NT|CYGWIN_NT|Linux|Darwin")

ifeq ($(OS),Windows_NT)
	KERNEL := Windows
else
	KERNEL := $(UNAME)
endif
ENV := $(UNAME)

SERVER_TARGET := server
CLIENT_TARGET := client

ifeq ($(KERNEL),Windows)
	ifeq ($(ENV), CYGWIN_NT)
		CPPFLAGS += -D CYGWIN
	endif
	ifeq ($(ENV), MINGW32_NT)
		CPPFLAGS += -D MINGW -D _WIN32_WINNT=_WIN32_WINNT_WIN7
		LDLIBS += -lws2_32
	endif
	SERVER_TARGET := $(SERVER_TARGET).exe
	CLIENT_TARGET := $(CLIENT_TARGET).exe
endif
ifeq ($(KERNEL),Linux)
	CPPFLAGS += -D LINUX
endif
ifeq ($(KERNEL),Darwin)
	CPPFLAGS += -D OSX
endif

.PHONY: all client server clean

all: client server

server: $(BUILD_DIR)/$(SERVER_TARGET)

client: $(BUILD_DIR)/$(CLIENT_TARGET)

$(BUILD_DIR)/$(SERVER_TARGET): $(SERVER_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)
	ln -sf $@

$(BUILD_DIR)/$(CLIENT_TARGET): $(CLIENT_OBJ)
	mkdir -p $(BUILD_DIR)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)
	ln -sf $@

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -c $< -o $@

-include $(DEP)

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(SERVER_TARGET)
	rm -f $(CLIENT_TARGET)
