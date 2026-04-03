/*
 * Defines shared datatypes like rstack, so that they
 * can be used in different files.
 */
#ifndef RSTACK_TYPES
#define RSTACK_TYPES

#include "rstack.h"

typedef struct {
    rstack_t **array;
    size_t     size;
    size_t     capacity;
    uint64_t   references;
    bool       visited; // A flag raised during traversal to detect cycles
} rstack_container_t;

enum rstack_type { 
    CONTAINER,
    NUMBER 
} type;

typedef struct rstack {
    enum rstack_type type;
    union {
        uint64_t            number;
        rstack_container_t *container;
    } as;
} rstack_t;

#endif // RSTACK_TYPES
