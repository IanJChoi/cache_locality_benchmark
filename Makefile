CC = cc
CFLAGS = -O2 -Wall -Wextra -std=c11

.PHONY: all clean

all: method_a method_b

method_a: method_a.c
	$(CC) $(CFLAGS) method_a.c -o method_a

method_b: method_b.c
	$(CC) $(CFLAGS) method_b.c -o method_b

clean:
	rm -f method_a method_b
