#include "seating_plan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int load_seating_plan(FILE *input_file, SeatingPlan *seating_plan) {
  /* TODO: validate, allocate, and load all physical rows. */
  if (seating_plan == NULL || input_file == NULL)
    return 0;
  if (fscanf(input_file, "%d\n", &seating_plan->row_count) != 1 || seating_plan->row_count <= 0 || seating_plan->row_count > MAX_ROW_COUNT)
    return 0;

  for (int i = 0; i < seating_plan->row_count; i++) {
    char row_label;
    int seat_count;
    SeatRow *row;
    SeatState **seat_states;
    seat_states = malloc(sizeof(SeatState *) * seating_plan->row_count);
    if (seat_states == NULL)
      return 0;
    if (fscanf(input_file, "%c %d\n", &row_label, &seat_count) != 2 || seat_count <= 0 || seat_count > MAX_SEAT_COUNT)
      return 0;
    (*row).label = row_label;
    (*row).seat_count = seat_count;
    seating_plan->rows[i] = *row;
    *seat_states = malloc(sizeof(SeatState) * seat_count);
    if (*seat_states == NULL)
      return 0;
    for (int j = 0; j < seat_count; j++) {
      int seat_state;
      SeatState state;
      if (fscanf(input_file, "%d", &seat_state) != 1)
        return 0;
      state = (SeatState)seat_state;
      (*seat_states)[j] = state;
    }
    seating_plan->seat_states = seat_states;
    return 1;
  }

  return 0;
}

int reserve_group_seats(SeatingPlan *seating_plan, int group_size,
                        int *selected_row_index, int *first_seat_index) {
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
