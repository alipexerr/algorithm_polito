#ifndef PLANNER_PLANNER_IO_H
#define PLANNER_PLANNER_IO_H

#include "planner/planner.h"

#include <stdio.h>

#define TASK_LABEL_CAPACITY 64

/** Commands understood by the supplied application loop. */
typedef enum {
    PLANNER_COMMAND_INVALID,
    PLANNER_COMMAND_LIST,
    PLANNER_COMMAND_FIND,
    PLANNER_COMMAND_ADD,
    PLANNER_COMMAND_REMOVE,
    PLANNER_COMMAND_COMPLETE,
    PLANNER_COMMAND_UNDO,
    PLANNER_COMMAND_STOP,
    PLANNER_COMMAND_END_OF_INPUT
} PlannerCommandType;

/** Parsed data for one interactive command. */
typedef struct {
    PlannerCommandType type;
    char identifier[TASK_IDENTIFIER_CAPACITY];
    int due_date;
    char label[TASK_LABEL_CAPACITY];
} PlannerCommand;

/**
 * @brief Loads tasks without relying on their order in the input stream.
 *
 * On failure every task read so far is released and the planner is left empty.
 * The caller retains ownership of @p input_file.
 *
 * @param[in,out] planner Planner that receives the loaded tasks.
 * @param[in] input_file Stream containing the initial task records.
 * @return PLANNER_OK on success, PLANNER_DUPLICATE for a repeated identifier,
 *         PLANNER_INVALID_DATA for malformed input, or PLANNER_NO_MEMORY when
 *         an allocation fails.
 */
PlannerResult planner_io_load_tasks(Planner *planner, FILE *input_file);

/**
 * @brief Prints the command menu once, before the application loop starts.
 */
void planner_io_print_menu(void);

/**
 * @brief Reads and parses one command after printing the short prompt.
 *
 * @param[out] command Parsed command and its arguments.
 * @return The parsed command type, including invalid and end-of-input results.
 */
PlannerCommandType planner_io_read_command(PlannerCommand *command);

/**
 * @brief Prints one task using the documented output format.
 *
 * A NULL pointer produces the documented not-found diagnostic.
 *
 * @param[in] task Task to print, or NULL.
 */
void planner_io_print_task(const Task *task);

/**
 * @brief Prints all pending tasks using the documented order and format.
 *
 * @param[in] planner Planner whose pending tasks are printed.
 */
void planner_io_print_tasks(const Planner *planner);

/**
 * @brief Prints the user-facing outcome of a modifying command.
 *
 * @param[in] command_type Command whose outcome is reported.
 * @param[in] result Result returned by the planner operation.
 */
void planner_io_print_result(PlannerCommandType command_type,
                             PlannerResult result);

/**
 * @brief Prints the diagnostic used for a malformed or unknown command.
 */
void planner_io_print_invalid_command(void);

#endif
