/*
 * Implements functions for creation and management of rstack containers
 */
#include "rstack_container.h"
#include "types.h"
#include <stdlib.h>

#define ARRAY_GROWTH_FACTOR 2

// Initialises an rstack_container_t.
// Assumes initial_capacity > 0. Returns a nullptr on allocation failure.
rstack_container_t *init_rstack_container(size_t initial_capacity) {
    rstack_container_t *container = malloc(sizeof(rstack_container_t));
    if (container == nullptr) {
        return nullptr;
    }
    container->size = 0;
    container->capacity = initial_capacity;
    container->references = 1;
    container->visited = false;

    container->array = malloc(sizeof(rstack_t *) * container->capacity);
    if (container->array == nullptr) {
        free(container);
        return nullptr;
    }
    return container;
}

// Pushes an rstack_t to the array of an rstack_container_t.
// On insufficient space, reallocates the array with a capacity multiplied by
// the ARRAY_GROWTH_FACTOR. Returns 0 on success, -1 in case of reallocation
// failure (the original pointer is left intact).
int rstack_container_push(rstack_container_t *container, rstack_t *rs) {
    if (container->size == container->capacity) {
        container->capacity *= ARRAY_GROWTH_FACTOR;
        rstack_t **new_pointer =
          realloc(container->array, sizeof(rstack_t *) * container->capacity);
        if (new_pointer == nullptr) {
            return -1;
        }
        container->array = new_pointer;
    }
    container->array[container->size++] = rs;
    return 0;
}
