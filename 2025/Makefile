CC = cc
CSTANDARD = -std=c99 -fsanitize=address,undefined
CWARNINGS = -Wall -Wextra -Wshadow -pedantic
ARCH = native
COPTIMIZE = -O2 -march=$(ARCH) -flto
CFLAGS = $(CSTANDARD) $(CWARNINGS) $(COPTIMIZE)
LDFLAGS = $(CFLAGS)

%: %.c
	$(CC) $(LDFLAGS) $< -o $@
	./$@
