#include "macros.h"
#include "../rstack.h"

int main() {
    rstack_t *stack = rstack_new();
    ASSERT(stack);
    rstack_delete(stack);
    return 0;
}
