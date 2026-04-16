/*
 * Defines shared datatypes for use in other files.
 *
 * Note: To prevent ambiguity between the overall 'rstack_t' type and the
 * specific variants that hold nested elements, the latter are referred to as
 * "containers".
 */
#ifndef RSTACK_TYPES
#define RSTACK_TYPES

#include "rstack.h"
#include <inttypes.h>
#include <stddef.h>

// Represents the lifecycle state of a node during trial deletion. 
// It is the equivalent of a color in the Bacon/Rajan trial deletion algorithm.
enum rstack_gc_state : unsigned char {
    NORMAL,
    UNDER_TRIAL,
    RESCUED,
    PROVISIONALLY_DEAD,
    DEAD
};

typedef struct rstack rstack_t;

typedef struct {
    rstack_t           **array;
    size_t               size;
    size_t               capacity;
    uint64_t             references;
    bool                 visited;
    enum rstack_gc_state gc_state;

    // Intrusive list pointer used to queue unreachable nodes during garbage
    // collection
    rstack_t            *gc_next;
} rstack_container_t;

enum rstack_type { 
    CONTAINER,
    NUMBER 
};

// The generic rstack object
typedef struct rstack {
    enum rstack_type type;
    union {
        uint64_t            number;
        rstack_container_t *container;
    } as;
} rstack_t;

#endif // RSTACK_TYPES
