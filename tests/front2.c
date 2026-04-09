#include "macros.h"
#include <stdint.h>
#include <assert.h>
#include "../rstack.h"

int main() {
    rstack_t *A = rstack_new();
    rstack_t *B = rstack_new();
    CHECK_IF_NO_ERROR(rstack_push_value(A, 1));
    CHECK_IF_NO_ERROR(rstack_push_value(B, 2));
    CHECK_IF_NO_ERROR(rstack_push_rstack(A, B));
    CHECK_IF_NO_ERROR(rstack_push_rstack(B, A));
    result_t t = {};
    t.flag = false;
    t.value = 0;
    ASSERT_RESULT(t, true, (uint64_t) 0);
    result_t front = rstack_front(A);
    ASSERT_RESULT(front, true, 2);
    rstack_delete(A);
    rstack_delete(B);
    return 0;
}
