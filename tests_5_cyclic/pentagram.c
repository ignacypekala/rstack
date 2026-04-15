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
    rstack_t *D = rstack_new();
    ASSERT(D != nullptr);
    rstack_t *E = rstack_new();
    ASSERT(E != nullptr);

    NO_ERROR(rstack_push_rstack(A, C));
    NO_ERROR(rstack_push_rstack(A, B));

    NO_ERROR(rstack_push_rstack(B, D));
    NO_ERROR(rstack_push_rstack(B, C));

    NO_ERROR(rstack_push_rstack(C, E));
    NO_ERROR(rstack_push_rstack(C, D));
    
    NO_ERROR(rstack_push_rstack(D, A));
    NO_ERROR(rstack_push_rstack(D, E));

    NO_ERROR(rstack_push_rstack(E, B));
    NO_ERROR(rstack_push_rstack(E, A));

    rstack_delete(A);

    ASSERT(rstack_empty(E) == true);
    rstack_delete(E);

    rstack_delete(D);
    rstack_delete(B);
    rstack_delete(C);
}
