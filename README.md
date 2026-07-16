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
- GCC (C23 standard)
- Make

For the tests:
- Bash
- Valgrind (optional, only needed for performing memory checks)

### Example usage:
```c
// main.c
#include "rstack.h"
#include <stdio.h>

int main() {
    rstack_t *my_stack = rstack_new();

    rstack_push_value(my_stack, 42);

    result_t res = rstack_front(my_stack);
    if (res.flag) {
        printf("Top value: %lu\n", res.value);
    }
    
    rstack_delete(my_stack);
    return 0;
}
```
Copy file `provided/rstack.h` to the root of your project.

Compile the library with:
```bash
make librstack.so
```

Link your program with:
```bash
gcc main.c -L. -lrstack -Wl,-rpath=. -o my_program
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

## Technical Highlights & Reflections
At first glance, building librstack.so seemed like a straightforward exercise
in writing clean, robust C code. However, it quickly unfolded into a deep dive
into the complexities of garbage collection and memory management. 

While I've worked with C before, developing this as my first standalone library
made it the most demanding C project I've tackled to date.

- **Trial Deletion**: 

    Implementing garbage collection proved to be a tricky challenge, sending me
    down a rabbit hole of reading 50-year-old computer science papers to fully
    grasp the theory before writing the code.

    At its core, the process relies on a trial deletion mechanism consisting of
    multiple subsequent and nested breadth-first search (BFS) lookups to
    accurately isolate and clean up cyclic references.

    When an object is dereferenced (rstack_delete call), but its reference
    counter hasn't fallen to zero, it is considered a suspect. Such an element is
    then subject to trial deletion to detect cyclic garbage.

    1. First, a breadth-first traversal is performed to decrement reference
       counters (RC) in its entire sub-graph (subjects are marked as
       **UNDER_TRIAL**). This simulates deletion of the entire sub-graph. 

    2. A subsequent BFS marks elements with (RC = 0) as **PROVISIONALLY_DEAD** in
       search of externally-referenced, alive elements (RC > 0). Upon finding
       such an element, a rescue mission is performed.
    
       2a. A rescued element is marked as **RESCUED** and all the elements in its
           sub-graph have their RCs incremented by a nested BFS traversal.

    3. Next, rescued elements are resurrected by resetting their states to
       **NORMAL**, while the remaining unrescued elements are marked as
       **DEAD**.

    4. Finally, all the **DEAD** elements are put in an intrusive linked-list
       (gc_next field).

    5. Afterwards, the garbage is freed and the process ends.

- **Library Architecture:** 
    
    Building a library from scratch gave me hands-on experience with the
    complete lifecycle of a C project, learning exactly how they are
    structured, compiled, linked, bundled, and distributed.  

- **Strict Compliance:**

    The implementation required meticulous attention to detail, specifically
    ensuring that all operations complied with strict requirements and returned
    the precise `errno` values expected.

- **Testing Strategy:**

    To ensure the library's reliability, I designed a custom test framework
    handling stdout, stderr, file I/O operations, and strict memory validation.
    It successfully executes a comprehensive suite of manual edge-case tests,
    baseline examples, and ported auto-generated tests. For a detailed
    breakdown of the testing approach, refer to the documentation in the `tests/`
    directory.

## Attribution
The project was originally developed for AKSO (Computer Architecture and
Operating Systems) course at MIMUW (Faculty of Mathematics, Informatics and
Mechanics of University of Warsaw).

All of the code, apart from:

- files provided in the assignment, namely: `file_four.in` and files located under `provided/`,

- files outlined in `tests/README.md` as vendored-in

is my original work and is available under the [MIT](./LICENSE) license.
