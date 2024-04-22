TARGET:=zed_gaming_menu
SRCS:=zed_gaming_menu.c displayImg.c  # Added displayImg.c
OBJS:=$(SRCS:.c=.o)
INCLUDE_DIRS:=.. $OECORE_TARGET_SYSROOT/usr/include/libzed  # Add the libzed include directory here
CFLAGS:=$(foreach incdir, $(INCLUDE_DIRS), -I$(incdir)) -g -O2
LZED:=-lzed

# default rule to build target
all: $(TARGET) displayImg.o  # Updated to depend on displayImg.o

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LZED)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

displayImg.o: displayImg.c  # Rule to compile displayImg.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(OBJS) $(TARGET)
