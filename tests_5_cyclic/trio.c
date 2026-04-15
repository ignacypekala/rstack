#include "../macros.h"
#include "../rstack.h"
#include <stdint.h>
#include <assert.h>

int main() {
    rstack_t *A = rstack_new();
    ASSERT(A != nullptr);

    rstack_t *B = rstack_new();
    ASSERT(B != nullptr);

    rstack_t *C = rstack_new();
    ASSERT(C != nullptr);
    NO_ERROR(rstack_push_value(C, 3));
    
    NO_ERROR(rstack_push_rstack(A, B));
    NO_ERROR(rstack_push_rstack(C, B));
    ASSERT(rstack_empty(B) == true);
    rstack_delete(B);

    NO_ERROR(rstack_push_rstack(A, C));
    NO_ERROR(rstack_push_rstack(C, A));

    ASSERT(rstack_empty(A) == false);
    ASSERT(rstack_empty(C) == false);

    rstack_delete(C);
    rstack_delete(A);
}
