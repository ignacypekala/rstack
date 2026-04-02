/*
 * Defines shared datatypes like rstack, so that they 
 * can be used in different files.
 */
#ifndef RSTACK_TYPES
#define RSTACK_TYPES

#include "rstack.h"

typedef struct {
    rstack_t **array;
    size_t size;
    size_t capacity;
    uint64_t references;
} rstack_stack_t ;

enum rstack_value_type {
    STACK,
    NUMBER
};

typedef struct rstack {
    enum rstack_value_type type;
    // Saves memory by storing a number and a stack in the same memory space
    union stack_or_number {
        uint64_t number;
        rstack_stack_t* stack;
    } body;
} rstack_t;


#endif // RSTACK_TYPES
