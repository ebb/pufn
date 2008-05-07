# public domain

SRCS := \
	block.c \
	boolean.c \
	continuation.c \
	dictionary.c \
	fail.c \
	fixnum.c \
	list.c \
	machine.c \
	main.c \
	object.c \
	parse.c \
	prelude.c \
	primitive.c \
	print.c \
	string.c \
	word.c \
	wrapper.c \

OBJS := ${SRCS:.c=.o}

CC := gcc
CFLAGS := -Wall -g
LD := gcc

.SUFFIXES :
.SUFFIXES : .c .o

pufn: $(OBJS)
	$(LD) -o $@ $(OBJS)

.c.o :
	$(CC) $(CFLAGS) -c $<

.PHONY : clean

clean :
	$(RM) -rf $(OBJS) pufn *~
