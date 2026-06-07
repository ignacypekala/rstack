#include "../macros.h"
#include "../rstack.h"
#include <stdint.h>
#include <assert.h>


int main() {
    rstack_t *A = rstack_new();
    ASSERT(A);
    ASSERT(rstack_empty(A) == true);
    ASSERT_RESULT(rstack_front(A), false);
    rstack_write(TEST_FILE("A1") , A);
    NO_ERROR(rstack_push_value(A, UINT64_MAX));
    ASSERT(rstack_empty(A) == false);
    ASSERT_RESULT(rstack_front(A), true, UINT64_MAX);

    rstack_t *B = rstack_new();
    ASSERT(B);
    NO_ERROR(rstack_push_value(B, 1));
    NO_ERROR(rstack_push_value(B, 2));
    NO_ERROR(rstack_push_rstack(B, A));

    ASSERT_RESULT(rstack_front(B), true, UINT64_MAX);
    ASSERT(rstack_empty(B) == false);
    rstack_write(TEST_FILE("B1") , B);

    rstack_delete(A);
    rstack_delete(B);
    return 0;
}
