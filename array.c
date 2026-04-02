/*
 * Implements functions for dynamic stack arrays
 */
#include "array.h"
#include "types.h"
#include <stdlib.h>

#define ARRAY_GROWTH_FACTOR 2

// Initialises an rstack array within a given rstack_stack 
// assumes that initial_size > 0
// returns 0 on success, -1 on memory allocation failure
int init_array(rstack_stack_t *stack, size_t initial_size) {
    rstack_t **array = malloc(sizeof(rstack_t*) * initial_size);
    if (array == nullptr) {
        return -1;
    }
    stack->array = array;
    return 0;
}

// Pushes an rstack_t into the array of an rstack_stack_t.
// On insufficient space, reallocates the array with a capacity multiplied by
// the ARRAY_GROWTH_FACTOR. Returns 0 on success, -1 in case of reallocation 
// failure (the original pointer is left intact).
int array_push(rstack_stack_t *stack, rstack_t *rs) {
    if (stack->size == stack->capacity) {
        stack->capacity *= ARRAY_GROWTH_FACTOR;
        rstack_t **new_pointer = realloc(
            stack->array,
            sizeof(rstack_t*) * stack->capacity
        );
        if (new_pointer == nullptr) {
            return -1;
        }
        stack->array = new_pointer;
    }
    stack->array[stack->size++] = rs;
    return 0;
}

