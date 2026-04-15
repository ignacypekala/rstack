#include "rstack.h"
#include "rstack_container.h"
#include "rstack_delete.h"
#include "types.h"
#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/types.h>

static const int UINT64_MAX_STRING_LENGTH = 20;
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
        rstack_t *element = container->array[container->size - i - 1];
        if (element->type == NUMBER) {
            t.flag = true;
            t.value = element->as.number;
            break;
        } else {
            t = rstack_front(element);
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
 * Reads a uint64_t from a file. Returns:
 * * 0 - success
 * * -1 - no number found
 * * -2 - invalid file contents have been detected
 * * -3 - file stream operation failed
 * On failure errno is set according to the specification of rstack_read.
 */
static int rstack_read_number(FILE *file, uint64_t *number) {
    char buffer[UINT64_MAX_STRING_LENGTH + 1];
    int i = 0;
    int leading_zeros = 0;
    while (!feof(file) && i < UINT64_MAX_STRING_LENGTH) {
        int c = fgetc(file);
        if (feof(file)) {
            break;
        } else if (c == EOF) {
            return -3;
        }

        if (i == 0 && (c == '0' || isspace(c))) {
            if (c == '0') leading_zeros++;
            continue; 
        } 

        if (isspace(c)) break;

        if (isdigit(c)) {
            buffer[i++] = (char) c;
        } else {
            errno = EBADMSG;
            return -2;
        }
    }
    // Trailing characters may remain if the buffer was filled
    if (i == UINT64_MAX_STRING_LENGTH) {
        int c = getc(file);
        if (c != EOF) {
            if (!isspace(c)) {
                errno = EBADMSG;
                return -2;
            }
            errno = ENOBUFS;
        }
        if (c != EOF && ungetc(c, file) == EOF) {
            return -3;
        } 
    }
    // fprintf(stderr, "%s\n", buffer);

    if (i == 0) {
        if (leading_zeros > 0) {
            buffer[i++] = '0';
        } else {
            return -1;
        }
    }

    buffer[i++] = '\0';

    errno = 0;
    char *end_ptr = nullptr;
    *number = (uint64_t) strtoull(buffer, &end_ptr, 10);
    if (errno == ERANGE) {
        return -2;
    }
    return 0;
}

/*
 * Reads numerical values from a whitespace-separated text file and
 * populates a new rstack.
 * Returns a pointer to the rstack, or nullptr on failure.
 * Possible errno values:
 *  - EINVAL: path is nullptr.
 *  - EBADMSG: invalid file contents
 *  - ERANGE: a value in the file exceeds the range of uint64_t.
 *  - ENOMEM: memory allocation failed.
 *  - ENOBUFS: ungetc failed to unget a character to the file descriptor
 *  And any values specified by the POSIX standard:
 *  https://pubs.opengroup.org/onlinepubs/9699919799/functions/fopen.html
 *  https://pubs.opengroup.org/onlinepubs/9699919799/functions/strtoul.html
 */
rstack_t *rstack_read(char const *path) {
    if (path == nullptr) {
        errno = EINVAL;
        return nullptr;
    }

    FILE *file = fopen(path, "r");
    if (file == nullptr) {
        return nullptr;
    }

    rstack_t *stack = rstack_new();

    uint64_t number = 0;
    int code;
    while (!feof(file) && (code = rstack_read_number(file, &number)) == 0) {
        // fprintf(stderr, "%d\n", code);
        if (rstack_push_value(stack, number) != 0) {
            rstack_delete(stack);
            fclose(file);
            return nullptr;
        }
    }
    fclose(file);
    if (code < -1) {
        // fprintf(stderr, "failed: %d\n", code);
        rstack_delete(stack);
        return nullptr;
    }
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

    container->visited = false;
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
