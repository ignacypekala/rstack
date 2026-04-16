CC = gcc
LDFLAGS = -shared -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc \
-Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup
CFLAGS = -Wall -Wextra -Wno-implicit-fallthrough -std=gnu23 -fPIC
CFLAGS += -O3

.PHONY: clean all
.PRECIOUS: test_%.o # REMOVE BEFORE SUBMISSION
all: librstack.so

librstack.so: rstack.o memory_tests.o rstack_container.o rstack_delete.o \
rstack_read.o
	$(CC) $^ -o $@ $(LDFLAGS) 

# REMOVE BEFORE SUBMISSION?
rstack_%: rstack_%.o librstack.so
	$(CC) $^ -o $@ -L . -l rstack -Wl,-rpath,'.'
# =========================

# REMOVE BEFORE SUBMISSION
TEST_BATCH ?= test
test_%.o: ./tests_$(TEST_BATCH)/%.c macros.h
	$(CC) -I tests -c $< -o $@ $(CFLAGS)
test_%_executable: test_%.o librstack.so
	$(CC) $^ -o $@ -L . -lrstack
# ========================

%.o: %.c %.h
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	#                    ||
	#                    \/
	rm -f *.o a.out rstack_example librstack.so 
	# REMOVE BEFORE SUBMISSION
	rm -f test_*.fout test_*_executable test_*.o test.fout  test.diff \
	test.stdout test.valgrind test.make
	# ========================
