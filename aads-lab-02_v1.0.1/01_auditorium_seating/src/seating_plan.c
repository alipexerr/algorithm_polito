#include "seating_plan.h"

int load_seating_plan(FILE *input_file, SeatingPlan *seating_plan) {
    /* TODO: validate, allocate, and load all physical rows. */
    return 0;
}

int reserve_group_seats(SeatingPlan *seating_plan,
                        int group_size,
                        int *selected_row_index,
                        int *first_seat_index) {
    /* TODO: find, select, and occupy the required consecutive block. */
    return 0;
}

SeatState **build_centered_seating_map(const SeatingPlan *seating_plan,
                                       int *map_column_count) {
    /* TODO: build the padded map without changing physical seat numbering. */
    return NULL;
}

void destroy_seating_plan(SeatingPlan *seating_plan) {
    /* TODO: release all seat rows and row descriptions. */
}

void destroy_seating_map(SeatState *seating_map[], int map_row_count) {
    /* TODO: release every seating-map row and then its row-pointer array. */
}
