CC=gcc
WARN=-Wall -Wextra -Wpedantic
OPTIM=-O3
DEBUG=-g
MAX_MSG_LEN=256
TABLE_WIDTH=6
TABLE_HEIGHT=6
CFLAGS=$(DEBUG) $(OPTIM) $(WARN) -fsigned-char -o $@
.PHONY : all clean

all : playfair

playfair : playfair.c
	$(CC) $(CFLAGS) $^

clean :
	rm -f playfair
