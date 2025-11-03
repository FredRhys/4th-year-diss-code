CC = gcc
LD = $(CC) -fsanitize=undefined -fsanitize=address
CFLAGS = -std=c23\
		 -g\
		 -Wall\
		 -Werror\
		 -pedantic\
		 -fsanitize=undefined\
		 -fsanitize=address\
		 -Ofast

.PHONY: basic
basic:
	$(LD) $(CFLAGS) -o basic basic.c -lm

.PHONY: tonellishanks
tonellishanks:
	$(LD) $(CFLAGS) -o tonellishanks tonellishanks.c