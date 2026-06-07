# Path to the rstack library repository
RSTACK_DIR ?= ..

CC = gcc
CFLAGS = -Wall -Wextra -Wno-implicit-fallthrough -std=gnu23
CFLAGS += -O2
CFLAGS += -I.

LDFLAGS = -L$(RSTACK_DIR) -lrstack -Wl,-rpath,$(RSTACK_DIR)

.PHONY: clean all build-lib
.PRECIOUS: test_%.o

all: build-lib

build-lib:
	$(MAKE) -C $(RSTACK_DIR) librstack.so

test_%_executable: test_%.o | build-lib
	$(CC) $< -o $@ $(LDFLAGS)

test_%.o: ./tests_$(TEST_BATCH)/%.c macros.h
	$(CC) -I tests -c $< -o $@ $(CFLAGS)

%.o: %.c 
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f test_*.o test_*_executable test.fout test_*.fout test.stdout \ 
		test.valgrind test.diff
