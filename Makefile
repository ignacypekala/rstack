CC = gcc
CFLAGS = -Wall -Wextra -Wno-implicit-fallthrough -std=gnu23 -fPIC
# CFLAGS += -O2
LDFLAGS = -shared -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc \
-Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup

.PHONY: clean all test
all: rstack_example

librstack.so: rstack.o memory_tests.o rstack_container.o
	$(CC) $^ -o $@ $(LDFLAGS) 

%.o: %.c %.h
	$(CC) -c $< -o $@ $(CFLAGS)
 
rstack_example: rstack_example.o librstack.so
	$(CC) -L . -l rstack $^ -o $@ -Wl,-rpath,'.'

test-%: CFLAGS +=-g
test-%: LDFLAGS +=-g
test-%: rstack_example
	valgrind --track-origins=yes --leak-check=full ./$< $*

test: CFLAGS +=-g
test: LDFLAGS +=-g
test: test.c librstack.so
	$(CC) $(CFLAGS) -c $< -o $@.o
	$(CC) -L . -l rstack $@.o librstack.so
	valgrind --track-origins=yes --leak-check=full ./a.out


clean:
	rm -f *.o a.out rstack_example librstack.so
