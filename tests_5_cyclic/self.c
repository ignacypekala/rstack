#include "../macros.h"
#include "../rstack.h"
#include <stdint.h>
#include <assert.h>

int main() {
    rstack_t *A = rstack_new();
    NO_ERROR(rstack_push_rstack(A, A));
    ASSERT(rstack_empty(A) == true);

    rstack_pop(A);
    rstack_push_value(A, 1);
    ASSERT(rstack_empty(A) == false);
    NO_ERROR(rstack_push_rstack(A, A));
    ASSERT(rstack_empty(A) == false);

    rstack_delete(A);
}
