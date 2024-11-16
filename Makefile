ifeq ($(OS),Windows_NT)
    CC = gcc
    CFLAGS = -Wall -g -Iinclude -I/c/msys64/mingw64/include/SDL2
    LDFLAGS = -L/c/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lws2_32
    SRCS_DIR = src
    OBJS_DIR = obj
    HEADERS_DIR = include
    TARGET = chess

    SRCS = $(wildcard $(SRCS_DIR)/*.c) $(wildcard src/client/linux_win.c)

    OBJS = $(patsubst $(SRCS_DIR)/%.c,$(OBJS_DIR)/%.o,$(SRCS))

    $(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

    $(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	@echo "Compiling $< into $@"  # Debug output
	$(CC) $(CFLAGS) -c $< -o $@
	gcc -Wall -g -o src/server/server.exe src/server/server_win.c -lws2_32 -O3
	gcc -Wall -g -o src/client/client.exe src/client/client_win.c -lw2_32 -O3

    clean:
	rm -f $(OBJS) $(TARGET)

    $(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

    $(HEADERS_DIR):
	mkdir -p $(HEADERS_DIR)

else
    
    CC = gcc
    CFLAGS = -Wall -g -Iinclude $(shell sdl2-config --cflags)
    LDFLAGS = $(shell sdl2-config --libs) -lSDL2_image -lc
    SRCS_DIR = src
    OBJS_DIR = obj
    HEADERS_DIR = include
    TARGET = chess

    SRCS = $(wildcard $(SRCS_DIR)/*.c) $(wildcard src/client/linux_linux.c)

    #SRCS = $(wildcard $(SRCS_DIR)/*.c) src/client/client_linux.c src/server/server_linux.c
    OBJS = $(patsubst $(SRCS_DIR)/%.c,$(OBJS_DIR)/%.o,$(SRCS)) #src/client/client_linux.c src/server/server_linux.c 


    $(TARGET): $(OBJS) 
	@echo "Linux selected"  # Debug output to show object file being compiled
	@echo "Linking $(TARGET)..."  # Debug output to confirm linking
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

    $(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	@echo "Compiling $< into $@"  # Debug output to show object file being compiled
	$(CC) $(CFLAGS) -c $< -o $@
	gcc src/server/server_linux.c -o src/server/server_linux.elf -lc -O3
	#gcc  -o src/client/client.elf src/client/client_linux.c -lc -O3

    clean:
	rm -f $(OBJS) $(TARGET)
endif
