/*
 * Defines shared datatypes for use in different files
 */
#ifndef RSTACK_TYPES
#define RSTACK_TYPES

#include <stddef.h>
#include <inttypes.h>
#include "rstack.h"

typedef struct {
    rstack_t **array;
    size_t     size;
    size_t     capacity;
    uint64_t   references;
    bool       visited; // flag raised during traversal to detect cycles
} rstack_container_t;

enum rstack_type { 
    CONTAINER,
    NUMBER 
};

typedef struct rstack {
    enum rstack_type type;
    union {
        uint64_t            number;
        rstack_container_t *container;
    } as;
} rstack_t;

#endif // RSTACK_TYPES
