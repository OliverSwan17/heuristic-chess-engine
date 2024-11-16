ifeq ($(OS),Windows_NT)
    CC = gcc
    CFLAGS = -Wall -g -Iinclude -I/c/msys64/mingw64/include/SDL2
    LDFLAGS = -L/c/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lws2_32
    SRCS_DIR = src
    OBJS_DIR = obj
    HEADERS_DIR = include
    TARGET = chess

    SRCS = $(wildcard $(SRCS_DIR)/*.c)
    $(info SRCS = $(SRCS))  # Debug output

    OBJS = $(patsubst $(SRCS_DIR)/%.c,$(OBJS_DIR)/%.o,$(SRCS))
    $(info OBJS = $(OBJS))  # Debug output

    $(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

    $(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	@echo "Compiling $< into $@"  # Debug output
	$(CC) $(CFLAGS) -c $< -o $@

    clean:
	rm -f $(OBJS) $(TARGET)

    $(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

    $(HEADERS_DIR):
	mkdir -p $(HEADERS_DIR)

else
    CC = gcc
    CFLAGS = -Wall -g -Iinclude $(shell sdl2-config --cflags)
    LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image 
    SRCS_DIR = src
    OBJS_DIR = obj
    HEADERS_DIR = include
    TARGET = chess

    SRCS = $(wildcard $(SRCS_DIR)/*.c)
    OBJS = $(patsubst $(SRCS_DIR)/%.c,$(OBJS_DIR)/%.o,$(SRCS))

    $(info $(TARGET))
    $(info $(OBJS))

    $(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."  # Debug output to confirm linking
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

    $(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	@echo "Compiling $< into $@"  # Debug output to show object file being compiled
	$(CC) $(CFLAGS) -c $< -o $@

    clean:
	rm -f $(OBJS) $(TARGET)
endif
