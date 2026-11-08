#include "seating_plan.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int load_seating_plan(FILE *input_file, SeatingPlan *seating_plan) {
  if (seating_plan == NULL || input_file == NULL)
    return 0;

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
    if (fscanf(input_file, " %c %d", &row_label, &seat_count) != 2 ||
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
                          
  if (seating_plan == NULL || group_size < MIN_GROUP_SIZE || group_size > MAX_GROUP_SIZE ||
      selected_row_index == NULL || first_seat_index == NULL)
    return 0;
  *selected_row_index = NO_SELECTION;
  *first_seat_index = NO_SELECTION;
  for (int i = 0; i < seating_plan->row_count; i++) {
    int seat_count = seating_plan->rows[i].seat_count;
    if (seat_count < group_size)
      continue;
    int half_seat = seat_count / 2;
    int* seat_distance = malloc(sizeof(int) * (seat_count - group_size + 1));
    if (seat_distance == NULL)
      return 0;
    for (int j = 0; j <= seat_count - group_size; j++) {
      int available = 1;
      for (int k = 0; k < group_size; k++) {
        if (seating_plan->seat_states[i][j + k] != SEAT_AVAILABLE) {
          available = 0;
          break;
        }
      }
      if (available) {
        seat_distance[j] = abs((j + group_size / 2) - half_seat);
      } else {
        seat_distance[j] = -1;
      }
    }
    for (int j = 0; j <= seat_count - group_size; j++) {
      if (seat_distance[j] != -1) {
        if (*selected_row_index == NO_SELECTION || seat_distance[j] < seat_distance[*first_seat_index] || 
            (seat_distance[j] == seat_distance[*first_seat_index] && j < *first_seat_index)) {
          *selected_row_index = i;
          *first_seat_index = j;
        }
      }
    }
    for (int k = 0; k < group_size; k++) {
      if (*selected_row_index != NO_SELECTION)
        seating_plan->seat_states[*selected_row_index][*first_seat_index + k] = SEAT_OCCUPIED;
    }
    free(seat_distance); 
    return 1; 
    
  } 
  return 0;
}

SeatState **build_centered_seating_map(const SeatingPlan *seating_plan,
                                       int *map_column_count) {
  int max_seat_count = 0;
  if (seating_plan == NULL || map_column_count == NULL || seating_plan->row_count <= 0)
    return NULL;
  for (int i = 0; i < seating_plan->row_count; i++) {
    if (seating_plan->rows[i].seat_count > max_seat_count)
      max_seat_count = seating_plan->rows[i].seat_count;
  }
  *map_column_count = max_seat_count;
  SeatState **seating_map = malloc(sizeof(SeatState *) * seating_plan->row_count);
  if (seating_map == NULL)  
    return NULL;
  for (int i = 0; i < seating_plan->row_count; i++) {
    seating_map[i] = malloc(sizeof(SeatState) * max_seat_count);
    if (seating_map[i] == NULL) {
      for (int j = 0; j < i; j++)
        free(seating_map[j]);
      free(seating_map);
      return NULL;
    }
    int seat_count = seating_plan->rows[i].seat_count;
    int total_padding = max_seat_count - seat_count;
    int left_padding = total_padding / 2;
    int right_padding = total_padding - left_padding;
    for (int j = 0; j < left_padding; j++){
      seating_map[i][j] = NO_SEAT;
    }
    for (int j = 0; j < seat_count; j++) {
      seating_map[i][left_padding + j] = seating_plan->seat_states[i][j];
    }
    for (int j = 0; j < right_padding; j++) {
      seating_map[i][left_padding + seat_count + j] = NO_SEAT;
    }    
  }
  return seating_map;
}

void destroy_seating_plan(SeatingPlan *seating_plan) {
  if (seating_plan == NULL)
    return;
  for (int i = 0; i < seating_plan->row_count; i++) {
    free(seating_plan->seat_states[i]);
  }
  free(seating_plan->seat_states);
  free(seating_plan->rows);
  seating_plan->rows = NULL;
  seating_plan->seat_states = NULL;
  seating_plan->row_count = 0;
}

void destroy_seating_map(SeatState *seating_map[], int map_row_count) {
  if (seating_map == NULL || map_row_count <= 0)
    return;
  for (int i = 0; i < map_row_count; i++) {
    free(seating_map[i]);
  }
  free(seating_map);
}
