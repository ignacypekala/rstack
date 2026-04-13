CC = gcc
CFLAGS = -Wall -Wextra -Wno-implicit-fallthrough -std=gnu23 -fPIC
LDFLAGS = -shared -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc \
-Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup

CFLAGS += -g # REMOVE BEFORE SUBMISSION
LDFLAGS += -g # REMOVE BEFORE SUBMISSION
# CFLAGS += -O2 # UNCOMMENT BEFORE SUBMISSION

.PHONY: clean all
.PRECIOUS: test_%.o
all: librstack.so

librstack.so: rstack.o memory_tests.o rstack_container.o
	$(CC) $^ -o $@ $(LDFLAGS) 

rstack_%: rstack_%.o librstack.so
	$(CC) $^ -o $@ -L . -l rstack -Wl,-rpath,'.'

test_%.o: ./tests/%.c ./tests/macros.h
	$(CC) -c $< -o $@ $(CFLAGS)

test_%: test_%.o librstack.so
	$(CC) $^ -o $@ -L . -l rstack -Wl,-rpath,'.'

%.o: %.c %.h
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f *.o a.out rstack_example librstack.so test_* test.fout test.stdout \
	test.valgrind
