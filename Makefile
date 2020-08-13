CC=gcc
WARN=-Wall -Wextra -Wpedantic
OPTIM=-O3
DEBUG=-g
MAX_MSG_LEN=256
TABLE_WIDTH=6
TABLE_HEIGHT=6
CFLAGS=$(DEBUG) $(OPTIM) $(WARN) -DMAX_MSG_LEN=$(MAX_MSG_LEN) -DTABLE_WIDTH=$(TABLE_WIDTH) -DTABLE_HEIGHT=$(TABLE_HEIGHT) -o $@
.PHONY : all clean

all : playfair-encrypt playfair-decrypt

playfair-encrypt : playfair.c
	$(CC) $(CFLAGS) -DENCRYPT $^

playfair-decrypt : playfair.c
	$(CC) $(CFLAGS) -DDECRYPT $^

clean :
	rm -f playfair-encrypt playfair-decrypt
