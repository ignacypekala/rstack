#include "macros.h"
#include <stdint.h>
#include <assert.h>
#include "../rstack.h"

int main() {
    rstack_t *A = rstack_new();
    ASSERT(A);
    rstack_t *B = rstack_new();
    ASSERT(B);

    ASSERT_RESULT(rstack_front(A), false, 0);

    CHECK_IF_NO_ERROR(rstack_push_value(A, 1));
    ASSERT_RESULT(rstack_front(A), true, 1);

    CHECK_IF_NO_ERROR(rstack_push_value(A, 2));
    ASSERT_RESULT(rstack_front(A), true, 2);

    CHECK_IF_NO_ERROR(rstack_push_rstack(A, B));
    CHECK_IF_NO_ERROR(rstack_push_rstack(A, B));

    ASSERT_RESULT(rstack_front(A), true, 2);

    rstack_delete(A);
    rstack_delete(B);
    return 0;
}
