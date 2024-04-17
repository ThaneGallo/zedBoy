CFLAGS=-g -I./snake
TARGET=zedboy
SOURCE=main.c snake/snake.c
ESL_ZEDBOARD_IP ?= qemu
BIN_DEST=/home/root
DEBUG_PORT=1234
OBJS=$(SOURCE:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kill:
	@ssh -q root@$(ESL_ZEDBOARD_IP) pkill -9 -f $(TARGET) || true

upload: $(TARGET)
	@scp -q $^ root@$(ESL_ZEDBOARD_IP):$(BIN_DEST)

run: upload
	@ssh -q root@$(ESL_ZEDBOARD_IP) $(BIN_DEST)/$(TARGET)

clean:
	rm -rf $(OBJS) $(TARGET)

debug-remote: upload
	@ssh -q -f root@$(ESL_ZEDBOARD_IP) "bash -c 'nohup gdbserver --once :$(DEBUG_PORT) $(BIN_DEST)/$(TARGET) &> /dev/null < /dev/null &'"

debug: debug-remote
	$(GDB) -iex "target remote $(ESL_ZEDBOARD_IP):$(DEBUG_PORT)" $(TARGET)

.PHONY: all clean upload run kill debug-remote debug
