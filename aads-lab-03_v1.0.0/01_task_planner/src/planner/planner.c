#include "planner/planner.h"

#include <stddef.h>

PlannerResult planner_add_task(Planner *planner,
                               const char *identifier,
                               int due_date,
                               const char *label) {
    /* TODO: allocate and insert a new owned task in the required order. */
    return PLANNER_NO_MEMORY;
}

const Task *planner_find_task(const Planner *planner, const char *identifier) {
    /* TODO: search the pending list for the exact identifier. */
    return NULL;
}

PlannerResult planner_remove_task(Planner *planner, const char *identifier) {
    /* TODO: unlink and release the matching pending task. */
    return PLANNER_NOT_FOUND;
}

PlannerResult planner_complete_task(Planner *planner, const char *identifier) {
    /* OPTIONAL TODO: move the matching task to the completion history. */
    return PLANNER_NOT_FOUND;
}

PlannerResult planner_undo_completion(Planner *planner) {
    /* OPTIONAL TODO: restore the most recently completed task. */
    return PLANNER_NO_HISTORY;
}

void planner_destroy(Planner *planner) {
    /* TODO: release pending tasks and any optional completion history. */
}
