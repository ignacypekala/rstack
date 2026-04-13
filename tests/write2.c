#include "macros.h"
#include "../rstack.h"
#include <assert.h>
#include <inttypes.h>

int main() {
    rstack_t *A = rstack_new();
    ASSERT(A);
    rstack_t *B = rstack_new();
    ASSERT(B);

    NO_ERROR(rstack_push_value(A, 1));
    NO_ERROR(rstack_push_value(B, 2));
    NO_ERROR(rstack_push_rstack(A, B));
    NO_ERROR(rstack_push_rstack(B, A));
    NO_ERROR(rstack_push_value(A, 3));
    NO_ERROR(rstack_push_value(B, 4));
    
    NO_ERROR(rstack_write(OUTPUT_FILE, A));
    rstack_t *C = rstack_read(OUTPUT_FILE);

    ASSERT_RESULT(rstack_front(C), true, 2); rstack_pop(C);
    ASSERT_RESULT(rstack_front(C), true, 1); rstack_pop(C);
    ASSERT_RESULT(rstack_front(C), false, 0);

    rstack_delete(A);
    rstack_delete(B);
    rstack_delete(C);
}
