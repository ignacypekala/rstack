#include "array.h"
#include "types.h"
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

rstack_t* rstack_new()
{
    rstack_t* rs = malloc(sizeof(rstack_t));
    if (rs == nullptr) {
        errno = ENOMEM;
    }
    rs->type = STACK;
    struct rstack_stack* stack = (*rs).body.stack;
    stack->arr = nullptr;
}

void rstack_delete(rstack_t* rs) { }
int rstack_push_value(rstack_t* rs, uint64_t value)
{
    return 0;
}
int rstack_push_rstack(rstack_t* rs1, rstack_t* rs2)
{
    return 0;
}
void rstack_pop(rstack_t* rs) { }
bool rstack_empty(rstack_t* rs)
{
    return false;
}
result_t rstack_front(rstack_t* rs)
{
    result_t t = { };
    return t;
}
rstack_t* rstack_read(char const* path)
{
    return nullptr;
}
int rstack_write(char const* path, rstack_t* rs)
{
    return 0;
}
