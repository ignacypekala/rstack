#include "../macros.h"
#include "../rstack.h"
#include <stdint.h>
#include <assert.h>


int main() {
    rstack_t *A = rstack_new();
    ASSERT(A != nullptr);
    NO_ERROR(rstack_push_value(A, 1));

    rstack_t *B = rstack_new();
    ASSERT(B != nullptr);
    NO_ERROR(rstack_push_value(B, 2));

    rstack_t *C = rstack_new();
    ASSERT(C != nullptr);
    NO_ERROR(rstack_push_value(C, 3));

    rstack_t *D = rstack_new();
    ASSERT(D != nullptr);
    NO_ERROR(rstack_push_value(D, 4));

    rstack_t *E = rstack_new();
    ASSERT(E != nullptr);
    NO_ERROR(rstack_push_value(E, 5));

    NO_ERROR(rstack_push_rstack(A, B));
    NO_ERROR(rstack_push_rstack(B, C));
    rstack_delete(B);
    NO_ERROR(rstack_push_rstack(C, D));
    NO_ERROR(rstack_push_rstack(D, E));
    NO_ERROR(rstack_push_rstack(E, A));

    ASSERT(rstack_empty(A) == false);
    NO_ERROR(rstack_push_value(C, 1));
    rstack_delete(E);
    ASSERT_RESULT(rstack_front(D), true, 1);
    rstack_pop(C);
    
    rstack_write(OUTPUT_FILE, A);

    rstack_delete(D);
    rstack_delete(A);
    rstack_delete(C);
}
