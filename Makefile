CC = gcc

DEFS   += -D_POSIX_C_SOURCE=200809L
CFLAGS += -Wall -Werror -Wextra -pedantic --std=c11 -I$(INC) $(DEFS)

INC	  = src/include
SRC  := $(wildcard src/*.c)
OBJS := billion
all: $(OBJS)

$(OBJS): $(SRC)
	$(CC) $(CFLAGS) $? -o $@
