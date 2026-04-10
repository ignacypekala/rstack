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
 * Creates an rstack object of type CONTAINER and initializes its internal storage.
 * Returns a pointer to the structure, or nullptr on allocation failure.
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

/*
 * Deletes an rstack and its contents.
 * For containers, it decrements the reference count and performs a recursive 
 * deletion if the count reaches zero. Does nothing if rs is a nullptr.
 */
void rstack_delete(rstack_t *rs) {
    if (rs == nullptr) return;

    if (rs->type == CONTAINER) {
        rstack_container_t *container = rs->as.container;
        container->references--;
        
        if (container->references <= 0) {
            for (size_t i = 0; i < container->size; i++) {
                rstack_delete(container->array[i]);
            }
            free(container->array);
            free(container);
            free(rs);
        }
    } else {
        free(rs);
    }
}

/*
 * Wraps a raw uint64_t in an rstack object and pushes it onto the stack.
 * Returns 0 on success, -1 on error (sets errno to EINVAL or ENOMEM).
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
 * Returns 0 on success, -1 on error (sets errno to EINVAL or ENOMEM).
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
 * populates a new rstack. Sets errno on failure (EINVAL, EBADMSG, or ERANGE).
 */
rstack_t *rstack_read(char const *path) {
    if (path == nullptr) {
        errno = EINVAL;
        return nullptr;
    }

    FILE *file = fopen(path, "r");
    if (file == nullptr) return nullptr;

    rstack_t *stack = rstack_new();

    char buffer[21]; 
    while (fscanf(file, "%20s", buffer) == 1) {
        if (!feof(file)) {
            char trailing_char = fgetc(file);
            if (!isspace(trailing_char)) {
                errno = EBADMSG;
                rstack_delete(stack);
                fclose(file);
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
 */
int rstack_write_helper(FILE *file, rstack_t *rs) {
    if (rs == nullptr) return 0;

    rstack_container_t *container = rs->as.container;
    if (container->visited) return 1;
    
    container->visited = true;
    int return_code = 0;

    for (size_t i = 0; i < container->size; i++) {
        rstack_t *substack = container->array[container->size - i - 1];
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
 * Returns 0 on success, -1 on error (sets errno to EINVAL or file errors).
 */
int rstack_write(char const *path, rstack_t *rs) {
    if (path == nullptr || rs == nullptr) {
        errno = EINVAL;
        return -1;
    }

    FILE *file = fopen(path, "w");
    if (file == nullptr) return -1;

    int result = rstack_write_helper(file, rs);
    if (result == -1) {
        return -1;
    }
    fclose(file);
    
    return (result == -1) ? -1 : 0;
}
