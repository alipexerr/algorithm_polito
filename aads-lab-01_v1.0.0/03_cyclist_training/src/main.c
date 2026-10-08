#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_LINE_CAPACITY 256

/**
 * @brief Displays the available commands once.
 */
static void print_menu(void);

/**
 * @brief Displays the command prompt and reads one command.
 *
 * The cyclist name is read only when it follows the command on the same line.
 * Otherwise the returned cyclist name is the empty string.
 *
 * @param[out] command Array with INPUT_LINE_CAPACITY elements.
 * @param[out] cyclist_name Array with INPUT_LINE_CAPACITY elements.
 * @return 1 after reading a line, or 0 when standard input ends.
 */
static int read_command(char command[INPUT_LINE_CAPACITY],
                        char cyclist_name[INPUT_LINE_CAPACITY]);

int main(int argc, char **argv) {
    /* TODO: check the arguments and load the cyclist data from argv[1]. */

    char command[INPUT_LINE_CAPACITY];
    char cyclist_name[INPUT_LINE_CAPACITY];

    print_menu();
    while (read_command(command, cyclist_name)) {
        /* TODO: implement the list, details, and best commands. */
        if (strcmp(command, "stop") == 0) {
            break;
        } else {
            printf("Unknown command.\n");
        }
    }

    return EXIT_SUCCESS;
}

static void print_menu(void) {
    printf("Available commands:\n"
           "  list\n"
           "  details <name>\n"
           "  best\n"
           "  stop\n");
}

static int read_command(char command[INPUT_LINE_CAPACITY],
                        char cyclist_name[INPUT_LINE_CAPACITY]) {
    char input_line[INPUT_LINE_CAPACITY];

    printf("Command? ");
    fflush(stdout);

    if (fgets(input_line, INPUT_LINE_CAPACITY, stdin) == NULL) {
        return 0;
    }

    command[0] = '\0';
    cyclist_name[0] = '\0';
    sscanf(input_line, "%s", command);

    if (strcmp(command, "details") == 0) {
        sscanf(input_line, "%s %s", command, cyclist_name);
    }

    return 1;
}
