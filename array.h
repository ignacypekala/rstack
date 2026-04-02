#ifndef RSTACK_ARRAY
#define RSTACK_ARRAY

#include <stddef.h>
#include "types.h"

int init_array(rstack_stack_t *stack, size_t initial_size);
int array_push(rstack_stack_t *stack, rstack_t *rs);

#endif // RSTACK_ARRAY
