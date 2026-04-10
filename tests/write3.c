#include "macros.h"
#include "../rstack.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

int main() {
    rstack_t *stack = rstack_new();
    ASSERT(stack);
    NO_ERROR(rstack_push_value(stack, 1));
    NO_ERROR(rstack_push_value(stack, 2));
    NO_ERROR(rstack_push_value(stack, 3));
    rstack_write(OUTPUT_FILE, stack);
    rstack_delete(stack);
}
