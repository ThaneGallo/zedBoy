CFLAGS = -Wall -Wextra -pedantic
SRCS = draw.c tetris.c
OBJS = $(SRCS:.c=.o)
TARGET = tetris

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(TARGET)

.PHONY: clean
