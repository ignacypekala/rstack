/*
 * Implements the rstack_delete function with trial deletion.
 * The algorithm is based heavily on the
 * "Concurrent Cycle Collection in Reference Counted Systems" published by 
 * David F. Bacon and V.T. Rajan (ECOOP 2001).
 *
 * Note:
 * Because containers can push other containers onto themselves, 
 * the whole structure forms a web of references. Throughout 
 * these comments, I often refer to the stacks as "nodes" and 
 * the overall structure as a "graph".
 */

#include "types.h"
#include <stdlib.h>

/*
 * Performs the first phase of trial deletion by removing internal references.
 * Traverses the subgraph of the given stack, decrementing the reference
 * counters of its container children and marking visited nodes as UNDER_TRIAL.
 */
static void gc_simulate_deletion(rstack_t *stack) {
    rstack_container_t *container = stack->as.container;

    container->gc_state = UNDER_TRIAL;
    for (size_t i = 0; i < container->size; i++) {
        rstack_t *element = container->array[i];

        if (element->type == CONTAINER) {
            rstack_container_t *element_container = element->as.container;

            element_container->references--;
            if (element_container->gc_state != UNDER_TRIAL) {
                gc_simulate_deletion(element);
            }
        }
    }
}

/*
 * Performs the second phase of trial deletion to identify garbage cycles.
 * Examines UNDER_TRIAL nodes: if a node has a positive reference counter
 * (indicating external references exist), it initiates a rescue. Otherwise,
 * it marks the node as PROVISIONALLY_DEAD and continues scanning its
 * descendants.
 */
static void gc_rescue(rstack_t *stack) {
    rstack_container_t *container = stack->as.container;
    container->gc_state = RESCUED;

    for (size_t i = 0; i < container->size; i++) {
        rstack_t *element = container->array[i];

        if (element->type == CONTAINER) {
            rstack_container_t *element_container = element->as.container;
            element_container->references++;

            if (element_container->gc_state != RESCUED) {
                gc_rescue(element);
            }
        }
    }
}

/*
 * Traverses through the graph in search of stacks with a positive reference
 * counter, those are rescued, while all the unrescued ones receive a state of
 * PROVISIONALLY_DEAD (as rescue missions may overwite this).
 */
static void gc_scan_for_rescue(rstack_t *stack) {
    rstack_container_t *container = stack->as.container;

    if (container->gc_state != UNDER_TRIAL) return;

    if (container->references > 0) {
        gc_rescue(stack);
    } else {
        container->gc_state = PROVISIONALLY_DEAD;
        for (size_t i = 0; i < container->size; i++) {
            rstack_t *element = container->array[i];

            if (element->type == CONTAINER) {
                gc_scan_for_rescue(element);
            }
        }
    }
};

/*
 * Finalizes the rescue operation for nodes falsely identified as garbage.
 * Traverses the subgraph and restores the state of any RESCUED nodes 
 * back to NORMAL.
 */
static void gc_resurrect_rescued(rstack_t *stack) {
    rstack_container_t *container = stack->as.container;
    
    if (container->gc_state == RESCUED) {
        container->gc_state = NORMAL;
        for (size_t i = 0; i < container->size; i++) {
            rstack_t *element = stack->as.container->array[i];

            if (element->type == CONTAINER) {
                gc_resurrect_rescued(element);
            }
        }
    }
}

/*
 * Identifies nodes for reclamation by traversing the subgraph of provisionally
 * dead containers and linking them into an intrusive list. Marks nodes as 
 * DEAD to prevent redundant visits and immediately deallocates numerical 
 * leaf nodes.
 * - stack: The root node from which the dead-node search originates.
 * - head: A pointer to the current tail of the intrusive linked list.
 */
static void gc_mark_dead(rstack_t *stack, rstack_t **head) {
    rstack_container_t *container = stack->as.container;

    if (container->gc_state == PROVISIONALLY_DEAD) {
        container->gc_state = DEAD;

        (*head)->as.container->gc_next = stack;
        *head = stack;

        for (size_t i = 0; i < container->size; i++) {
            rstack_t *element = stack->as.container->array[i];

            if (element->type == CONTAINER) {
                gc_mark_dead(element, head);
            } else {
                free(element);
            }
        }
    }
}

/*
 * Finalizes garbage collection by traversing the intrusive linked list 
 * of DEAD nodes. Deallocates internal storage arrays, container structures, 
 * and the rstack objects themselves.
 * - start: The first element of the intrusive linked list to be reclaimed.
 */
static void gc_reclaim(rstack_t *start) {
    rstack_t *next = start;
    while (next != nullptr) {
        rstack_t *stack = next;
        rstack_container_t *container = stack->as.container;
        next = container->gc_next;

        free(container->array);
        free(container);
        free(stack);
    }
}

/*
 * Initiates the release of the given rstack object.
 * Numerical objects are deallocated immediately. Container objects have
 * their reference counters decremented. If a container's reference count
 * drops to zero, it is recursively destroyed. If it remains positive,
 * the trial deletion algorithm is executed to detect and clean up isolated
 * cyclic reference graphs.
 */
void rstack_delete(rstack_t *rs) {
    if (rs == nullptr) return;

    if (rs->type == NUMBER) {
        free(rs);
    } else {
        rstack_container_t *container = rs->as.container;
        if (--container->references <= 0) {
            for (size_t i = 0; i < container->size; i++) {
                rstack_t *element = container->array[i];
                rstack_delete(element);
            }
            free(container->array);
            free(container);
            free(rs);
        } else {
            gc_simulate_deletion(rs);
            gc_scan_for_rescue(rs);
            gc_resurrect_rescued(rs);

            rstack_container_t sentinel_container = { 
                .gc_next = nullptr 
            };
            rstack_t sentinel = { 
                .as.container = &sentinel_container 
            };
            rstack_t *head = &sentinel;
            gc_mark_dead(rs, &head);
            gc_reclaim(sentinel.as.container->gc_next);
        }
    }
}

