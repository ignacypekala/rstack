CC=gcc
CGFLAGS=-std=gnu23
# CFLAGS=-Wall -Wextra -Wno-implicit-fallthrough -std=gnu23 -fPIC
CLFAGS += -O2
LDFLAGS=-shared -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc \
-Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup


.PHONY: clean all
all: librstack.so

librstack.so: memory_tests.o rstack.o array.o
	$(CC) $^ -o $@ $(LDFLAGS) 

%.o: %.c %.h
	$(CC) -c $< -o $@ $(CFLAGS)
 
rstack_example: rstack_example.o memory_tests.o librstack.so
	$(CC) -L . -l rstack $^ -o $@ -Wl,-rpath,'.'

test: CFLAGS +=-g
test: LDFLAGS +=-g
test: rstack_example
	valgrind --leak-check=full ./$< -s
	

clean:
	rm -f *.o rstack_example librstack.so
