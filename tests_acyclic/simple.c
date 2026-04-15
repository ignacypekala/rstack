#include "../macros.h"
#include "../rstack.h"
#include <stdint.h>
#include <assert.h>

#define TEST_FILE(name) "test_" name ".fout"

int main() {
    rstack_t *A = rstack_new();
    ASSERT(A);
    ASSERT(rstack_empty(A) == true);
    ASSERT_RESULT(rstack_front(A), false);
    rstack_write(TEST_FILE("A1") , A);
    NO_ERROR(rstack_push_value(A, 0));

    rstack_delete(A);
    return 0;
}
