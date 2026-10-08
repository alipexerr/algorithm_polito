#include "monitoring_grid.h"

#include <string.h>

#define INPUT_LINE_CAPACITY 256

typedef struct {
    int row_offset;
    int column_offset;
} DirectionStep;

/*
 * Movement vector for each direction. SequenceDirection values index this
 * table, and designated initializers make that correspondence explicit.
 */
static const DirectionStep DIRECTION_STEPS[DIRECTION_COUNT] = {
    [HORIZONTAL] = {0, 1},
    [VERTICAL] = {1, 0},
    [DIAGONAL_DOWN_RIGHT] = {1, 1},
    [DIAGONAL_DOWN_LEFT] = {1, -1},
};

/* Printable label associated with each SequenceDirection value. */
static const char *const DIRECTION_NAMES[DIRECTION_COUNT] = {
    [HORIZONTAL] = "HORIZONTAL",
    [VERTICAL] = "VERTICAL",
    [DIAGONAL_DOWN_RIGHT] = "DIAGONAL_DOWN_RIGHT",
    [DIAGONAL_DOWN_LEFT] = "DIAGONAL_DOWN_LEFT",
};

static int larger_dimension(int row_count, int column_count);
static int dimensions_are_valid(const MonitoringGrid *monitoring_grid);
static int row_contains_only_valid_cells(const char *grid_row,
                                         int column_count);
static int complete_line_was_read(const char *input_line, FILE *input_file);
static int read_grid_row(FILE *input_file,
                         char grid_row[MAX_COLUMNS + 1],
                         int column_count);
static int read_grid_rows(FILE *input_file, MonitoringGrid *monitoring_grid);
static int is_inside_grid(const MonitoringGrid *monitoring_grid,
                          int row_index,
                          int column_index);
static int is_anomalous_cell(const MonitoringGrid *monitoring_grid,
                             int row_index,
                             int column_index);
static int is_sequence_start(const MonitoringGrid *monitoring_grid,
                             int row_index,
                             int column_index,
                             SequenceDirection direction);
static AnomalySequence follow_sequence(const MonitoringGrid *monitoring_grid,
                                       int start_row_index,
                                       int start_column_index,
                                       SequenceDirection direction);
static void collect_cell_sequences(const MonitoringGrid *monitoring_grid,
                                   int row_index,
                                   int column_index,
                                   AnomalySequence sequences[],
                                   int *sequence_count);

int load_monitoring_grid(FILE *input_file, MonitoringGrid *monitoring_grid) {
    if (input_file == NULL || monitoring_grid == NULL) {
        return 0;
    }

    char input_line[INPUT_LINE_CAPACITY];
    if (fgets(input_line, INPUT_LINE_CAPACITY, input_file) == NULL ||
        !complete_line_was_read(input_line, input_file)) {
        return 0;
    }

    MonitoringGrid parsed_grid = {0};
    int parsed_fields = sscanf(input_line,
                               "%d %d %d",
                               &parsed_grid.row_count,
                               &parsed_grid.column_count,
                               &parsed_grid.minimum_sequence_length);

    if (parsed_fields != 3 || !dimensions_are_valid(&parsed_grid) ||
        !read_grid_rows(input_file, &parsed_grid)) {
        return 0;
    }

    *monitoring_grid = parsed_grid;
    return 1;
}

/*
 * Row-major traversal and enum order produce the required report order.
 * Following only maximal-sequence starts avoids retraversing subsegments and
 * keeps the search in O(rows * columns) time for four fixed directions.
 */
int find_anomaly_sequences(const MonitoringGrid *monitoring_grid,
                           AnomalySequence sequences[MAX_ANOMALY_SEQUENCES]) {
    int sequence_count = 0;

    for (int row_index = 0; row_index < monitoring_grid->row_count;
         ++row_index) {
        for (int column_index = 0; column_index < monitoring_grid->column_count;
             ++column_index) {
            if (is_anomalous_cell(monitoring_grid, row_index, column_index)) {
                collect_cell_sequences(monitoring_grid,
                                       row_index,
                                       column_index,
                                       sequences,
                                       &sequence_count);
            }
        }
    }

    return sequence_count;
}

const char *get_sequence_direction_name(SequenceDirection direction) {
    switch (direction) {
    case HORIZONTAL:
        return DIRECTION_NAMES[HORIZONTAL];
    case VERTICAL:
        return DIRECTION_NAMES[VERTICAL];
    case DIAGONAL_DOWN_RIGHT:
        return DIRECTION_NAMES[DIAGONAL_DOWN_RIGHT];
    case DIAGONAL_DOWN_LEFT:
        return DIRECTION_NAMES[DIAGONAL_DOWN_LEFT];
    default:
        return "UNKNOWN_DIRECTION";
    }
}

static int larger_dimension(int row_count, int column_count) {
    if (row_count > column_count) {
        return row_count;
    }

    return column_count;
}

static int dimensions_are_valid(const MonitoringGrid *monitoring_grid) {
    int maximum_sequence_length =
        larger_dimension(monitoring_grid->row_count,
                         monitoring_grid->column_count);

    return monitoring_grid->row_count >= 1 &&
           monitoring_grid->row_count <= MAX_ROWS &&
           monitoring_grid->column_count >= 1 &&
           monitoring_grid->column_count <= MAX_COLUMNS &&
           monitoring_grid->minimum_sequence_length >= 2 &&
           monitoring_grid->minimum_sequence_length <= maximum_sequence_length;
}

static int row_contains_only_valid_cells(const char *grid_row,
                                         int column_count) {
    if ((int) strlen(grid_row) != column_count) {
        return 0;
    }

    for (int column_index = 0; column_index < column_count; ++column_index) {
        if (grid_row[column_index] != '.' && grid_row[column_index] != 'X') {
            return 0;
        }
    }

    return 1;
}

static int complete_line_was_read(const char *input_line, FILE *input_file) {
    return strchr(input_line, '\n') != NULL || feof(input_file);
}

static int read_grid_row(FILE *input_file,
                         char grid_row[MAX_COLUMNS + 1],
                         int column_count) {
    char input_line[MAX_COLUMNS + 2];
    if (fgets(input_line, MAX_COLUMNS + 2, input_file) == NULL ||
        !complete_line_was_read(input_line, input_file)) {
        return 0;
    }

    int row_length = (int) strlen(input_line);
    if (row_length > 0 && input_line[row_length - 1] == '\n') {
        input_line[row_length - 1] = '\0';
    }

    if (!row_contains_only_valid_cells(input_line, column_count)) {
        return 0;
    }

    strcpy(grid_row, input_line);
    return 1;
}

static int read_grid_rows(FILE *input_file, MonitoringGrid *monitoring_grid) {
    for (int row_index = 0; row_index < monitoring_grid->row_count;
         ++row_index) {
        if (!read_grid_row(input_file,
                           monitoring_grid->cells[row_index],
                           monitoring_grid->column_count)) {
            return 0;
        }
    }

    return 1;
}

static int is_inside_grid(const MonitoringGrid *monitoring_grid,
                          int row_index,
                          int column_index) {
    return row_index >= 0 && row_index < monitoring_grid->row_count &&
           column_index >= 0 && column_index < monitoring_grid->column_count;
}

static int is_anomalous_cell(const MonitoringGrid *monitoring_grid,
                             int row_index,
                             int column_index) {
    return is_inside_grid(monitoring_grid, row_index, column_index) &&
           monitoring_grid->cells[row_index][column_index] == 'X';
}

static int is_sequence_start(const MonitoringGrid *monitoring_grid,
                             int row_index,
                             int column_index,
                             SequenceDirection direction) {
    /* A start has no anomalous predecessor along the same direction. */
    DirectionStep direction_step = DIRECTION_STEPS[direction];
    int previous_row_index = row_index - direction_step.row_offset;
    int previous_column_index = column_index - direction_step.column_offset;

    return !is_anomalous_cell(monitoring_grid,
                              previous_row_index,
                              previous_column_index);
}

static AnomalySequence follow_sequence(const MonitoringGrid *monitoring_grid,
                                       int start_row_index,
                                       int start_column_index,
                                       SequenceDirection direction) {
    DirectionStep direction_step = DIRECTION_STEPS[direction];
    AnomalySequence sequence = {
        start_row_index,
        start_column_index,
        start_row_index,
        start_column_index,
        1,
        direction,
    };

    int next_row_index = sequence.end_row_index + direction_step.row_offset;
    int next_column_index =
        sequence.end_column_index + direction_step.column_offset;

    while (is_anomalous_cell(monitoring_grid,
                             next_row_index,
                             next_column_index)) {
        sequence.end_row_index = next_row_index;
        sequence.end_column_index = next_column_index;
        ++sequence.length;

        next_row_index += direction_step.row_offset;
        next_column_index += direction_step.column_offset;
    }

    return sequence;
}

static void collect_cell_sequences(const MonitoringGrid *monitoring_grid,
                                   int row_index,
                                   int column_index,
                                   AnomalySequence sequences[],
                                   int *sequence_count) {
    for (SequenceDirection direction = HORIZONTAL; direction < DIRECTION_COUNT;
         ++direction) {
        if (!is_sequence_start(monitoring_grid,
                               row_index,
                               column_index,
                               direction)) {
            continue;
        }

        AnomalySequence sequence = follow_sequence(monitoring_grid,
                                                   row_index,
                                                   column_index,
                                                   direction);
        if (sequence.length >= monitoring_grid->minimum_sequence_length) {
            sequences[*sequence_count] = sequence;
            ++(*sequence_count);
        }
    }
}
