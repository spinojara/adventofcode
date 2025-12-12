MAKEFLAGS += --always-make

CC = cc
CSTANDARD = -std=c99
CWARNINGS = -Wall -Wextra -Wshadow -pedantic
ARCH = native
COPTIMIZE = -O2 -march=$(ARCH) -flto
CFLAGS = $(CSTANDARD) $(CWARNINGS) $(COPTIMIZE)
LDFLAGS = $(CFLAGS)
LDLIBS = -lm

%: %.c
	$(CC) $(LDFLAGS) $< -o $@ $(LDLIBS)
	./$@
