ifeq ($(OS),Windows_NT)
CC = gcc
CFLAGS = -Wall -g -Iinclude -I/c/msys64/mingw64/include/SDL2 -O3
LDFLAGS = -L/c/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lws2_32

SRCS_DIR = src
OBJS_DIR = obj
HEADERS_DIR = include

TARGET = chess

SRCS = $(shell find $(SRCS_DIR) -name '*.c' | grep -v 'src/linux_networking' | grep -v 'src/windows_networking/server.c')
OBJS = $(SRCS:$(SRCS_DIR)/%.c=$(OBJS_DIR)/%.o)

# Explicitly add server.o as it's in a specific directory
SERVER_OBJ = $(OBJS_DIR)/windows_networking/server.o

all: $(TARGET)

# Linking objects
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Rule to compile .c files into .o files
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) server.exe $(SERVER_OBJ)

.PHONY: all clean server


# Specific target to build server.exe
server: $(SERVER_OBJ)
	$(CC) -Wall -g -o server.exe src/windows_networking/server.c -lws2_32

$(SERVER_OBJ): src/windows_networking/server.c
	$(CC) $(CFLAGS) -c $< -o $@
else
CC = gcc
CFLAGS = -Wall -g -Iinclude -I/usr/include/SDL2
LDFLAGS = -L/usr/lib -lSDL2 -lSDL2_image -lpthread -lm

SRCS_DIR = src
OBJS_DIR = obj
HEADERS_DIR = include

TARGET = chess

SRCS = $(shell find $(SRCS_DIR) -name '*.c' | grep -v 'src/windows_networking' | grep -v 'src/linux_networking/server.c')
OBJS = $(SRCS:$(SRCS_DIR)/%.c=$(OBJS_DIR)/%.o)

# Explicitly add server.o as it's in a specific directory
SERVER_OBJ = $(OBJS_DIR)/linux_networking/server.o

all: $(TARGET)

# Linking objects
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Rule to compile .c files into .o files
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) server $(SERVER_OBJ)

.PHONY: all clean server

# Specific target to build server
server: $(SERVER_OBJ)
	$(CC) -Wall -g -o server src/linux_networking/server.c -lpthread

$(SERVER_OBJ): src/linux_networking/server.c
	$(CC) $(CFLAGS) -c $< -o $@

endif
