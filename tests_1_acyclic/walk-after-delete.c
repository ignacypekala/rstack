#include "../macros.h"
#include "../rstack.h"
#include <stdint.h>
#include <assert.h>


int main() {
    rstack_t *A = rstack_new();
    ASSERT(rstack_empty(A) == true);
    rstack_t *B = rstack_new();
    NO_ERROR(rstack_push_value(B, 1));
    NO_ERROR(rstack_push_rstack(B, A));
    rstack_delete(A);
    ASSERT(rstack_empty(B) == false);
    ASSERT_RESULT(rstack_front(B), true, 1);
    rstack_delete(B);
}
