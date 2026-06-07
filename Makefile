CC = gcc
LDFLAGS = -shared -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc \
-Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup
CFLAGS = -Wall -Wextra -Wno-implicit-fallthrough -std=gnu23 -fPIC
CFLAGS += -O2
CFLAGS += -I./provided

.PHONY: clean all
.PRECIOUS: test_%.o
all: librstack.so

LIB_SRCS = src/rstack.c src/rstack_container.c src/rstack_delete.c \
	src/rstack_read.c provided/memory_tests.c
LIB_OBJS = $(LIB_SRCS:.c=.o)

librstack.so: $(LIB_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) 

rstack_%: rstack_%.o librstack.so
	$(CC) $^ -o $@ -L . -l rstack -Wl,-rpath,'.'

%.o: %.c 
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f src/*.o provided/*.o rstack_example librstack.so 
