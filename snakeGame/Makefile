TARGET:= snake.o
SRCS:=snake.c draw.c  # Added displayImg.c
OBJS:=$(SRCS:.c=.o)
CFLAGS:=$(foreach incdir, $(INCLUDE_DIRS), -I$(incdir)) -g -O2

# default rule to build target
all: $(TARGET) draw.o  # Updated to depend on displayImg.o

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LZED)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

draw.o: draw.c  # Rule to compile displayImg.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(OBJS) $(TARGET)
