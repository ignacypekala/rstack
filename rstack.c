#include "array.h"
#include "types.h"
#include "rstack.h"
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

// Creates a new empty rstack.
// Returns a pointer to the created structure or nullptr on failure.
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

// Deletes an rstack.
// - rs - pointer to the deleted structure
// Doesn't do anything if nullptr was provided. 
// After deletion the rstack pointer must not be used.
void rstack_delete(rstack_t* rs) { }

// Pushes a numerical value to an rstack.
// - rs - pointer to an rstack
// value - pushed number
// Returns 0 on success, -1 if rs == nullptr or an error occured when allocating 
// memory. In case of failure errno is set accordingly to EINVAL or ENOMEM.
int rstack_push_value(rstack_t* rs, uint64_t value)
{
    return 0;
}

// Pushes an rstack onto an rstack.
// - rs1 - pointer to the rstack onto which an rstack will be pushed
// - rs2 - pointer to the pushed rstack 
// Returns 0 on success, -1 if either rs1 == nullptr or rs2 == nullptr
// or an error occured when allocating memory. 
// In case of failure errno is set accordingly to EINVAL or ENOMEM.
int rstack_push_rstack(rstack_t* rs1, rstack_t* rs2)
{
    return 0;
}

// Pops non-recursively the top element of an rstack.
// - rs - pointer to an rstack
// If rs == nullptr or the rstack is empty, doesn't do anything.
void rstack_pop(rstack_t* rs) { }

// Recursively checks whether an rstack contains a number.
// - rs - pointer to an rstack
// Returns true if rs == nullptr or the rstack doesn't contain a number.
// false - if the rstack contains a number
bool rstack_empty(rstack_t* rs)
{
    return false;
}

// Recursively finds the topmost numerical value on an rstack.
// - rs - pointer to an rstack
// Returns a result_t structure, where:
//  flag == true <==> value contains the found number
//  flag == false <==> rs == nullptr, the rstack is empty 
//  or there is no such number.
result_t rstack_front(rstack_t* rs)
{
    result_t t = { };
    return t;
}

// Creates a new stack, with numbers found in a file.
// - path - path to a file
// Returns a pointer to an rstack or nullptr if path == nullptr or 
// an error occured.  In which case the appropriate errno is set. (define)
rstack_t* rstack_read(char const* path)
{
    return nullptr;
}

// Writes the numbers from a stack to a file.
// - path - name of a file
// - rs - pointer to an rstack
// Returns 0 on success, -1 if either path or rs is a nullptr, or an 
// error occured. In which case the appropriate errno is set. (define)
int rstack_write(char const* path, rstack_t* rs)
{
    return 0;
}
