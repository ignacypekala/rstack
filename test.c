#include <stdio.h>
#include "rstack.h"
 
int main(void)
{
    rstack_t *stack = rstack_read("test.in");
    result_t t = rstack_front(stack);
    while (t.flag) {
        printf("%llu\n", (unsigned long long) t.value);
        t = rstack_front(stack);
    }
}
