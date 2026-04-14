#include "../rstack.h"
#include "../macros.h"

int main(void) {
    rstack_t *rs5 = rstack_new();
    ASSERT(rs5);
    rstack_t *rs6 = rstack_new();
    ASSERT(rs6);
    rstack_t *rs7 = rstack_new();
    ASSERT(rs7);

    NO_ERROR(rstack_push_value(rs5, 5));
    NO_ERROR(rstack_push_value(rs6, 6));
    NO_ERROR(rstack_push_rstack(rs6, rs6));
    NO_ERROR(rstack_push_rstack(rs7, rs7));
    NO_ERROR(rstack_push_rstack(rs6, rs7));
    NO_ERROR(rstack_push_rstack(rs5, rs6));
    ASSERT(rstack_empty(rs5) == false);
    ASSERT(rstack_empty(rs6) == false);
    ASSERT(rstack_empty(rs7) == true);
    ASSERT_RESULT(rstack_front(rs5), true, 6);
    ASSERT_RESULT(rstack_front(rs6), true, 6);
    ASSERT_RESULT(rstack_front(rs7), false, 0);
    NO_ERROR(rstack_write(OUTPUT_FILE, rs5));
    rstack_delete(rs5);
    rstack_delete(rs6);
    rstack_delete(rs7);

    return PASS;
}
