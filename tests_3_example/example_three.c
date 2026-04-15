#include "../rstack.h"
#include "../macros.h"

int main(void) {
    rstack_t *rs0 = rstack_new();
    ASSERT(rs0);
    rstack_t *rs3 = rstack_new();
    ASSERT(rs3);

    NO_ERROR(rstack_push_value(rs0, 7));
    NO_ERROR(rstack_push_rstack(rs3, rs0));
    NO_ERROR(rstack_push_rstack(rs3, rs0));
    NO_ERROR(rstack_push_rstack(rs3, rs0));
    NO_ERROR(rstack_push_value(rs0, 8));
    NO_ERROR(rstack_push_value(rs0, 9));
    ASSERT(rstack_empty(rs3) == false);
    ASSERT(rstack_empty(rs0) == false);
    ASSERT_RESULT(rstack_front(rs3), true, 9);
    ASSERT_RESULT(rstack_front(rs0), true, 9);
    rstack_pop(rs3);
    rstack_pop(rs0);
    NO_ERROR(rstack_write(OUTPUT_FILE, rs3));
    ASSERT(rstack_empty(rs3) == false);
    ASSERT(rstack_empty(rs0) == false);
    ASSERT_RESULT(rstack_front(rs3), true, 8);
    ASSERT_RESULT(rstack_front(rs0), true, 8);
    rstack_delete(rs3);
    rstack_delete(rs0);

    return PASS;
}
