#ifndef RSTACK_CONTAINER
#define RSTACK_CONTAINER

#include "types.h"
#include <stddef.h>

rstack_container_t *init_rstack_container(size_t initial_capacity);
int rstack_container_push(rstack_container_t *container, rstack_t *rs);

#endif // RSTACK_CONTAINER
