CC      = gcc
CFLAGS  = -Wall -Wextra 
DEPFLAGS = -MMD -MP
SRCS    = $(wildcard *.c)
OBJS    = $(patsubst %.c,%.o,$(SRCS))
DEPS    = $(OBJS:.o=.d)
TARGET  = chcmpss

.PHONY: all debug clean distclean 

all: CFLAGS += -O2
all: $(TARGET)

debug: CFLAGS += -g -D_DEBUG -O0
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