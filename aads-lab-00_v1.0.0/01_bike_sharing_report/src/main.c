#include "station_report.h"

#include <stdlib.h>

static FILE *open_input_file(const char *file_path);
static void print_zero_available_stations(const Station stations[],
                                          int station_count);
static void print_query_result(const Station stations[],
                               int station_count,
                               const char *query_station_id);
static void print_sorted_stations(const Station stations[], int station_count);
static void print_station_statistics(const Station stations[],
                                     int station_count);
static void print_station_report(const Station stations[],
                                 int station_count,
                                 const char *query_station_id);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: bike_sharing_report <input_file>\n");
        return EXIT_FAILURE;
    }

    FILE *input_file = open_input_file(argv[1]);
    if (input_file == NULL) {
        return EXIT_FAILURE;
    }

    Station stations[MAX_STATIONS];
    char query_station_id[STATION_TEXT_CAPACITY];
    int station_count;
    int snapshot_loaded =
        load_snapshot(input_file, stations, &station_count, query_station_id);
    fclose(input_file);

    if (!snapshot_loaded) {
        fprintf(stderr, "Invalid station snapshot.\n");
        return EXIT_FAILURE;
    }

    print_station_report(stations, station_count, query_station_id);
    return EXIT_SUCCESS;
}

static FILE *open_input_file(const char *file_path) {
    FILE *input_file = fopen(file_path, "r");

    if (input_file == NULL) {
        fprintf(stderr, "Cannot open input file.\n");
    }

    return input_file;
}

static void print_zero_available_stations(const Station stations[],
                                          int station_count) {
    int zero_available_indices[MAX_STATIONS];
    int zero_available_count =
        collect_zero_availability_station_indexes(stations,
                                                  station_count,
                                                  zero_available_indices);

    printf("Stations with no available bikes: %d", zero_available_count);
    if (zero_available_count > 0) {
        printf(" (");
        for (int result_index = 0; result_index < zero_available_count;
             ++result_index) {
            int station_index = zero_available_indices[result_index];
            if (result_index > 0) {
                printf(", ");
            }
            printf("%s", stations[station_index].id);
        }
        printf(")");
    }
    putchar('\n');
}

static void print_query_result(const Station stations[],
                               int station_count,
                               const char *query_station_id) {
    const Station *requested_station =
        find_station_by_id(stations, station_count, query_station_id);

    puts("QUERY RESULT");
    printf("Requested station: %s\n", query_station_id);

    if (requested_station == NULL) {
        puts("Station not found.");
        return;
    }

    printf("Name: %s\n", requested_station->name);
    printf("Docks: %d\n", requested_station->docks);
    printf("Available bikes: %d\n", requested_station->available_bikes);
    printf("Daily rentals: %d\n", requested_station->daily_rentals);
}

static void print_sorted_stations(const Station stations[], int station_count) {
    int sorted_station_indices[MAX_STATIONS];
    build_sorted_indexes_array(stations, station_count, sorted_station_indices);

    puts("STATIONS SORTED BY DAILY RENTALS");
    puts("ID | Name | Docks | Available bikes | Daily rentals");
    for (int result_index = 0; result_index < station_count; ++result_index) {
        const Station *station =
            &stations[sorted_station_indices[result_index]];
        printf("%s | %s | %d | %d | %d\n",
               station->id,
               station->name,
               station->docks,
               station->available_bikes,
               station->daily_rentals);
    }
}

static void print_station_statistics(const Station stations[],
                                     int station_count) {
    StationTotals totals = compute_totals(stations, station_count);
    int most_available_station_index =
        find_most_available_station_index(stations, station_count);

    puts("STATS");
    printf("Stations: %d\n", station_count);
    printf("Total docks: %d\n", totals.total_docks);
    printf("Available bikes: %d\n", totals.total_available);
    printf("Daily rentals: %d\n", totals.total_rentals);
    printf("Station with most available bikes: %s (%d bikes)\n",
           stations[most_available_station_index].id,
           stations[most_available_station_index].available_bikes);

    print_zero_available_stations(stations, station_count);
}

static void print_station_report(const Station stations[],
                                 int station_count,
                                 const char *query_station_id) {
    print_sorted_stations(stations, station_count);
    putchar('\n');
    print_station_statistics(stations, station_count);
    putchar('\n');
    print_query_result(stations, station_count, query_station_id);
}
