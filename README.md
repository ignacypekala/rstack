# librstack.so
A C library for operations on recursive stacks - stacks capable of holding
unsigned 64-bit integers and other stacks.

Handles cyclic garbage with trial deletion based heavily on an algorithm
featured in "Concurrent Cycle Collection in Reference Counted Systems" by David
F. Bacon and V.T. Rajan (ECOOP 2001).

## Supported Operations

- `rstack_t* rstack_new()`

- `int       rstack_push_value(rstack_t *rs, uint64_t value)`

- `int       rstack_push_rstack(rstack_t *rs1, rstack_t *rs2)`

- `void      rstack_pop(rstack_t *rs)`

- `bool      rstack_empty(rstack_t *rs)`:

    Whether the stack contains a numeric value.

- `void      rstack_delete(rstack_t *rs)`:

    Decrements the stack's reference counter, freeing the orphaned objects
    recursively. Performs trial deletion to eliminate cyclic grabage.


- `result_t  rstack_front(rstack_t *rs)`:
    
    Returns the topmost numeric value if such exists. Detects empty cycles and
    steps over them.

    

- `int       rstack_write(char const *path, rstack_t *rs)`:

    Writes all the numbers reachable from the top of the stack. Stops upon
    detecting a cycle.

- `rstack_t* rstack_read(char const *path)`

## Test suite
The test suite was developed externally under
[rstack_tests](https://github.com/ignacypekala/rstack_tests) to be shared with
other students.

It has been vendored in under `tests/`. 

Run the tests:
```bash
cd tests
./test-all.sh
```

## Attribution
The project was originally developed for AKSO (Computer architecture and
operating systems) course at MIMUW (Faculty of Mathematics, Informatics and
Mechanics of University of Warsaw).

All of the code, apart from:

    - files provided in the assignment located under `provided/`,

    - files outlined in `tests/README.md` as vendored

are my original work.
