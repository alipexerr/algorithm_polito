#ifndef PLANNER_PLANNER_H
#define PLANNER_PLANNER_H

#define TASK_IDENTIFIER_CAPACITY 16

/** A pending or completed activity. */
typedef struct task {
    char identifier[TASK_IDENTIFIER_CAPACITY];
    int due_date;
    char *label;
    struct task *next;
} Task;

/**
 * The complete task planner.
 *
 * Pending tasks form an ordered singly linked list. Completed tasks form the
 * optional LIFO history used by `complete` and `undo`.
 */
typedef struct {
    Task *pending_tasks;
    Task *completed_tasks;
    int pending_task_count;
} Planner;

/** Possible outcomes of planner and loading operations. */
typedef enum {
    PLANNER_OK,
    PLANNER_NOT_FOUND,
    PLANNER_DUPLICATE,
    PLANNER_NO_HISTORY,
    PLANNER_INVALID_DATA,
    PLANNER_NO_MEMORY
} PlannerResult;

/**
 * @brief Adds one task while preserving the required order.
 *
 * The planner stores its own exact-size copy of @p label.
 *
 * @param[in,out] planner Planner to modify.
 * @param[in] identifier Unique task identifier.
 * @param[in] due_date Due date represented as YYYYMMDD.
 * @param[in] label Non-empty task label.
 * @return PLANNER_OK, PLANNER_DUPLICATE, or PLANNER_NO_MEMORY.
 */
PlannerResult planner_add_task(Planner *planner,
                               const char *identifier,
                               int due_date,
                               const char *label);

/**
 * @brief Finds a pending task by its exact identifier.
 *
 * @param[in] planner Planner to search.
 * @param[in] identifier Identifier to find.
 * @return The matching task, or NULL when it is absent.
 */
const Task *planner_find_task(const Planner *planner, const char *identifier);

/**
 * @brief Removes and releases one pending task.
 *
 * @param[in,out] planner Planner to modify.
 * @param[in] identifier Identifier of the task to remove.
 * @return PLANNER_OK or PLANNER_NOT_FOUND.
 */
PlannerResult planner_remove_task(Planner *planner, const char *identifier);

/**
 * @brief Moves one pending task to the completion history.
 *
 * This operation is part of the optional extension.
 *
 * @param[in,out] planner Planner to modify.
 * @param[in] identifier Identifier of the task to complete.
 * @return PLANNER_OK or PLANNER_NOT_FOUND.
 */
PlannerResult planner_complete_task(Planner *planner, const char *identifier);

/**
 * @brief Restores the most recently completed task.
 *
 * This operation is part of the optional extension.
 *
 * @param[in,out] planner Planner to modify.
 * @return PLANNER_OK, PLANNER_NO_HISTORY, or PLANNER_DUPLICATE.
 */
PlannerResult planner_undo_completion(Planner *planner);

/**
 * @brief Releases every task and label owned by a planner.
 *
 * The planner object itself belongs to the caller and is reset to an empty
 * state rather than released.
 *
 * @param[in,out] planner Planner to clear.
 */
void planner_destroy(Planner *planner);

#endif
