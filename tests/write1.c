#include "macros.h"
#include "../rstack.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

uint64_t values[10] = {0, 1, UINT64_MAX, 2, 3, 4, 5, 6, 7, 8};

int write(char *path) {
    rstack_t *stack = rstack_new();
    ASSERT(stack);
    for (int i = 0; i < 10; i++) {
        NO_ERROR(rstack_push_value(stack, values[i]));
    }
    NO_ERROR(rstack_write(path, stack));
    rstack_delete(stack);

    return PASS;
}

int validate(char *path) {
    FILE *file = fopen(path, "rb");
    ASSERT(file);

    char buffer[32]; 
    for (size_t i = 0; i < SIZE(values); i++) {
        ASSERT(fgets(buffer, sizeof(buffer), file) != NULL);

        char *endptr;
        uint64_t current_val = strtoull(buffer, &endptr, 10);
        ASSERT(endptr != buffer);
        ASSERT(*endptr == '\n');
        ASSERT(*(endptr + 1) == '\0');
        ASSERT(current_val == values[SIZE(values) - i - 1]);
    }
    return PASS; 
}

int main() {
    char *path = "write1.out";
    remove(path);

    NO_ERROR(write(path));
    NO_ERROR(validate(path));

    // Ensures an empty file exists
    FILE *file = fopen(path, "w");
    ASSERT(file);
    fclose(file);

    NO_ERROR(write(path));
    NO_ERROR(validate(path));
}
