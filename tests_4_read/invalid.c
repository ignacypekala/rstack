#include "../macros.h"
#include "../rstack.h"
#include <assert.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(
            stderr,
            "Too few arguments. Correct usage: ./test_read1 PATH\n"
        );
        exit(1);
    }
    rstack_t *stack = rstack_read(argv[1]);
    ASSERT(stack == nullptr);
    rstack_delete(stack);
    return 0;
}
