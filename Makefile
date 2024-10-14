CC = gcc
CFLAGS = -Wall -g -Iinclude -I/c/msys64/mingw64/include/SDL2      # Include the header files from the include directory and SDL2
LDFLAGS = -L/c/msys64/mingw64/lib -lmingw32 -lSDL2main -lSDL2     # Linker flags for SDL2
SRCS_DIR = src
OBJS_DIR = obj
HEADERS_DIR = include
TARGET = chess

# List of source files
SRCS = $(wildcard $(SRCS_DIR)/*.c)
# Replace .c with .o and prepend the object directory
OBJS = $(patsubst $(SRCS_DIR)/%.c,$(OBJS_DIR)/%.o,$(SRCS))

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)  # Link with SDL2 libraries

# Rule for building object files
$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJS) $(TARGET)

# Ensure the obj directory exists
$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

# Ensure the headers directory exists
$(HEADERS_DIR):
	mkdir -p $(HEADERS_DIR)

# Add this dependency
$(TARGET): $(OBJS_DIR) $(HEADERS_DIR) $(OBJS)
