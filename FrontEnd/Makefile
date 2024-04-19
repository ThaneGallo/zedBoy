TARGET := zed_gaming_menu
SRCS := zed_gaming_menu.c displayImg.c 
OBJS := $(SRCS:.c=.o)
CFLAGS := $(foreach incdir,$(INCLUDE_DIRS),-I$(incdir)) -g -O2
LZED := -lzed

# Default rule to build target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LZED)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(OBJS) $(TARGET)
