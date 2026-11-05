#include "planner/planner.h"
#include "planner/planner_io.h"

#include <stdio.h>
#include <stdlib.h>

/* Dispatches commands until input ends or the user requests termination. */
static int run_command_loop(Planner *planner);

int main(int argument_count, char *arguments[]) {
    if (argument_count != 2) {
        fprintf(stderr, "Usage: task_planner <input_file>\n");
        return EXIT_FAILURE;
    }

    FILE *input_file = fopen(arguments[1], "r");
    if (input_file == NULL) {
        fprintf(stderr, "Cannot open task file.\n");
        return EXIT_FAILURE;
    }

    Planner planner = {0};
    PlannerResult load_result = planner_io_load_tasks(&planner, input_file);
    fclose(input_file);
    if (load_result != PLANNER_OK) {
        fprintf(stderr, "Cannot load task file.\n");
        return EXIT_FAILURE;
    }

    planner_io_print_menu();
    int exit_status = run_command_loop(&planner);
    planner_destroy(&planner);
    return exit_status;
}

static int run_command_loop(Planner *planner) {
    PlannerCommand command;

    while (planner_io_read_command(&command) != PLANNER_COMMAND_END_OF_INPUT) {
        switch (command.type) {
        case PLANNER_COMMAND_LIST:
            planner_io_print_tasks(planner);
            break;
        case PLANNER_COMMAND_FIND: {
            const Task *task = planner_find_task(planner, command.identifier);
            planner_io_print_task(task);
            break;
        }
        case PLANNER_COMMAND_ADD: {
            PlannerResult result = planner_add_task(planner,
                                                    command.identifier,
                                                    command.due_date,
                                                    command.label);
            planner_io_print_result(command.type, result);
            if (result == PLANNER_NO_MEMORY) {
                return EXIT_FAILURE;
            }
            break;
        }
        case PLANNER_COMMAND_REMOVE: {
            PlannerResult result =
                planner_remove_task(planner, command.identifier);
            planner_io_print_result(command.type, result);
            break;
        }
        case PLANNER_COMMAND_COMPLETE: {
            PlannerResult result =
                planner_complete_task(planner, command.identifier);
            planner_io_print_result(command.type, result);
            break;
        }
        case PLANNER_COMMAND_UNDO: {
            PlannerResult result = planner_undo_completion(planner);
            planner_io_print_result(command.type, result);
            break;
        }
        case PLANNER_COMMAND_STOP:
            return EXIT_SUCCESS;
        case PLANNER_COMMAND_INVALID:
            planner_io_print_invalid_command();
            break;
        case PLANNER_COMMAND_END_OF_INPUT:
            break;
        }
    }
    return EXIT_SUCCESS;
}
