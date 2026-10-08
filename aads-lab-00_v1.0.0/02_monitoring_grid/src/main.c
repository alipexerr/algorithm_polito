#include "monitoring_grid.h"

#include <stdio.h>
#include <stdlib.h>

static void print_sequence(const AnomalySequence *sequence);
static FILE *open_input_file(const char *file_path);
static void print_sequence_report(const MonitoringGrid *monitoring_grid,
                                  const AnomalySequence sequences[],
                                  int sequence_count);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: monitoring_grid <input_file>\n");
        return EXIT_FAILURE;
    }

    FILE *input_file = open_input_file(argv[1]);
    if (input_file == NULL) {
        return EXIT_FAILURE;
    }

    MonitoringGrid monitoring_grid;
    int grid_loaded = load_monitoring_grid(input_file, &monitoring_grid);
    fclose(input_file);

    if (!grid_loaded) {
        fprintf(stderr, "Invalid monitoring grid.\n");
        return EXIT_FAILURE;
    }

    AnomalySequence sequences[MAX_ANOMALY_SEQUENCES];
    int sequence_count = find_anomaly_sequences(&monitoring_grid, sequences);
    print_sequence_report(&monitoring_grid, sequences, sequence_count);

    return EXIT_SUCCESS;
}

static void print_sequence(const AnomalySequence *sequence) {
    printf("%d | %d | %d | %d | %s | %d\n",
           sequence->start_row_index + 1,
           sequence->start_column_index + 1,
           sequence->end_row_index + 1,
           sequence->end_column_index + 1,
           get_sequence_direction_name(sequence->direction),
           sequence->length);
}

static FILE *open_input_file(const char *file_path) {
    FILE *input_file = fopen(file_path, "r");

    if (input_file == NULL) {
        fprintf(stderr, "Cannot open input file.\n");
    }

    return input_file;
}

static void print_sequence_report(const MonitoringGrid *monitoring_grid,
                                  const AnomalySequence sequences[],
                                  int sequence_count) {
    puts("ANOMALY SEQUENCES");
    puts("Start row | Start column | End row | End column | "
         "Direction | Length");

    for (int sequence_index = 0; sequence_index < sequence_count;
         ++sequence_index) {
        print_sequence(&sequences[sequence_index]);
    }

    puts("\nSTATS");
    printf("Grid size: %d rows x %d columns\n",
           monitoring_grid->row_count,
           monitoring_grid->column_count);
    printf("Minimum sequence length: %d\n",
           monitoring_grid->minimum_sequence_length);
    printf("Sequences found: %d\n", sequence_count);
}
