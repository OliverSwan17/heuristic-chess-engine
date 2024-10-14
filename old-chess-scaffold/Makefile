CC = gcc
CFLAGS = -g -fPIC
LIBS = -lSDL2 -lSDL2_image
SRCS = chess.c fen.c draw.c logic.c game.c
OBJS = $(SRCS:.c=.o)
TARGET = chess

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
