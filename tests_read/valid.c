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
    ASSERT(stack != nullptr);
    while (!rstack_empty(stack)) {
        result_t front = rstack_front(stack);
        printf("%" PRIu64 "\n", front.value);
        rstack_pop(stack);
    }
    rstack_delete(stack);
    return 0;
}
