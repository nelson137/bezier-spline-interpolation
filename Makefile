SRCS   := $(wildcard *.c)
BUILD  := build
OBJS   := $(SRCS:%.c=$(BUILD)/%.o)
EXE    := a.out

CFLAGS := -Wall -Werror $(shell pkg-config --cflags gtk+-3.0)
LIBS   := -lm $(shell pkg-config --libs gtk+-3.0)

GCC    := /usr/bin/gcc
RM     := /bin/rm -rf
MKDIR  := /bin/mkdir -p

$(EXE): $(OBJS)
	@echo "$^ -> $@"
	@$(GCC) $(OBJS) $(LIBS)
.PHONY: main

$(BUILD)/%.o: %.c | $(BUILD)
	@echo "$^ -> $@"
	@$(GCC) -c $(CFLAGS) $< -o $@

$(BUILD):
	@$(MKDIR) $@

clean:
	$(RM) $(BUILD) a.out
.PHONY: clean
