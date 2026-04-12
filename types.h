/*
 * Defines shared datatypes for use in different files
 */
#ifndef RSTACK_TYPES
#define RSTACK_TYPES

#include <stddef.h>
#include <inttypes.h>
#include "rstack.h"

// Rstack garbage collector state AKA the node color
enum rstack_gc_state {
    NORMAL, // (uncolored/black)
    UNDER_TRIAL, // (gray)
    RESCUED, // (black)
    PROVISIONALLY_DEAD, // (white)
    DEAD,
};

typedef struct {
    rstack_t           **array;
    size_t               size;
    size_t               capacity;
    uint64_t             references;
    bool                 visited;
    enum rstack_gc_state state;
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
