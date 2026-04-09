#ifndef TEST_MACROS
#define TEST_MACROS

#include <inttypes.h>
#include <stdio.h>

#define PASS 0
#define FAIL 1
#define WRONG_TEST 2
#define VALGRIND_ERROR 3

#define REPORT(...)                                                            \
    do {                                                                       \
        fprintf(stderr, "%s:%d: ", __FILE__, __LINE__);                        \
        fprintf(stderr, __VA_ARGS__);                                          \
        fprintf(stderr, "\n");                                                 \
    } while (0)

// Oblicza liczbę elementów tablicy x.
#define SIZE(x) (sizeof x / sizeof x[0])

#define ASSERT(f)                                                              \
    do {                                                                       \
        if (!(f)) {                                                            \
            REPORT("Assertion failed");                                        \
            return FAIL;                                                       \
        }                                                                      \
    } while (0)

#define ASSERT_RESULT(c, f, ...)                                               \
    do {                                                                       \
        result_t r = c;                                                        \
        if (r.flag != (f)) {                                                   \
            REPORT("Result assertion failed, expected flag == %s",             \
                   f ? "true" : "false");                                      \
            return FAIL;                                                       \
        }                                                                      \
        if ((f) && r.value != __VA_ARGS__ - 0) {                               \
            REPORT("Result assertion failed, expected value == %" PRIu64,      \
                   (uint64_t) __VA_ARGS__);                                     \
            return FAIL;                                                       \
        }                                                                      \
    } while (0)

#define CHECK_IF_NO_ERROR(f)                                                   \
    do {                                                                       \
        if ((f) != 0)                                                          \
            return FAIL;                                                       \
    } while (0)

#define PRINT_U64(v) printf("%" PRIu64 "\n", v);

#endif // TEST_MACROS
