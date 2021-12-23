CC = gcc

CFLAGS += -Wall -Werror -Wextra -pedantic --std=c11 -I$(INC)

INC	  = src/include
SRC  := src/main.c src/lexer.c
OBJS := billion
all: $(OBJS)

$(OBJS): $(SRC)
	$(CC) $(CFLAGS) $? -o $@
