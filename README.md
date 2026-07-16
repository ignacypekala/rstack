# librstack.so
A C library for operations on recursive stacks - stacks capable of holding
unsigned 64-bit integers and other stacks.

Handles cyclic garbage with a trial deletion approach based heavily on an 
algorithm featured in "Concurrent Cycle Collection in Reference Counted
Systems" by David F. Bacon and V.T. Rajan (ECOOP 2001).

## Supported Operations

```c
rstack_t* rstack_new();

int       rstack_push_value(rstack_t *rs, uint64_t value);

int       rstack_push_rstack(rstack_t *rs1, rstack_t *rs2);

void      rstack_pop(rstack_t *rs);

// Whether the stack is empty (does not contain a numeric value at the top).
bool      rstack_empty(rstack_t *rs);

// Decrements the stack's reference counter, freeing the orphaned objects
// recursively. Performs trial deletion to eliminate cyclic garbage.
void      rstack_delete(rstack_t *rs)

// Returns the topmost numeric value if such exists. Detects empty cycles and
// steps over them.
result_t  rstack_front(rstack_t *rs)
    
// Writes all the numbers reachable from the top of the stack. Stops upon
// detecting a cycle.
int       rstack_write(char const *path, rstack_t *rs)

rstack_t* rstack_read(char const *path)
```

## How to run
### Requirements
- GCC
- Make

For the tests:
- Bash
- Valgrind (optional, only needed for performing memory checks)

### Compile
```bash
make librstack.so
```

### Run the tests
```bash
cd tests
./test-all.sh
```

## Test suite
The test suite was developed externally under
[rstack_tests](https://github.com/ignacypekala/rstack_tests) to be shared with
other students. It has therefore been vendored in under `tests/`. 

## Attribution
The project was originally developed for AKSO (Computer Architecture and
Operating Systems) course at MIMUW (Faculty of Mathematics, Informatics and
Mechanics of University of Warsaw).

All of the code, apart from:

- files provided in the assignment, namely: `file_four.in` and files located under `provided/`,

- files outlined in `tests/README.md` as vendored-in

is my original work and is available under the [MIT](./LICENSE) license.
