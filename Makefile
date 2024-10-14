CC = gcc
SRCS = main.c fen.c
OBJS = $(SRCS:.c=.o)
TARGET = chess

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)