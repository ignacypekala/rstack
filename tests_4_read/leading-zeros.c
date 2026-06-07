#include "../macros.h"
#include "../rstack.h"
#include <assert.h>
#include <inttypes.h>

int main() {
    rstack_t *stack = rstack_read("tests_malicious/leading-zeros.data");
    if (stack != nullptr) {
        ASSERT_RESULT(rstack_front(stack), true, 1);
        rstack_pop(stack);
        ASSERT_RESULT(rstack_front(stack), true, 2);
        rstack_pop(stack);
        ASSERT_RESULT(rstack_front(stack), true, 18446744073709551615ULL);
        rstack_pop(stack);
        ASSERT_RESULT(rstack_front(stack), true, 0);
        rstack_pop(stack);
        ASSERT_RESULT(rstack_front(stack), true, 18446744073709551615ULL);
        rstack_pop(stack);
    }
    rstack_delete(stack);
    return 0;
}
