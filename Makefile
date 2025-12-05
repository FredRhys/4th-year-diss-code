CC = gcc
LD = $(CC) -fsanitize=undefined -fsanitize=address
CFLAGS = -std=c23\
		 -g\
		 -Wall\
		 -Werror\
		 -Wextra\
		 -pedantic\
		 -fsanitize=undefined\
		 -fsanitize=address\
		 -O2

.PHONY: basic
basic: basic.c factor64.o
	$(LD) $(CFLAGS) -o basic basic.c factor64.o -lm

.PHONY: tonellishanks
tonellishanks:
	$(LD) $(CFLAGS) -o tonellishanks tonellishanks.c

.PHONY: montgomery
montgomery:
	$(LD) $(CFLAGS) -o montgomery montgomery.c

.PHONY: mod
mod:
	$(LD) $(CFLAGS) -o mod mod.c