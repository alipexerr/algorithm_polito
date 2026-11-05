#include "seating_plan.h"
#include "support/allocation_tracker.h"
#include "unity.h"

#include <stdio.h>
#include <stdlib.h>

static FILE *stream_from_text(const char *text) {
    FILE *stream = tmpfile();

    TEST_ASSERT_NOT_NULL(stream);
    TEST_ASSERT_GREATER_OR_EQUAL(0, fputs(text, stream));
    rewind(stream);
    return stream;
}

void setUp(void) {
}

void tearDown(void) {
}

static void test_load_seating_plan_preserves_rows_and_states(void) {
    FILE *input = stream_from_text("3\nA 2 0 1\nB 4 1 0 0 1\nC 1 0\n");
    SeatingPlan seating_plan = {NULL, NULL, 0};

    TEST_ASSERT_TRUE(load_seating_plan(input, &seating_plan));
    TEST_ASSERT_EQUAL_INT(3, seating_plan.row_count);
    TEST_ASSERT_EQUAL_CHAR('A', seating_plan.rows[0].label);
    TEST_ASSERT_EQUAL_INT(2, seating_plan.rows[0].seat_count);
    TEST_ASSERT_EQUAL_INT(SEAT_AVAILABLE, seating_plan.seat_states[0][0]);
    TEST_ASSERT_EQUAL_INT(SEAT_OCCUPIED, seating_plan.seat_states[0][1]);
    TEST_ASSERT_EQUAL_CHAR('B', seating_plan.rows[1].label);
    TEST_ASSERT_EQUAL_INT(4, seating_plan.rows[1].seat_count);
    TEST_ASSERT_TRUE(seating_plan.seat_states[0] !=
                     seating_plan.seat_states[1]);
    TEST_ASSERT_TRUE(seating_plan.seat_states[1] !=
                     seating_plan.seat_states[2]);

    destroy_seating_plan(&seating_plan);
    fclose(input);
}

static void test_load_seating_plan_with_partial_row_leaves_reusable_empty_plan(
    void) {
    FILE *partial_input = stream_from_text("2\nA 2 0 1\nB 3 0 1\n");
    SeatingPlan seating_plan = {NULL, NULL, 0};

    TEST_ASSERT_FALSE(load_seating_plan(partial_input, &seating_plan));
    TEST_ASSERT_NULL(seating_plan.rows);
    TEST_ASSERT_NULL(seating_plan.seat_states);
    TEST_ASSERT_EQUAL_INT(0, seating_plan.row_count);
    fclose(partial_input);

    FILE *valid_input = stream_from_text("1\nC 1 0\n");
    TEST_ASSERT_TRUE(load_seating_plan(valid_input, &seating_plan));
    TEST_ASSERT_EQUAL_INT(1, seating_plan.row_count);

    free(seating_plan.seat_states[0]);
    free(seating_plan.seat_states);
    free(seating_plan.rows);
    fclose(valid_input);
}

static void test_reserve_group_seats_with_cross_row_space_returns_unavailable(
    void) {
    SeatRow rows[] = {{'A', 2}, {'B', 2}};
    SeatState first_row[] = {SEAT_AVAILABLE, SEAT_AVAILABLE};
    SeatState second_row[] = {SEAT_AVAILABLE, SEAT_AVAILABLE};
    SeatState *seat_states[] = {first_row, second_row};
    SeatingPlan seating_plan = {rows, seat_states, 2};
    int selected_row_index = 99;
    int first_seat_index = 99;

    TEST_ASSERT_FALSE(reserve_group_seats(&seating_plan,
                                          3,
                                          &selected_row_index,
                                          &first_seat_index));
    TEST_ASSERT_EQUAL_INT(NO_SELECTION, selected_row_index);
    TEST_ASSERT_EQUAL_INT(NO_SELECTION, first_seat_index);
    TEST_ASSERT_EQUAL_INT(SEAT_AVAILABLE, first_row[0]);
    TEST_ASSERT_EQUAL_INT(SEAT_AVAILABLE, first_row[1]);
    TEST_ASSERT_EQUAL_INT(SEAT_AVAILABLE, second_row[0]);
    TEST_ASSERT_EQUAL_INT(SEAT_AVAILABLE, second_row[1]);
}

static void test_reserve_group_seats_with_multiple_rows_uses_first_suitable(
    void) {
    SeatRow rows[] = {{'A', 5}, {'B', 7}};
    SeatState first_row[] = {SEAT_OCCUPIED,
                             SEAT_AVAILABLE,
                             SEAT_AVAILABLE,
                             SEAT_OCCUPIED,
                             SEAT_AVAILABLE};
    SeatState second_row[] = {SEAT_AVAILABLE,
                              SEAT_AVAILABLE,
                              SEAT_AVAILABLE,
                              SEAT_AVAILABLE,
                              SEAT_AVAILABLE,
                              SEAT_AVAILABLE,
                              SEAT_AVAILABLE};
    SeatState *seat_states[] = {first_row, second_row};
    SeatingPlan seating_plan = {rows, seat_states, 2};
    int selected_row_index = NO_SELECTION;
    int first_seat_index = NO_SELECTION;

    TEST_ASSERT_TRUE(reserve_group_seats(&seating_plan,
                                         2,
                                         &selected_row_index,
                                         &first_seat_index));
    TEST_ASSERT_EQUAL_INT(0, selected_row_index);
    TEST_ASSERT_EQUAL_INT(1, first_seat_index);
    TEST_ASSERT_EQUAL_INT(SEAT_OCCUPIED, first_row[1]);
    TEST_ASSERT_EQUAL_INT(SEAT_OCCUPIED, first_row[2]);
}

static void test_reserve_group_seats_with_equal_distance_uses_lower_seat(void) {
    SeatRow rows[] = {{'A', 8}};
    SeatState only_row[] = {SEAT_AVAILABLE,
                            SEAT_AVAILABLE,
                            SEAT_OCCUPIED,
                            SEAT_OCCUPIED,
                            SEAT_OCCUPIED,
                            SEAT_OCCUPIED,
                            SEAT_AVAILABLE,
                            SEAT_AVAILABLE};
    SeatState *seat_states[] = {only_row};
    SeatingPlan seating_plan = {rows, seat_states, 1};
    int selected_row_index = NO_SELECTION;
    int first_seat_index = NO_SELECTION;

    TEST_ASSERT_TRUE(reserve_group_seats(&seating_plan,
                                         2,
                                         &selected_row_index,
                                         &first_seat_index));
    TEST_ASSERT_EQUAL_INT(0, selected_row_index);
    TEST_ASSERT_EQUAL_INT(0, first_seat_index);
    TEST_ASSERT_EQUAL_INT(SEAT_OCCUPIED, only_row[0]);
    TEST_ASSERT_EQUAL_INT(SEAT_OCCUPIED, only_row[1]);
}

static void test_reserve_group_seats_with_multiple_blocks_uses_closest(void) {
    SeatRow rows[] = {{'A', 9}};
    SeatState only_row[] = {SEAT_AVAILABLE,
                            SEAT_AVAILABLE,
                            SEAT_AVAILABLE,
                            SEAT_OCCUPIED,
                            SEAT_AVAILABLE,
                            SEAT_AVAILABLE,
                            SEAT_AVAILABLE,
                            SEAT_OCCUPIED,
                            SEAT_OCCUPIED};
    SeatState *seat_states[] = {only_row};
    SeatingPlan seating_plan = {rows, seat_states, 1};
    int selected_row_index = NO_SELECTION;
    int first_seat_index = NO_SELECTION;

    TEST_ASSERT_TRUE(reserve_group_seats(&seating_plan,
                                         3,
                                         &selected_row_index,
                                         &first_seat_index));
    TEST_ASSERT_EQUAL_INT(0, selected_row_index);
    TEST_ASSERT_EQUAL_INT(4, first_seat_index);
    TEST_ASSERT_EQUAL_INT(SEAT_OCCUPIED, only_row[4]);
    TEST_ASSERT_EQUAL_INT(SEAT_OCCUPIED, only_row[5]);
    TEST_ASSERT_EQUAL_INT(SEAT_OCCUPIED, only_row[6]);
}

static void test_reserve_group_seats_marks_only_selected_block(void) {
    SeatRow rows[] = {{'A', 5}};
    SeatState row_seat_states[] = {SEAT_AVAILABLE,
                                   SEAT_AVAILABLE,
                                   SEAT_AVAILABLE,
                                   SEAT_AVAILABLE,
                                   SEAT_AVAILABLE};
    SeatState *seat_states[] = {row_seat_states};
    SeatingPlan seating_plan = {rows, seat_states, 1};
    int selected_row_index = NO_SELECTION;
    int first_seat_index = NO_SELECTION;

    TEST_ASSERT_TRUE(reserve_group_seats(&seating_plan,
                                         3,
                                         &selected_row_index,
                                         &first_seat_index));
    TEST_ASSERT_EQUAL_INT(0, selected_row_index);
    TEST_ASSERT_EQUAL_INT(1, first_seat_index);

    SeatState expected_states[] = {SEAT_AVAILABLE,
                                   SEAT_OCCUPIED,
                                   SEAT_OCCUPIED,
                                   SEAT_OCCUPIED,
                                   SEAT_AVAILABLE};
    TEST_ASSERT_EQUAL_INT_ARRAY(expected_states, row_seat_states, 5);
}

static void
test_build_centered_seating_map_with_even_and_odd_padding_splits_both(void) {
    SeatRow rows[] = {{'A', 3}, {'B', 4}, {'C', 6}};
    SeatState row_a[] = {SEAT_AVAILABLE, SEAT_OCCUPIED, SEAT_AVAILABLE};
    SeatState row_b[] = {SEAT_OCCUPIED,
                         SEAT_AVAILABLE,
                         SEAT_AVAILABLE,
                         SEAT_OCCUPIED};
    SeatState row_c[] = {SEAT_AVAILABLE,
                         SEAT_AVAILABLE,
                         SEAT_OCCUPIED,
                         SEAT_OCCUPIED,
                         SEAT_AVAILABLE,
                         SEAT_AVAILABLE};
    SeatState *seat_states[] = {row_a, row_b, row_c};
    SeatingPlan seating_plan = {rows, seat_states, 3};
    int map_column_count = 0;

    SeatState **seating_map =
        build_centered_seating_map(&seating_plan, &map_column_count);

    TEST_ASSERT_NOT_NULL(seating_map);
    TEST_ASSERT_EQUAL_INT(6, map_column_count);

    TEST_ASSERT_EQUAL_INT(NO_SEAT, seating_map[0][0]);
    TEST_ASSERT_EQUAL_INT(SEAT_AVAILABLE, seating_map[0][1]);
    TEST_ASSERT_EQUAL_INT(SEAT_OCCUPIED, seating_map[0][2]);
    TEST_ASSERT_EQUAL_INT(SEAT_AVAILABLE, seating_map[0][3]);
    TEST_ASSERT_EQUAL_INT(NO_SEAT, seating_map[0][4]);
    TEST_ASSERT_EQUAL_INT(NO_SEAT, seating_map[0][5]);

    TEST_ASSERT_EQUAL_INT(NO_SEAT, seating_map[1][0]);
    TEST_ASSERT_EQUAL_INT(SEAT_OCCUPIED, seating_map[1][1]);
    TEST_ASSERT_EQUAL_INT(SEAT_OCCUPIED, seating_map[1][4]);
    TEST_ASSERT_EQUAL_INT(NO_SEAT, seating_map[1][5]);

    TEST_ASSERT_TRUE(seating_map[0] != row_a);
    TEST_ASSERT_TRUE(seating_map[1] != row_b);
    destroy_seating_map(seating_map, 3);
}

static void test_destroy_seating_plan_with_allocated_data_leaves_empty(void) {
    SeatingPlan seating_plan = {NULL, NULL, 2};
    seating_plan.rows = malloc(2 * sizeof(SeatRow));
    seating_plan.seat_states = calloc(2, sizeof(SeatState *));
    TEST_ASSERT_NOT_NULL(seating_plan.rows);
    TEST_ASSERT_NOT_NULL(seating_plan.seat_states);

    seating_plan.seat_states[0] = malloc(2 * sizeof(SeatState));
    seating_plan.seat_states[1] = malloc(3 * sizeof(SeatState));
    TEST_ASSERT_NOT_NULL(seating_plan.seat_states[0]);
    TEST_ASSERT_NOT_NULL(seating_plan.seat_states[1]);

    destroy_seating_plan(&seating_plan);

    TEST_ASSERT_NULL(seating_plan.rows);
    TEST_ASSERT_NULL(seating_plan.seat_states);
    TEST_ASSERT_EQUAL_INT(0, seating_plan.row_count);

    /* A second release, an empty plan, and NULL are all accepted. */
    destroy_seating_plan(&seating_plan);
    destroy_seating_plan(NULL);
    destroy_seating_map(NULL, 0);
}

static void test_every_allocated_block_is_released(void) {
    FILE *input = stream_from_text("3\nA 2 0 1\nB 4 1 0 0 1\nC 1 0\n");
    int blocks_before = outstanding_allocation_count();
    SeatingPlan seating_plan = {NULL, NULL, 0};

    TEST_ASSERT_TRUE(load_seating_plan(input, &seating_plan));
    int map_column_count = 0;
    SeatState **seating_map =
        build_centered_seating_map(&seating_plan, &map_column_count);
    TEST_ASSERT_NOT_NULL(seating_map);

    destroy_seating_map(seating_map, seating_plan.row_count);
    destroy_seating_plan(&seating_plan);

    int leaked_blocks = outstanding_allocation_count() - blocks_before;
    fclose(input);
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        0,
        leaked_blocks,
        "memory blocks are still allocated: every malloc, calloc and "
        "realloc needs a matching free");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_load_seating_plan_preserves_rows_and_states);
    RUN_TEST(
        test_load_seating_plan_with_partial_row_leaves_reusable_empty_plan);
    RUN_TEST(test_reserve_group_seats_with_cross_row_space_returns_unavailable);
    RUN_TEST(test_reserve_group_seats_with_multiple_rows_uses_first_suitable);
    RUN_TEST(test_reserve_group_seats_with_equal_distance_uses_lower_seat);
    RUN_TEST(test_reserve_group_seats_with_multiple_blocks_uses_closest);
    RUN_TEST(test_reserve_group_seats_marks_only_selected_block);
    RUN_TEST(
        test_build_centered_seating_map_with_even_and_odd_padding_splits_both);
    RUN_TEST(test_destroy_seating_plan_with_allocated_data_leaves_empty);
    RUN_TEST(test_every_allocated_block_is_released);
    return UNITY_END();
}
