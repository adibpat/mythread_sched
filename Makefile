LIBB = mythread.a

CC = gcc

SRC = myqueue.c mysched.c

TEST_SRC = mytest.c

TEST_OBJ = $(TEST_SRC:.c=.o)

SRC_OBJ = $(SRC:.c=.o)

INCLUDE = myatomic.h mythread.h mymutex.h myqueue.h futex.h futex_inline.h mycond.h  

CFLAGS = -g -lm

.PHONY: all lib clean test

all: lib test

lib: $(LIBB)

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.~ test main

test: $(TEST_OBJ) $(SRC_OBJ) $(INCLUDE) lib
	$(CC) -o test $(CFLAGS) $(TEST_OBJ) $(SRC_OBJ) $(LIBB)