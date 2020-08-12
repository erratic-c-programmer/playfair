CC=gcc
WARN=-Wall -Wextra -Wpedantic
OPTIM=-O3
DEBUG=-g
CFLAGS=$(DEBUG) $(OPTIM) $(WARN) -o $@
MAX_MSG_LEN=256
.PHONY : all clean

all : playfair-encrypt playfair-decrypt

playfair-encrypt : playfair.c
	$(CC) $(CFLAGS) -DMAX_MSG_LEN=$(MAX_MSG_LEN) -DENCRYPT $^

playfair-decrypt : playfair.c
	$(CC) $(CFLAGS) -DMAX_MSG_LEN=$(MAX_MSG_LEN) -DDECRYPT $^

clean :
	rm -f playfair-encrypt playfair-decrypt
