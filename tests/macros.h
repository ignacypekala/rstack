#ifndef TEST_MACROS
#define TEST_MACROS

#include <inttypes.h>
#include <stdio.h> // IWYU pragma: keep (this stops clangd from reporting this include as unnecessary)

#define PASS 0
#define FAIL 1

#define OUTPUT_FILE "test.fout"

#define REPORT(...)                                                            \
    do {                                                                       \
        fprintf(stderr, "%s:%d (%s): ", __FILE__, __LINE__, __func__);         \
        fprintf(stderr, __VA_ARGS__);                                          \
        fprintf(stderr, "\n");                                                 \
    } while (0)

#define SIZE(x) (sizeof x / sizeof x[0])

#define ASSERT(f)                                                              \
    do {                                                                       \
        if (!(f)) {                                                            \
            REPORT("Assertion failed: %s", #f);                                \
            return FAIL;                                                       \
        }                                                                      \
    } while (0)

// If __VA_ARGS__ has a value, use it. Otherwise, fallback to 0.
#define GET_EXPECTED(...) __VA_OPT__(__VA_ARGS__) __VA_OPT__(+) 0UL

#define ASSERT_RESULT(c, f, ...)                                               \
    do {                                                                       \
        result_t r = c;                                                        \
        if (r.flag != (f)) {                                                   \
            REPORT("Result assertion failed, %s.flag is \"%s\" but should be " \
                   "\"%s\".",                                                  \
                   #c,                                                         \
                   r.flag ? "true" : "false",                                  \
                   #f);                                                        \
            return FAIL;                                                       \
        }                                                                      \
        if ((f) && r.value != __VA_ARGS__ - 0) {                               \
            REPORT("Result assertion failed, %s.value is %" PRIu64             \
                   " but should be %" PRIu64 ".",                              \
                   #c,                                                         \
                   r.value,                                                    \
                   GET_EXPECTED());                                            \
            return FAIL;                                                       \
        }                                                                      \
    } while (0)

#define NO_ERROR(f)                                                            \
    do {                                                                       \
        if ((f) != 0) {                                                        \
            REPORT(                                                            \
              "Expected %s to exit with no error but it returned %d", #f, f);  \
            return FAIL;                                                       \
        }                                                                      \
    } while (0)
#define CHECK_IF_NO_ERROR(f) NO_ERROR(f);

#define PRINT_U64(v) printf("%" PRIu64 "\n", v);

#define TEST_FILE(name) "test_" name ".fout"

#endif // TEST_MACROS
