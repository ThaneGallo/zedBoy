CFLAGS = -Wall -Wextra -pedantic
SRCS = ../../utils/draw.c ../../utils/gameConstants.c flappy.c
OBJS = $(SRCS:.c=.o)
TARGET = flappy

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(TARGET)

.PHONY: clean
