#include "../rstack.h"
#include "../macros.h"

int main(void) {
    rstack_t *rs1 = rstack_new();
    ASSERT(rs1);
    rstack_t *rs2 = rstack_new();
    ASSERT(rs2);

    NO_ERROR(rstack_push_rstack(rs1, rs2));
    NO_ERROR(rstack_push_rstack(rs2, rs1));
    ASSERT(rstack_empty(rs1) == true);
    ASSERT(rstack_empty(rs2) == true);
    ASSERT_RESULT(rstack_front(rs1), false, 0);
    ASSERT_RESULT(rstack_front(rs2), false, 0);
    rstack_delete(rs1);
    rstack_delete(rs2);

    rs2 = rstack_new();
    ASSERT(rs2);

    NO_ERROR(rstack_push_value(rs2, 4));
    NO_ERROR(rstack_push_value(rs2, 5));
    NO_ERROR(rstack_push_rstack(rs2, rs2));
    ASSERT(rstack_empty(rs2) == false);
    ASSERT_RESULT(rstack_front(rs2), true, 5);
    NO_ERROR(rstack_push_value(rs2, 6));
    NO_ERROR(rstack_write(OUTPUT_FILE, rs2));
    ASSERT(rstack_empty(rs2) == false);
    ASSERT_RESULT(rstack_front(rs2), true, 6);
    rstack_delete(rs2);

    return PASS;
}
