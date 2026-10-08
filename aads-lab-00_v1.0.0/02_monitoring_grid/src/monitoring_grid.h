#ifndef MONITORING_GRID_H
#define MONITORING_GRID_H

#include <stdio.h>

#define MAX_ROWS 50
#define MAX_COLUMNS 50

/**
 * @brief Supported directions for an aligned anomaly sequence.
 *
 * Values are contiguous starting from zero. Their numeric order defines the
 * report order and the indices used by the direction lookup tables.
 * DIRECTION_COUNT is a count and sentinel, not a valid direction.
 */
typedef enum {
    HORIZONTAL = 0,      /**< From left to right. */
    VERTICAL,            /**< From top to bottom. */
    DIAGONAL_DOWN_RIGHT, /**< Toward the next row and column. */
    DIAGONAL_DOWN_LEFT,  /**< Toward the next row and previous column. */
    DIRECTION_COUNT      /**< Count of supported directions and sentinel. */
} SequenceDirection;

/** Upper bound: at most one sequence start per cell and direction. */
#define MAX_ANOMALY_SEQUENCES (MAX_ROWS * MAX_COLUMNS * DIRECTION_COUNT)

/**
 * @brief Validated monitoring grid and its reporting threshold.
 */
typedef struct {
    char cells[MAX_ROWS][MAX_COLUMNS + 1]; /**< Grid rows terminated by '\0'. */
    int row_count;                         /**< Number of populated rows. */
    int column_count;                      /**< Number of cells per row. */
    int minimum_sequence_length;           /**< Smallest reported segment. */
} MonitoringGrid;

/**
 * @brief Endpoints, direction, and length of one maximal anomaly sequence.
 */
typedef struct {
    int start_row_index;         /**< Zero-based starting row. */
    int start_column_index;      /**< Zero-based starting column. */
    int end_row_index;           /**< Zero-based ending row. */
    int end_column_index;        /**< Zero-based ending column. */
    int length;                  /**< Number of cells in the segment. */
    SequenceDirection direction; /**< Alignment direction. */
} AnomalySequence;

/**
 * @brief Loads and validates a complete monitoring grid.
 *
 * @param[in] input_file Stream positioned at the grid dimensions.
 * @param[out] monitoring_grid Structure receiving the loaded grid.
 * @return 1 on success, or 0 after invalid input or a read failure.
 */
int load_monitoring_grid(FILE *input_file, MonitoringGrid *monitoring_grid);

/**
 * @brief Finds all maximal anomalous sequences in the grid.
 *
 * @param[in] monitoring_grid Grid to analyze.
 * @param[out] sequences Array receiving the detected sequences.
 * @return Number of sequences written to the output array.
 */
int find_anomaly_sequences(const MonitoringGrid *monitoring_grid,
                           AnomalySequence sequences[MAX_ANOMALY_SEQUENCES]);

/**
 * @brief Returns the printable name of a sequence direction.
 *
 * @param[in] direction Direction to describe.
 * @return Constant direction name, or `UNKNOWN_DIRECTION` when invalid.
 */
const char *get_sequence_direction_name(SequenceDirection direction);

#endif
