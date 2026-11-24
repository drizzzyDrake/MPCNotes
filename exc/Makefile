CC = gcc
CFLAGS = -Wall -Wextra -O2

SRCS = $(wildcard *.c)
TARGETS = $(SRCS:.c=)

.PHONY: all clean

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)
