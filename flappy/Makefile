CFLAGS = -Wall -Wextra -pedantic
SRCS = draw.c flappy.c
OBJS = $(SRCS:.c=.o)
TARGET = flappy

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(TARGET)

.PHONY: clean