#include "rstack.h"
#include "rstack_container.h"
#include "types.h"
#include <errno.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

static const size_t INITIAL_ARRAY_SIZE = 10;

/*
 * Creates a generic instance of rstack_t. 
 * Returns a pointer to the created structure or nullptr on allocation failure.
 */
static rstack_t *rstack_generic_new() {
    rstack_t *rs = malloc(sizeof(rstack_t));
    if (rs == nullptr) return nullptr;
    return rs;
}

/*
 * Creates a new rstack_t object with type == NUMBER.
 * Returns a pointer to the created structure or nullptr on allocation failure.
 */
static rstack_t *rstack_number_new(uint64_t number) {
    rstack_t *rs = rstack_generic_new();
    if (rs == nullptr) return nullptr;
    rs->type = NUMBER;
    rs->as.number = number;
    return rs;
}

/*
 * Creates a new rstack_t object with type == CONTAINER. The container is
 * created too. Returns a pointer to the created structure or nullptr on
 * failure.
 */
static rstack_t *rstack_container_new() {
    rstack_t *rs = rstack_generic_new();
    if (rs == nullptr) return nullptr;
    rs->type = CONTAINER;

    rs->as.container = init_rstack_container(INITIAL_ARRAY_SIZE);
    if (rs->as.container == nullptr)
        free(rs);

    return rs;
}

/*
 * Creates a new rstack_t with type == CONTAINER.
 * Returns a pointer to the created structure or nullptr in the case of
 * allocation failure (errno is set to ENOMEM).
 */
rstack_t *rstack_new() {
    rstack_t *rs = rstack_container_new();
    if (rs == nullptr) errno = ENOMEM;
    return rs;
}

/*
 * Deletes an rstack.
 * * rs - pointer to the deleted structure
 * Doesn't do anything if nullptr was provided. 
 * The supplied pointer mustn't be used after deletion.
 * TODO: Detect orphaned cycles
 */
void rstack_delete(rstack_t *rs) {
    if (rs == nullptr) return;

    if (rs->type == CONTAINER) {
        rstack_container_t *container = rs->as.container;
        container->references--;
        // Upon deletion of this rstack_t, its descendants will lose a reference.
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
 * Pushes a numerical value onto an rstack.
 * * rs - pointer to an rstack
 * * value - pushed number
 * Returns 0 on success, -1 if rs == nullptr or an error occured
 * when allocating memory. In case of failure errno is set accordingly to EINVAL
 * or ENOMEM. Assumes rs == nullptr || rs->type == CONTAINER
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
        errno = ENOMEM;
        return -1;
    }
    return 0;
}

/*
 * Pushes an rstack onto an rstack.
 * * rs1 - pointer to the rstack onto which an rstack will be pushed
 * * rs2 - pointer to the pushed rstack
 * Returns 0 on success, -1 if either rs1 == nullptr, rs2 == nullptr, or an
 * error occured when allocating memory. In case of failure errno is set to
 * EINVAL or ENOMEM, accordingly.
 * Assumes both rs
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
 * Non-recursively pops the top element of an rstack. 
 * * rs - pointer to an rstack 
 * If rs == nullptr or the rstack is empty, doesn't do anything.
 */
void rstack_pop(rstack_t *rs) {
    rstack_container_t *container = rs->as.container;
    if (container->size <= 0)
        return;
    rstack_delete(container->array[--container->size]);
}

/*
 * Recursively finds the topmost numerical value on an rstack. 
 * * rs - pointer to an rstack 
 * Returns a result_t structure, where: flag == true <==> value
 * contains the found number flag == false <==> rs == nullptr, the rstack is
 * empty or there is no such number.
 * Assumes rs == nullptr || rs->type == CONTAINER
 */
result_t rstack_front(rstack_t *rs) {
    result_t t = {};
    if (rs == nullptr) {
        t.flag = false;
        return t;
    }

    rstack_container_t *container = rs->as.container;
    if (container->visited) {
        t.flag = false;
        return t;
    } 
    container->visited = true;

    for (size_t i = 0; i < container->size; i++) {
        rstack_t *substack = container->array[i];
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
 * Recursively checks whether an rstack contains a number.
 * * rs - pointer to an rstack
 * Returns true if rs == nullptr or rstack doesn't contain a number.
 * false - if the rstack contains a number
 * Assumes rs->type == CONTAINER
 */
bool rstack_empty(rstack_t *rs) {
    result_t t = rstack_front(rs);
    return !t.flag;
}

/*
 * Creates a new stack, with numbers found in a file.
 * * path - path to a file
 * Returns a pointer to an rstack or nullptr if path == nullptr or
 * an error occured.  In which case the appropriate errno is set. (define)
 */
rstack_t *rstack_read(char const *path) {
    return nullptr;
}

/*
 * Writes the numbers from a stack to a file.
 * * path - name of a file
 * * rs - pointer to an rstack
 * Returns 0 on success, -1 if either path or rs is a nullptr, or an
 * error occured. In which case the appropriate errno is set. (define) 
 */
int rstack_write(char const *path, rstack_t *rs) {
    return 0;
}
