#include "station_report.h"
#include <string.h>

int load_snapshot(FILE *input_file,
                  Station stations[],
                  int *station_count,
                  char query_station_id[STATION_TEXT_CAPACITY]) {
    /* TODO: load and validate the complete station snapshot. */
    
    if (fscanf(input_file, "%d", station_count) != 1) {
        return 0;
    }
    if (*station_count < 1 || *station_count > 100) {
        return 0;
    }

    for (int i = 0; i< *station_count; i++) {
        if (fscanf(input_file, "%s %s %d %d %d",
                   stations[i].id,
                   stations[i].name,
                   &stations[i].docks,
                   &stations[i].available_bikes,
                   &stations[i].daily_rentals) != 5) {
            
            return 0;
        }
    }
    
    if (fscanf(input_file, "%s", query_station_id) != 1) {
        return 0;
    }

    for (int i = 0; i < *station_count; i++) {
        if (stations[i].available_bikes > stations[i].docks) return 0;
        if (stations[i].docks <= 0 || stations[i].available_bikes < 0 || stations[i].daily_rentals < 0) return 0;
        if (strlen(stations[i].id) >= STATION_TEXT_CAPACITY || strlen(stations[i].name) >= STATION_TEXT_CAPACITY) return 0;
        for (int j = i + 1; j < *station_count; j++) {
            if (strcmp(stations[i].id, stations[j].id) == 0) return 0;
        }
    }
    return 1;
}

StationTotals compute_totals(const Station stations[], int station_count) {
    /* TODO: compute all aggregate values. */
    
    StationTotals totals = {0, 0, 0};
    for (int i = 0; i < station_count; i++) {
        totals.total_docks += stations[i].docks;
        totals.total_available += stations[i].available_bikes;
        totals.total_rentals += stations[i].daily_rentals;
    }
    return totals;
}

const Station *find_station_by_id(const Station stations[],
                                  int station_count,
                                  const char *station_id) {
    /* TODO: search for the requested station identifier. */
    if (station_count <= 0) {
        return NULL;
    }
    for (int i = 0; i < station_count; i++) {
        if (strcmp(stations[i].id, station_id) == 0) {
            return &stations[i];
        }
    }
    return NULL;
}

int find_most_available_station_index(const Station stations[],
                                      int station_count) {
    /* TODO: select the station with the most available bicycles. */
    if (station_count <= 0) {
        return -1;
    }
    int max_index = 0;
    for (int i = 1; i < station_count; i++) {
        if (stations[i].available_bikes > stations[max_index].available_bikes) {
            max_index = i;
        } else if (stations[i].available_bikes == stations[max_index].available_bikes) {
            if (strcmp(stations[i].id, stations[max_index].id) < 0) {
                max_index = i;
            }
        }
    }
    return max_index;
}

int collect_zero_availability_station_indexes(
    const Station stations[],
    int station_count,
    int station_indices[MAX_STATIONS]) {
    /* TODO: collect and order the indices of stations without bicycles. */
    int count = 0;
    for (int i = 0; i < station_count; i++) {
        if (stations[i].available_bikes == 0) {
            station_indices[count] = i;
            count++;
        }
    }
    // Sort the indices based on station identifiers
    for (int i = count - 1; i > 0; i--) {
        for (int j = 0; j < i; j++) {
            if (strcmp(stations[station_indices[i]].id, stations[station_indices[j]].id) < 0) {
                int temp = station_indices[i];
                station_indices[i] = station_indices[j];
                station_indices[j] = temp;
            }
        }
    }
    return count;

}

void build_sorted_indexes_array(const Station stations[],
                                int station_count,
                                int station_indices[MAX_STATIONS]) {
    /* TODO: initialize and sort the complete index view. */
    for (int i = 0; i < station_count; i++) {
        station_indices[i] = i;
    }
    // Sort the indices based on daily rentals and station identifiers
    for (int i = station_count - 1; i > 0; i--) {
        for (int j = 0; j < i; j++) {
            if (stations[station_indices[i]].daily_rentals > stations[station_indices[j]].daily_rentals) {
                int temp = station_indices[i];
                station_indices[i] = station_indices[j];
                station_indices[j] = temp;
            } else if (stations[station_indices[i]].daily_rentals == stations[station_indices[j]].daily_rentals) {
                if (strcmp(stations[station_indices[i]].id, stations[station_indices[j]].id) < 0) {
                    int temp = station_indices[i];
                    station_indices[i] = station_indices[j];
                    station_indices[j] = temp;
                }
            }
        }
    }
}
