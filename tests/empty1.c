#include "macros.h"
#include <stdint.h>
#include <assert.h>
#include "../rstack.h"

int main() {
    rstack_t *A = rstack_new();
    ASSERT(A);
    ASSERT(rstack_empty(A) == true);
    rstack_t *B = rstack_new();
    ASSERT(B);
    ASSERT(rstack_empty(B) == true);

    NO_ERROR(rstack_push_value(A, 42));
    ASSERT(rstack_empty(A) == false);

    rstack_pop(A);
    ASSERT(rstack_empty(A) == true);

    rstack_delete(A);
    rstack_delete(B);
    return 0;
}
