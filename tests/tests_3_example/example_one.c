#include "../rstack.h"
#include "../macros.h"

int main(void) {
    rstack_t *rs0 = rstack_new();
    ASSERT(rs0);
    rstack_t *rs1 = rstack_new();
    ASSERT(rs1);

    NO_ERROR(rstack_push_value(rs1, 1));
    NO_ERROR(rstack_push_value(rs1, 2));
    NO_ERROR(rstack_push_value(rs0, 0));
    NO_ERROR(rstack_push_rstack(rs1, rs0));
    NO_ERROR(rstack_push_value(rs0, 3));
    ASSERT(rstack_empty(rs0) == false);
    ASSERT_RESULT(rstack_front(rs0), true, 3);
    rstack_delete(rs0);
    NO_ERROR(rstack_push_value(rs1, 4));
    NO_ERROR(rstack_write(OUTPUT_FILE, rs1));
    ASSERT(rstack_empty(rs1) == false);
    ASSERT_RESULT(rstack_front(rs1), true, 4);
    rstack_delete(rs1);

    return PASS;
}
