/*
 * Implements functions for creation and management of rstack containers
 */
#include "rstack_container.h"
#include "types.h"
#include <stdlib.h>

#define ARRAY_GROWTH_FACTOR 2

/*
 * Initializes an rstack_container_t and it's internal storage. Assumes
 * initial_capacity > 0. Returns a pointer to the newly allocated container, or
 * nullptr on allocation failure.
 */
rstack_container_t *init_rstack_container(size_t initial_capacity) {
    rstack_container_t *container = malloc(sizeof(rstack_container_t));
    if (container == nullptr) {
        return nullptr;
    }
    container->size = 0;
    container->capacity = initial_capacity;
    container->references = 1;
    container->dfs_visiting = false;
    container->dfs_visited = false;
    container->gc_state = NORMAL;
    container->gc_next = nullptr;

    container->array = malloc(sizeof(rstack_t *) * container->capacity);
    if (container->array == nullptr) {
        free(container);
        return nullptr;
    }
    return container;
}

/*
 * Pushes an rstack_t object onto the internal array of the container.
 * If the current capacity is reached, the array is dynamically expanded
 * by a factor of ARRAY_GROWTH_FACTOR.
 * Returns 0 on success, or -1 if reallocation fails (in which case the
 * original array pointer is left intact).
 */
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
