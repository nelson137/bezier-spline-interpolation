EXE    := a.out
SRCS   := $(wildcard *.c)
BUILD  := build
OBJS   := $(SRCS:%.c=$(BUILD)/%.o)

CFLAGS := -Wall -Werror $(shell pkg-config --cflags gtk+-3.0)
LIBS   := -lm $(shell pkg-config --libs gtk+-3.0)

GCC    := /usr/bin/gcc
RM     := /bin/rm -rf
MKDIR  := /bin/mkdir -p

build/$(EXE): $(OBJS)
	@echo "$^ -> $@"
	@$(GCC) -o $@ $(OBJS) $(LIBS)

$(BUILD)/%.o: %.c | $(BUILD)
	@echo "$^ -> $@"
	@$(GCC) -c -o $@ $(CFLAGS) $<

$(BUILD):
	@$(MKDIR) $@

clean:
	$(RM) $(BUILD)
.PHONY: clean
