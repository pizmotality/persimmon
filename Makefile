CC = gcc
CFLAGS += -Wall
LIBS = -lxcb -pthread

BIN = persimmon

BUILD_DIR = ./build

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))
DEPS = $(patsubst %.c,$(BUILD_DIR)/%.d,$(SRCS))

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@

$(BUILD_DIR)/%.o: %.c | mkdir
	$(CC) $(CFLAGS) -MMD -c $< -o $@

mkdir:
	@mkdir -p $(BUILD_DIR)

.PHONY: clean mkdir

clean:
	$(RM) $(OBJS) $(DEPS) $(BIN)

-include $(DEPS)
