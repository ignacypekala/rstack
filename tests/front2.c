#include "macros.h"
#include <stdint.h>
#include <assert.h>
#include "../rstack.h"

int main() {
    rstack_t *A = rstack_new();
    ASSERT(A);
    rstack_t *B = rstack_new();
    ASSERT(B);

    NO_ERROR(rstack_push_value(A, 1));
    NO_ERROR(rstack_push_value(B, 2));
    NO_ERROR(rstack_push_rstack(A, B));
    NO_ERROR(rstack_push_rstack(B, A));

    ASSERT_RESULT(rstack_front(A), true, 2);
    rstack_delete(A);
    rstack_delete(B);
    return 0;
}
