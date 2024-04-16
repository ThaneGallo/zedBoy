TARGET:=snake
SRCS:=Snake.c
OBJS:=$(SRCS:.c=.o)
INCLUDE_DIRS:=.. $OECORE_TARGET_SYSROOT/usr/include/libzed  # Add the libzed include directory here
CFLAGS:=$(foreach incdir, $(INCLUDE_DIRS), -I$(incdir)) -g -O2
LZED:=-lzed

# default rule to build target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LZED)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(OBJS) $(TARGET)