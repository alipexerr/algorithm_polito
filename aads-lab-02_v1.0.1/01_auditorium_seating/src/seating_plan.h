#ifndef SEATING_PLAN_H
#define SEATING_PLAN_H

#include <stdio.h>

#define MAX_ROW_COUNT 26
#define MAX_SEAT_COUNT 99
#define MIN_GROUP_SIZE 1
#define MAX_GROUP_SIZE 99
#define NO_SELECTION (-1)

/** @brief Represents a physical seat or an empty position in a seating map. */
typedef enum {
    NO_SEAT = -1,
    SEAT_AVAILABLE = 0,
    SEAT_OCCUPIED = 1
} SeatState;

/** @brief Describes one physical row without owning its seat-state array. */
typedef struct {
    char label;
    int seat_count;
} SeatRow;

/** @brief Owns row descriptions and one allocated seat-state array per row. */
typedef struct {
    SeatRow *rows;
    SeatState **seat_states;
    int row_count;
} SeatingPlan;

/**
 * @brief Loads the complete seating plan from an open stream.
 *
 * The supplied plan must be initialized empty and must not own allocations.
 * Its fields are overwritten without releasing previous contents. Each
 * physical row receives an independently allocated seat-state array with
 * exactly the required number of elements. On failure, every allocation
 * acquired by this call is released and the supplied plan is left empty. The
 * caller owns a successful result and releases it with destroy_seating_plan.
 *
 * @param[in] input_file Stream positioned at the row count.
 * @param[out] seating_plan Empty destination receiving the loaded seating plan
 *                          on success; left empty on failure.
 * @return 1 on success, or 0 after invalid input, a read error, or an
 *         allocation failure.
 */
int load_seating_plan(FILE *input_file, SeatingPlan *seating_plan);

/**
 * @brief Reserves the required block in the first suitable physical row.
 *
 * Within that row, selects the block closest to the row center. Equal-distance
 * blocks are resolved by the lower starting seat number. When a block is
 * found, all its seats are marked as occupied.
 * The seating plan must be valid, group_size must be between MIN_GROUP_SIZE
 * and MAX_GROUP_SIZE, and both output pointers must be valid.
 *
 * @param[in] seating_plan Current seating plan.
 * @param[in] group_size Number of consecutive available seats required.
 * @param[out] selected_row_index Zero-based selected row index, or
 *                                NO_SELECTION if unavailable.
 * @param[out] first_seat_index Zero-based first seat index, or NO_SELECTION if
 *                              unavailable.
 * @return 1 when the reservation succeeds, otherwise 0 without modifying the
 *         seating plan.
 */
int reserve_group_seats(SeatingPlan *seating_plan,
                        int group_size,
                        int *selected_row_index,
                        int *first_seat_index);

/**
 * @brief Builds an independently allocated centered seating map.
 *
 * The result has one row per physical row and one column per seat in the
 * longest row. NO_SEAT marks a padding position, SEAT_AVAILABLE an available
 * physical seat, and SEAT_OCCUPIED an occupied physical seat. Odd padding
 * places its extra position on the right.
 *
 * @param[in] seating_plan Seating plan containing the physical rows.
 * @param[out] map_column_count Map width on success; zero on failure.
 * @return The independently allocated seating map owned by the caller, or NULL
 *         after an allocation failure. Release a successful result with
 *         destroy_seating_map.
 */
SeatState **build_centered_seating_map(const SeatingPlan *seating_plan,
                                       int *map_column_count);

/**
 * @brief Releases all data owned by a seating plan and leaves it empty.
 *
 * @param[in,out] seating_plan Seating plan to release; NULL is accepted.
 */
void destroy_seating_plan(SeatingPlan *seating_plan);

/**
 * @brief Releases a centered seating map.
 *
 * @param[in] seating_map Centered seating map; NULL is accepted.
 * @param[in] map_row_count Number of rows in the map.
 */
void destroy_seating_map(SeatState *seating_map[], int map_row_count);

#endif
