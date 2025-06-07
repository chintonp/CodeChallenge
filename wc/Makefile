CC      = gcc
CFLAGS  = -Wall -Wextra -O2
DEPFLAGS = -MMD -MP
SRCS    = $(wildcard *.c)
OBJS    = $(patsubst %.c,%.o,$(SRCS))
DEPS    = $(OBJS:.o=.d)
TARGET  = chwc

.PHONY: all debug clean distclean

all: $(TARGET)

debug: CFLAGS += -g
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c 
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@


clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

distclean: clean
	rm -f $(DEPS)

-include $(DEPS)