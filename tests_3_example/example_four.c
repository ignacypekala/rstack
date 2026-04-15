#include "../rstack.h"
#include "../macros.h"

int main(void) {
    rstack_t *rs4 = rstack_read("./tests_3_example/example_four.data");
    ASSERT(rs4);

    NO_ERROR(rstack_write(OUTPUT_FILE, rs4));
    rstack_delete(rs4);

    return PASS;
}
