#include "rstack.h"
#include "rstack_container.h"
#include "rstack_delete.h"
#include "types.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

static const size_t RSTACK_INITAL_CAPACITY = 10;

/*
 * Internal helper to allocate a generic rstack structure.
 * Returns a pointer to the new structure, or nullptr on allocation failure.
 */
static rstack_t *rstack_generic_new() {
    rstack_t *rs = malloc(sizeof(rstack_t));
    if (rs == nullptr) return nullptr;
    return rs;
}

/*
 * Creates an rstack object of type NUMBER.
 * Returns a pointer to the structure, or nullptr on allocation failure.
 */
static rstack_t *rstack_number_new(uint64_t number) {
    rstack_t *rs = rstack_generic_new();
    if (rs == nullptr) return nullptr;
    rs->type = NUMBER;
    rs->as.number = number;
    return rs;
}

/*
 * Creates an rstack object of type CONTAINER and initializes its internal
 * storage. Returns a pointer to the structure, or nullptr on allocation
 * failure.
 */
static rstack_t *rstack_container_new() {
    rstack_t *rs = rstack_generic_new();
    if (rs == nullptr) return nullptr;
    rs->type = CONTAINER;

    rs->as.container = init_rstack_container(RSTACK_INITAL_CAPACITY);
    if (rs->as.container == nullptr) {
        free(rs);
        return nullptr;
    }

    return rs;
}

/*
 * Public constructor for a new rstack.
 * Sets errno to ENOMEM on failure.
 */
rstack_t *rstack_new() {
    rstack_t *rs = rstack_container_new();
    if (rs == nullptr) errno = ENOMEM;
    return rs;
}

/*
 * Wraps a raw uint64_t in an rstack object and pushes it onto the stack.
 * Returns 0 on success, -1 on error.
 * Possible errno values:
 * - EINVAL: rs is nullptr.
 * - ENOMEM: Memory allocation failed for the new element or container growth.
 */
int rstack_push_value(rstack_t *rs, uint64_t value) {
    if (rs == nullptr) {
        errno = EINVAL;
        return -1;
    }
    rstack_t *number = rstack_number_new(value);
    if (number == nullptr) {
        errno = ENOMEM;
        return -1;
    }
    if (rstack_container_push(rs->as.container, number) == -1) {
        rstack_delete(number);
        errno = ENOMEM;
        return -1;
    }
    return 0;
}

/*
 * Pushes one rstack onto another and increments the reference count of the
 * child. 
 * Returns 0 on success, -1 on error. Possible errno values:
 * - EINVAL: rs1 or rs2 is nullptr.
 * - ENOMEM: Memory allocation failed for container growth.
 */
int rstack_push_rstack(rstack_t *rs1, rstack_t *rs2) {
    if (rs1 == nullptr || rs2 == nullptr) {
        errno = EINVAL;
        return -1;
    }
    if (rstack_container_push(rs1->as.container, rs2) == -1) {
        errno = ENOMEM;
        return -1;
    }
    rs2->as.container->references++;
    return 0;
}

/*
 * Removes the top element from the rstack.
 * Does nothing if the stack is a nullptr or empty.
 */
void rstack_pop(rstack_t *rs) {
    if (rs == nullptr) return;
    rstack_container_t *container = rs->as.container;
    if (container->size <= 0)
        return;
    rstack_delete(container->array[--container->size]);
}

/*
 * Internal helper to recursively search for the topmost numerical value
 * in the stack container. Uses dfs_ flags to handle cyclic structures and
 * prevent infinite loops. Returns a result_t.
 */
static result_t rstack_front_traverser(rstack_t *stack) {
    result_t t = { .flag = false, .value = 0 };

    rstack_container_t *container = stack->as.container;
    if (container->dfs_visiting || container->dfs_visited) return t;
    container->dfs_visiting = true;

    for (size_t i = 0; i < container->size; i++) {
        rstack_t *element = container->array[container->size - i - 1];
        if (element->type == NUMBER) {
            t.flag = true;
            t.value = element->as.number;
            break;
        } else {
            t = rstack_front_traverser(element);
            if (t.flag) break;
        }
    }
    container->dfs_visited = true;
    container->dfs_visiting = false;
    return t;
}

/*
 * Internal helper to recursively clear the "dfs_visited" flag on a stack
 * and all of its nested container elements after a traversal completes.
 */
static void rstack_reset_visited(rstack_t *stack) {
    rstack_container_t *container = stack->as.container;
    if (!container->dfs_visited) return;
    container->dfs_visited = false;

    for (size_t i = 0; i < container->size; i++) {
        rstack_t *element = container->array[i];
        if (element->type == CONTAINER) {
            rstack_reset_visited(element);
        }
    }
}

/*
 * Retrieves the topmost numerical value from the stack.
 * Returns a result_t with the flag set to true and the corresponding value
 * on success, or flag set to false if the stack is empty or nullptr.
 */
result_t rstack_front(rstack_t *rs) {
    result_t t = { .flag = false, .value = 0 };
    if (rs == nullptr) return t;

    t = rstack_front_traverser(rs);
    rstack_reset_visited(rs);
    return t;
}

/*
 * Checks if the stack contains any numerical values, searching recursively.
 * Returns true if empty or nullptr, false if a number is found.
 */
bool rstack_empty(rstack_t *rs) {
    result_t t = rstack_front(rs);
    return !t.flag;
}


/*
 * Recursive helper for writing stack contents to a file.
 * Returns 1 if a cycle is detected, 0 on success, or -1 on write error.
 * On write error, errno is set via fprintf:
 * https://pubs.opengroup.org/onlinepubs/9699919799/functions/fprintf.html
 */
int rstack_write_helper(FILE *file, rstack_t *rs) {
    if (rs == nullptr) return 0;

    rstack_container_t *container = rs->as.container;
    if (container->dfs_visiting) return 1;

    container->dfs_visiting = true;
    int return_code = 0;

    for (size_t i = 0; i < container->size; i++) {
        rstack_t *element = container->array[i];
        if (element->type == NUMBER) {
            if (fprintf(file, "%" PRIu64 "\n", element->as.number) < 0) {
                return_code = -1;
                break;
            }
        } else {
            int code = rstack_write_helper(file, element);
            return_code = code;
            if (code != 0) break;
        }
    }

    container->dfs_visiting = false;
    return return_code;
}

/*
 * Writes all numerical values in the rstack to a file, one per line.
 * Returns 0 on success, -1 on error.
 * Possible errno values:
 * - EINVAL: path or rs is nullptr.
 * - Any errno set by fopen:
 *   https://pubs.opengroup.org/onlinepubs/9699919799/functions/fopen.html
 * - Any errno set by fprintf:
 *   https://pubs.opengroup.org/onlinepubs/9699919799/functions/fprintf.html
 */
int rstack_write(char const *path, rstack_t *rs) {
    if (path == nullptr || rs == nullptr) {
        errno = EINVAL;
        return -1;
    }

    FILE *file = fopen(path, "w");
    if (file == nullptr) return -1;

    int result = rstack_write_helper(file, rs);
    // Fclose may overwrite errno.
    int temp_errno = errno;
    fclose(file);
    errno = temp_errno;

    return (result == -1) ? -1 : 0;
}
