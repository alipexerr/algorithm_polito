#include "planner/planner_io.h"

#include <string.h>

#define INPUT_LINE_CAPACITY 256
#define COMMAND_CAPACITY 16

/* Reports whether a line contains no non-whitespace characters. */
static int line_is_blank(const char *line);
/* Parses and adds one task record. */
static PlannerResult load_task(Planner *planner, const char *line);
/* Reports whether a command line contains only its command name. */
static int command_has_no_arguments(const char *line);

PlannerResult planner_io_load_tasks(Planner *planner, FILE *input_file) {
    char line[INPUT_LINE_CAPACITY];
    PlannerResult result = PLANNER_OK;

    planner_destroy(planner);
    while (fgets(line, sizeof(line), input_file) != NULL) {
        if (line_is_blank(line)) {
            continue;
        }
        result = load_task(planner, line);
        if (result != PLANNER_OK) {
            break;
        }
    }
    if (ferror(input_file)) {
        result = PLANNER_INVALID_DATA;
    }
    if (result != PLANNER_OK) {
        planner_destroy(planner);
    }
    return result;
}

void planner_io_print_menu(void) {
    printf("Available commands:\n"
           "  list                             show all pending tasks\n"
           "  find <identifier>                show one task\n"
           "  add <identifier> <date> <label>  add one pending task\n"
           "  remove <identifier>              remove one pending task\n"
           "  complete <identifier>            complete one task (optional)\n"
           "  undo                             undo the latest completion "
           "(optional)\n"
           "  stop                             end the program\n");
}

PlannerCommandType planner_io_read_command(PlannerCommand *command) {
    char line[INPUT_LINE_CAPACITY];
    char command_name[COMMAND_CAPACITY];
    char extra = '\0';

    printf("Command? ");
    fflush(stdout);
    if (fgets(line, sizeof(line), stdin) == NULL) {
        command->type = PLANNER_COMMAND_END_OF_INPUT;
        return command->type;
    }
    if (sscanf(line, " %s", command_name) != 1) {
        command->type = PLANNER_COMMAND_INVALID;
        return command->type;
    }

    if (strcmp(command_name, "list") == 0 && command_has_no_arguments(line)) {
        command->type = PLANNER_COMMAND_LIST;
    } else if (strcmp(command_name, "find") == 0 && sscanf(line,
                                                           " %s %s %c",
                                                           command_name,
                                                           command->identifier,
                                                           &extra) == 2) {
        command->type = PLANNER_COMMAND_FIND;
    } else if (strcmp(command_name, "add") == 0 && sscanf(line,
                                                          " %s %s %d %s %c",
                                                          command_name,
                                                          command->identifier,
                                                          &command->due_date,
                                                          command->label,
                                                          &extra) == 4) {
        command->type = PLANNER_COMMAND_ADD;
    } else if (
        strcmp(command_name, "remove") == 0 &&
        sscanf(line, " %s %s %c", command_name, command->identifier, &extra) ==
            2) {
        command->type = PLANNER_COMMAND_REMOVE;
    } else if (
        strcmp(command_name, "complete") == 0 &&
        sscanf(line, " %s %s %c", command_name, command->identifier, &extra) ==
            2) {
        command->type = PLANNER_COMMAND_COMPLETE;
    } else if (strcmp(command_name, "undo") == 0 &&
               command_has_no_arguments(line)) {
        command->type = PLANNER_COMMAND_UNDO;
    } else if (strcmp(command_name, "stop") == 0 &&
               command_has_no_arguments(line)) {
        command->type = PLANNER_COMMAND_STOP;
    } else {
        command->type = PLANNER_COMMAND_INVALID;
    }
    return command->type;
}

void planner_io_print_task(const Task *task) {
    if (task == NULL) {
        printf("Task not found.\n");
        return;
    }
    printf("TASK\n"
           "ID: %s\n"
           "Due date: %08d\n"
           "Label: %s\n",
           task->identifier,
           task->due_date,
           task->label);
}

void planner_io_print_tasks(const Planner *planner) {
    printf("PENDING TASKS %d\n", planner->pending_task_count);
    if (planner->pending_tasks == NULL) {
        return;
    }

    printf("ID | DUE DATE | LABEL\n");
    for (const Task *task = planner->pending_tasks; task != NULL;
         task = task->next) {
        printf("%s | %08d | %s\n",
               task->identifier,
               task->due_date,
               task->label);
    }
}

void planner_io_print_result(PlannerCommandType command_type,
                             PlannerResult result) {
    switch (command_type) {
    case PLANNER_COMMAND_ADD:
        if (result == PLANNER_OK) {
            printf("Task added.\n");
        } else if (result == PLANNER_DUPLICATE) {
            printf("Task already exists.\n");
        } else {
            fprintf(stderr, "Cannot allocate task.\n");
        }
        break;
    case PLANNER_COMMAND_REMOVE:
        printf(result == PLANNER_OK ? "Task removed.\n" : "Task not found.\n");
        break;
    case PLANNER_COMMAND_COMPLETE:
        printf(result == PLANNER_OK ? "Task completed.\n"
                                    : "Task not found.\n");
        break;
    case PLANNER_COMMAND_UNDO:
        if (result == PLANNER_OK) {
            printf("Completion undone.\n");
        } else if (result == PLANNER_DUPLICATE) {
            printf("Task already exists.\n");
        } else {
            printf("Nothing to undo.\n");
        }
        break;
    default:
        break;
    }
}

void planner_io_print_invalid_command(void) {
    printf("Unknown command.\n");
}

static int line_is_blank(const char *line) {
    char character = '\0';
    return sscanf(line, " %c", &character) != 1;
}

static PlannerResult load_task(Planner *planner, const char *line) {
    char identifier[TASK_IDENTIFIER_CAPACITY];
    char label[TASK_LABEL_CAPACITY];
    char extra = '\0';
    int due_date = 0;

    if (sscanf(line, " %s %d %s %c", identifier, &due_date, label, &extra) !=
        3) {
        return PLANNER_INVALID_DATA;
    }
    return planner_add_task(planner, identifier, due_date, label);
}

static int command_has_no_arguments(const char *line) {
    char command_name[COMMAND_CAPACITY];
    char extra = '\0';
    return sscanf(line, " %s %c", command_name, &extra) == 1;
}
