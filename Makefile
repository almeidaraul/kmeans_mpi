CFLAGS=-Wextra -O3
SRCDIR=src

sequential:
	gcc $(SRCDIR)/sequential.c -o sequential.out
