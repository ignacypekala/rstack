#include "../rstack.h"
#include "../macros.h"
#include "errno.h"
#include <stdio.h>

int main() {
    rstack_t *A = rstack_new();
    ASSERT(A);

    rstack_delete(nullptr);
    ASSERT(rstack_push_value(nullptr, 0) == -1);
    ASSERT(errno == EINVAL);

    ASSERT(rstack_push_rstack(A, nullptr) == -1);
    ASSERT(errno == EINVAL);

    ASSERT(rstack_push_rstack(nullptr, A) == -1);
    ASSERT(errno == EINVAL);

    rstack_pop(A);
    rstack_delete(A);

    ASSERT_RESULT(rstack_front(nullptr), false);

    ASSERT(rstack_empty(nullptr) == true);

    errno = 0;
    ASSERT(rstack_read(nullptr) == nullptr);
    ASSERT(errno != 0);

    errno = 0;
    ASSERT(rstack_read("./tam/gdzie/diabel/mowi/dobranoc.txt") == nullptr);
    ASSERT(errno != 0);

    errno = 0;
    ASSERT(rstack_write(nullptr, A) == -1);
    ASSERT(errno != 0);

    char dummy_test_file[] = "test_malicious_einval.txt";
    fclose(fopen(dummy_test_file, "w"));
    errno = 0;
    ASSERT(rstack_write(dummy_test_file, nullptr) == -1);
    ASSERT(errno != 0);
    remove(dummy_test_file);
}
