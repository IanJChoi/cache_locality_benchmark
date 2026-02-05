CC=clang
CFLAGS=-O3 -std=c11 -Wall -Wextra -Wpedantic

TARGET=cache_lab
SRCS=main.c spatial.c temporal.c
OBJS=$(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c tools.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
