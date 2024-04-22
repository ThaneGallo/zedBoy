CFLAGS = -Wall -Wextra -pedantic
SRCS = draw.c pong.c
OBJS = $(SRCS:.c=.o)
TARGET = pong

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(TARGET)

.PHONY: clean
