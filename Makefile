CC = gcc
CFLAGS = -Wall -g -Iinclude -I/c/msys64/mingw64/include/SDL2
LDFLAGS = -L/c/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image
SRCS_DIR = src
OBJS_DIR = obj
HEADERS_DIR = include
TARGET = chess

SRCS = $(wildcard $(SRCS_DIR)/*.c)
OBJS = $(patsubst $(SRCS_DIR)/%.c,$(OBJS_DIR)/%.o,$(SRCS))

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)  # Link with SDL2 libraries

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

$(HEADERS_DIR):
	mkdir -p $(HEADERS_DIR)

$(TARGET): $(OBJS_DIR) $(HEADERS_DIR) $(OBJS)
