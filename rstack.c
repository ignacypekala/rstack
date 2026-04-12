#include "rstack.h"
#include "rstack_container.h"
#include "types.h"
#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

static const int UINT64_MAX_STRING_LENGTH = 20;
static const char* UINT64_STRING_FORMAT = "%20s";
static const size_t INITIAL_ARRAY_SIZE = 10;

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

    rs->as.container = init_rstack_container(INITIAL_ARRAY_SIZE);
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

static void increment_or_decrement(rstack_t *stack, bool should_increment) {
    rstack_container_t *container = stack->as.container;

    container->visited = true;
    for (size_t i = 0; i < container->size; i++) {
        rstack_t *substack = container->array[i];
        if (substack->type == CONTAINER) {
            rstack_container_t *substack_container = substack->as.container;
            substack_container->references += should_increment ? +1 : -1;
            if (!substack_container->visited) {
                increment_or_decrement(substack, should_increment);
            }
        }
    }
    container->visited = false;
}

static int scan_and_rescue(rstack_t *stack) {
    rstack_container_t *container = stack->as.container;
    if (container->visited) return 0;
    container->visited = true;
    for (size_t i = 0; i < container->size; i++) {
       rstack_t *substack = container->array[i];
       if (substack->type == CONTAINER) {
           rstack_container_t *substack_container = substack->as.container;
           if (container->references > 0) {
               substack_container->references++;
           }
           scan_and_rescue(substack);
       }
    }
    container->visited = false;
    return 0;
};

static void collect(rstack_t *stack) {
    rstack_container_t *container = stack->as.container;
    if (container->visited) return;

    if (container->references <= 0) {
        container->visited = true;
        
        for (size_t i = 0; i < container->size; i++) {
            rstack_t *substack = stack->as.container->array[i];
            if (substack->type == CONTAINER) {
                substack->as.container->references--;
                collect(substack);
            } else {
                free(substack);
            }
        }
        container->visited = false;
        free(container->array);
        free(container);
        free(stack);
    }
}

/*
 * Deletes an rstack and its contents.
 * For containers, it decrements the reference count and performs a recursive
 * deletion if the count reaches zero. Does nothing if rs is a nullptr.
 */
void rstack_delete(rstack_t *rs) {
    if (rs == nullptr) return;
    if (rs->type == NUMBER) {
        free(rs);
    } else {
        rstack_container_t *container = rs->as.container;
        if (--container->references <= 0) {
            for (size_t i = 0; i < container->size; i++) {
                rstack_delete(container->array[i]);
            }
            free(container->array);
            free(container);
            free(rs);
        } else {
            increment_or_decrement(rs, false);
            scan_and_rescue(rs);
            collect(rs);
        }
    }
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
 * Pushes one rstack onto another and increments the reference count of the child.
 * Returns 0 on success, -1 on error.
 * Possible errno values:
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
 * Recursively searches for the topmost numerical value in the stack.
 * Uses a 'visited' flag to prevent infinite loops in cyclic structures.
 * Returns a result_t with flag=true if a value is found.
 */
result_t rstack_front(rstack_t *rs) {
    result_t t = { .flag = false, .value = 0 };
    if (rs == nullptr) return t;

    rstack_container_t *container = rs->as.container;
    if (container->visited) return t;
    container->visited = true;

    for (size_t i = 0; i < container->size; i++) {
        rstack_t *substack = container->array[container->size - i - 1];
        if (substack->type == NUMBER) {
            t.flag = true;
            t.value = substack->as.number;
            break;
        } else {
            t = rstack_front(substack);
            if (t.flag) break;
        }
    }

    container->visited = false;
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
 * Reads numerical values from a whitespace-separated text file and
 * populates a new rstack.
 * Returns a pointer to the rstack, or nullptr on failure.
 * Possible errno values:
 *  - EINVAL: path is nullptr.
 *  - EBADMSG: File contains non-whitespace trailing characters or invalid
 *    formatting.
 *  - ERANGE: A value in the file exceeds the range of uint64_t.
 *  - ENOMEM: Memory allocation failed.
 *  And
 *  https://pubs.opengroup.org/onlinepubs/9699919799/functions/fopen.html
 *  https://pubs.opengroup.org/onlinepubs/9699919799/functions/strtoul.html
 */
rstack_t *rstack_read(char const *path) {
    if (path == nullptr) {
        errno = EINVAL;
        return nullptr;
    }

    FILE *file = fopen(path, "r");
    if (file == nullptr) return nullptr;

    rstack_t *stack = rstack_new();

    // Fscanf additionally writes \0 to the buffer (hence the size discrepancy).
    char buffer[UINT64_MAX_STRING_LENGTH + 1];
    while (fscanf(file, UINT64_STRING_FORMAT, buffer) == 1) {
        // Sequences longer than 20 digits fall out of range of uint64_t.
        if (!feof(file)) {
            char trailing_char = fgetc(file);
            if (!isspace(trailing_char)) {
                rstack_delete(stack);
                fclose(file);
                errno = EBADMSG;
                return nullptr;
            } else {
                ungetc(trailing_char, file);
            }
        }

        char *end_ptr = nullptr;
        errno = 0;
        uint64_t number = (uint64_t) strtoull(buffer, &end_ptr, 10);
        if (errno == ERANGE) {
            rstack_delete(stack);
            fclose(file);
            return nullptr;
        }

        if (rstack_push_value(stack, number) != 0) {
            rstack_delete(stack);
            fclose(file);
            return nullptr;
        };
    }
    fclose(file);
    return stack;
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
    if (container->visited) return 1;
    
    container->visited = true;
    int return_code = 0;

    for (size_t i = 0; i < container->size; i++) {
        rstack_t *substack = container->array[i];
        if (substack->type == NUMBER) {
            if (fprintf(file, "%" PRIu64 "\n", substack->as.number) < 0) {
                return_code = -1;
                break;
            }
        } else {
            int code = rstack_write_helper(file, substack);
            return_code = code;
            if (code != 0) break;
        }
    }

    container->visited = false;
    return return_code;
}

/*
 * Writes all numerical values in the rstack to a file, one per line.
 * Returns 0 on success, -1 on error.
 * Possible errno values:
 * - EINVAL: path or rs is nullptr.
 * - Any errno set by fopen: https://pubs.opengroup.org/onlinepubs/9699919799/functions/fopen.html
 * - Any errno set by fprintf: https://pubs.opengroup.org/onlinepubs/9699919799/functions/fprintf.html
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
