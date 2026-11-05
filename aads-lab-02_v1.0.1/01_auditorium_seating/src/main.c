#include "seating_plan.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_LINE_CAPACITY 256

/* Opens the auditorium input file, reporting an error on failure. */
static FILE *open_input_file(const char *filename);

/* Displays the available commands once before the command loop. */
static void print_command_menu(void);

/* Reads one command, returning 0 only when standard input ends. */
static int read_command(char command[INPUT_LINE_CAPACITY], int *group_size);

/* Prints the row and seats assigned by one successful reservation. */
static void print_successful_reservation(const SeatingPlan *seating_plan,
                                         int group_size,
                                         int selected_row_index,
                                         int first_seat_index);

/* Prints one reservation request that could not be satisfied. */
static void print_unavailable_reservation(int group_size);

/* Prints a centered seating map using fixed-width seat slots. */
static void print_centered_map(const SeatingPlan *seating_plan,
                               SeatState *const seating_map[],
                               int map_column_count);

/* Builds, prints, and releases the map; returns 0 if it cannot be built. */
static int print_seating_plan(const SeatingPlan *seating_plan);

/* Processes commands until stop or end of input, returning 0 on failure. */
static int run_command_loop(SeatingPlan *seating_plan);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: auditorium_seating <input_file>\n");
        return EXIT_FAILURE;
    }

    FILE *input_file = open_input_file(argv[1]);
    if (input_file == NULL) {
        return EXIT_FAILURE;
    }

    SeatingPlan seating_plan = {NULL, NULL, 0};
    if (!load_seating_plan(input_file, &seating_plan)) {
        fprintf(stderr, "Invalid auditorium data.\n");
        fclose(input_file);
        return EXIT_FAILURE;
    }
    fclose(input_file);

    if (!print_seating_plan(&seating_plan) ||
        !run_command_loop(&seating_plan)) {
        destroy_seating_plan(&seating_plan);
        return EXIT_FAILURE;
    }

    destroy_seating_plan(&seating_plan);
    return EXIT_SUCCESS;
}

static FILE *open_input_file(const char *filename) {
    FILE *input_file = fopen(filename, "r");

    if (input_file == NULL) {
        fprintf(stderr, "Cannot open input file: %s\n", filename);
    }
    return input_file;
}

static void print_command_menu(void) {
    printf("\nAvailable commands:\n"
           "  reserve <group_size> reserve consecutive seats for a group\n"
           "  stop                 end the session\n");
}

static int read_command(char command[INPUT_LINE_CAPACITY], int *group_size) {
    char command_line[INPUT_LINE_CAPACITY];

    printf("Command? ");
    fflush(stdout);

    if (fgets(command_line, INPUT_LINE_CAPACITY, stdin) == NULL) {
        return 0;
    }

    command[0] = '\0';
    *group_size = 0;

    int command_parts = sscanf(command_line, "%s", command);
    if (command_parts == 1 && strcmp(command, "stop") == 0) {
        return 1;
    }

    int reserve_parts = sscanf(command_line, "%s %d", command, group_size);
    if (reserve_parts == 2 && strcmp(command, "reserve") == 0 &&
        *group_size >= MIN_GROUP_SIZE && *group_size <= MAX_GROUP_SIZE) {
        return 1;
    }

    command[0] = '\0';
    return 1;
}

static void print_successful_reservation(const SeatingPlan *seating_plan,
                                         int group_size,
                                         int selected_row_index,
                                         int first_seat_index) {
    printf("RESERVATION\n");
    printf("Requested seats: %d\n", group_size);
    char row_label = seating_plan->rows[selected_row_index].label;
    printf("Assigned row: %c\n", row_label);
    printf("Assigned seats:");
    for (int seat_offset = 0; seat_offset < group_size; seat_offset++) {
        printf(" %c%d", row_label, first_seat_index + seat_offset + 1);
    }
    printf("\n");
}

static void print_unavailable_reservation(int group_size) {
    printf("RESERVATION\n");
    printf("Requested seats: %d\n", group_size);
    printf("Result: not available\n");
}

static void print_centered_map(const SeatingPlan *seating_plan,
                               SeatState *const seating_map[],
                               int map_column_count) {
    printf("\nSEATING MAP\n");
    printf("Legend: .=available X=occupied\n");

    for (int row_index = 0; row_index < seating_plan->row_count; row_index++) {
        int physical_seat_number = 1;
        printf("%c |", seating_plan->rows[row_index].label);

        for (int map_column_index = 0; map_column_index < map_column_count;
             map_column_index++) {
            SeatState seat_state = seating_map[row_index][map_column_index];
            if (seat_state == NO_SEAT) {
                printf("    ");
            } else {
                char seat_symbol = seat_state == SEAT_AVAILABLE ? '.' : 'X';
                printf(" %2d%c", physical_seat_number, seat_symbol);
                physical_seat_number++;
            }
        }
        printf(" |\n");
    }
}

static int print_seating_plan(const SeatingPlan *seating_plan) {
    int map_column_count = 0;
    SeatState **seating_map =
        build_centered_seating_map(seating_plan, &map_column_count);
    if (seating_map == NULL) {
        fprintf(stderr, "Cannot build seating map.\n");
        return 0;
    }

    print_centered_map(seating_plan, seating_map, map_column_count);
    destroy_seating_map(seating_map, seating_plan->row_count);
    return 1;
}

static int run_command_loop(SeatingPlan *seating_plan) {
    char command[INPUT_LINE_CAPACITY];
    int group_size = 0;

    print_command_menu();
    while (read_command(command, &group_size)) {
        if (strcmp(command, "stop") == 0) {
            break;
        }

        if (strcmp(command, "reserve") != 0) {
            printf("Invalid command.\n");
            continue;
        }

        int selected_row_index = NO_SELECTION;
        int first_seat_index = NO_SELECTION;
        if (reserve_group_seats(seating_plan,
                                group_size,
                                &selected_row_index,
                                &first_seat_index)) {
            print_successful_reservation(seating_plan,
                                         group_size,
                                         selected_row_index,
                                         first_seat_index);
        } else {
            print_unavailable_reservation(group_size);
        }
        if (!print_seating_plan(seating_plan)) {
            return 0;
        }
    }
    return 1;
}
