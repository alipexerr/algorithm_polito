#include "seating_plan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int load_seating_plan(FILE *input_file, SeatingPlan *seating_plan) {
  if (seating_plan == NULL || input_file == NULL)
    return 0;

  seating_plan->rows = NULL;
  seating_plan->seat_states = NULL;
  seating_plan->row_count = 0;

  if (fscanf(input_file, "%d", &seating_plan->row_count) != 1 ||
      seating_plan->row_count <= 0 || seating_plan->row_count > MAX_ROW_COUNT)
    return 0;

  seating_plan->rows = malloc(sizeof(SeatRow) * seating_plan->row_count);
  seating_plan->seat_states =
      calloc(seating_plan->row_count, sizeof(SeatState *));
  if (seating_plan->rows == NULL || seating_plan->seat_states == NULL)
    goto failure;

  for (int i = 0; i < seating_plan->row_count; i++) {
    char row_label;
    int seat_count;
    if (fscanf(input_file, "%c %d", &row_label, &seat_count) != 2 ||
        seat_count <= 0 || seat_count > MAX_SEAT_COUNT)
      goto failure;

    seating_plan->rows[i].label = row_label;
    seating_plan->rows[i].seat_count = seat_count;
    seating_plan->seat_states[i] = malloc(sizeof(SeatState) * seat_count);
    if (seating_plan->seat_states[i] == NULL)
      goto failure;

    for (int j = 0; j < seat_count; j++) {
      int state;
      if (fscanf(input_file, "%d", &state) != 1 ||
          (state != SEAT_AVAILABLE && state != SEAT_OCCUPIED))
        goto failure;
      seating_plan->seat_states[i][j] = (SeatState)state;
    }
  }

  return 1;

failure:
  if (seating_plan->seat_states != NULL) {
    for (int i = 0; i < seating_plan->row_count; i++)
      free(seating_plan->seat_states[i]);
  }
  free(seating_plan->seat_states);
  free(seating_plan->rows);
  seating_plan->rows = NULL;
  seating_plan->seat_states = NULL;
  seating_plan->row_count = 0;
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
