#include "../rstack.h"

int main() {
    rstack_t *A = rstack_new();
    rstack_t *B = rstack_new();
    rstack_t *C = rstack_new();
    rstack_push_rstack(A, B);
    rstack_push_rstack(A, C);
    rstack_push_rstack(C, B);
    rstack_push_rstack(C, A);
    rstack_delete(B);
    rstack_delete(C);
    rstack_delete(A);
}
