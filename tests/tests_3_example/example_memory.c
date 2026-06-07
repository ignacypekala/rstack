#include "../memory_tests.h"
#include "../rstack.h"
#include "errno.h"
#include "../macros.h"

#define V(code, where) (((unsigned long)code) << (3 * where))

// Testuje reakcję implementacji na niepowodzenie alokacji pamięci.
// Błąd alokacji jest zgłaszany raz. Druga próba powinna się udać.
static unsigned long alloc_fail_test(void) {
    unsigned long visited = 0;
    rstack_t     *rs;

    errno = 0;
    if ((rs = rstack_new()) != nullptr)
        visited |= V(1, 0);
    else if (errno == ENOMEM && (rs = rstack_new()) != nullptr)
        visited |= V(2, 0);
    else
        return visited |= V(4, 0); // To nie powinno się wykonać.

    errno = 0;
    if (rstack_push_value(rs, 1) == 0)
        visited |= V(1, 1);
    else if (errno == ENOMEM && rstack_push_value(rs, 1) == 0)
        visited |= V(2, 1);
    else
        return visited |= V(4, 1); // To nie powinno się wykonać.

    errno = 0;
    if (rstack_write(OUTPUT_FILE, rs) == 0)
        visited |= V(1, 2);
    else if (errno != 0 && rstack_write(OUTPUT_FILE, rs) == 0)
        visited |= V(2, 2);
    else
        return visited |= V(4, 2); // To nie powinno się wykonać.

    rstack_delete(rs);

    return visited;
}

// Testuje reakcję implementacji na niepowodzenie alokacji pamięci.
static int memory_test(unsigned long (*test_function)(void)) {
    memory_test_data_t *mtd = get_memory_test_data();

    unsigned fail = 0, pass = 0;
    mtd->call_total = 0;
    mtd->fail_counter = 1;
    while (fail < 3 && pass < 3) {
        mtd->call_counter = 0;
        mtd->alloc_counter = 0;
        mtd->free_counter = 0;
        mtd->function_name = NULL;
        unsigned long visited_points = test_function();
        if (mtd->alloc_counter != mtd->free_counter ||
            (visited_points & 0444444444444444444444UL) != 0) {
            fprintf(stderr,
                    "fail_counter %u, alloc_counter %u, free_counter %u, "
                    "function_name %s, visited_point %lo\n",
                    mtd->fail_counter,
                    mtd->alloc_counter,
                    mtd->free_counter,
                    mtd->function_name,
                    visited_points);
            ++fail;
        }
        if (mtd->function_name == NULL)
            ++pass;
        else
            pass = 0;
        mtd->fail_counter++;
    }

    return mtd->call_total > 0 && fail == 0 ? PASS : FAIL;
}

// Testuje reakcję implementacji na niepowodzenie alokacji pamięci.
int main(void) {
    memory_tests_check();
    return memory_test(alloc_fail_test);
}
