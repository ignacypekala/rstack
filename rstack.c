// #include <stdlib.h>
#include <inttypes.h>
#include <stddef.h>

typedef struct rstack rstack_t;

typedef struct {
    bool flag; // To pole mówi, czy pole value zawiera wynik.
    uint64_t value; // W tym polu jest właściwy wynik.
} result_t;

rstack_t* rstack_new()
{
    return nullptr;
}
void rstack_delete(rstack_t* rs) { }
int rstack_push_value(rstack_t* rs, uint64_t value)
{
    return 0;
}
int rstack_push_rstack(rstack_t* rs1, rstack_t* rs2)
{
    return 0;
}
void rstack_pop(rstack_t* rs) { }
bool rstack_empty(rstack_t* rs)
{
    return false;
}
result_t rstack_front(rstack_t* rs)
{
    result_t t = { };
    return t;
}
rstack_t* rstack_read(char const* path)
{
    return nullptr;
}
int rstack_write(char const* path, rstack_t* rs)
{
    return 0;
}
