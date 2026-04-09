#include "macros.h"
#include <stdint.h>
#include <stdio.h>
#include "../rstack.h"

int main() {
    rstack_t *stack = rstack_new();
    uint64_t values[5] = {1, INT64_MAX, INT64_MIN, 2, 3};
    for (int i = 0; i < 5; i++) {
        CHECK_IF_NO_ERROR(rstack_push_value(stack, values[i]));
    }
    for (int i = 0; i < 5; i++) {
        result_t front = rstack_front(stack);
        ASSERT_RESULT(front, true, values[5 - i - 1]);
        rstack_pop(stack);
    }
    ASSERT_RESULT(rstack_front(stack), false, 0);
    rstack_delete(stack);
    return 0;
}
