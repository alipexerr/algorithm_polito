#ifndef STATION_REPORT_H
#define STATION_REPORT_H

#include <stdio.h>

#define MAX_STATIONS 100
#define STATION_TEXT_CAPACITY 32

/**
 * @brief State and daily activity recorded for one bike-sharing station.
 */
typedef struct {
    char id[STATION_TEXT_CAPACITY];   /**< Unique station identifier. */
    char name[STATION_TEXT_CAPACITY]; /**< Single-token station name. */
    int docks;                        /**< Total number of docks. */
    int available_bikes;              /**< Currently available bicycles. */
    int daily_rentals;                /**< Rentals completed during the day. */
} Station;

/**
 * @brief Aggregate values computed for a complete station snapshot.
 */
typedef struct {
    int total_docks;     /**< Sum of all station docks. */
    int total_available; /**< Sum of all available bicycles. */
    int total_rentals;   /**< Sum of all daily rentals. */
} StationTotals;

/**
 * @brief Loads and validates a complete station snapshot.
 *
 * The input stream must contain the station count, exactly that many station
 * records, and the identifier to search for. Station identifiers must be
 * unique and every value must satisfy the limits defined by the problem.
 * The function reads the stream but does not close it.
 *
 * @param[in] input_file Already open input stream positioned at the beginning of
 *                       the snapshot.
 * @param[out] stations Array that receives the station records in input order.
 * @param[out] station_count Receives the number of loaded stations.
 * @param[out] query_station_id String (array of char) that receives the identifier to
 *                             search for.
 * @return 1 when the complete snapshot is valid and loaded; 0 otherwise.
 */
int load_snapshot(FILE *input_file,
                  Station stations[],
                  int *station_count,
                  char query_station_id[STATION_TEXT_CAPACITY]);

/**
 * @brief Computes aggregate values for all stations in the snapshot.
 *
 * @param[in] stations Station records to examine.
 * @param[in] station_count Number of records stored in stations.
 * @return Totals for docks, available bicycles, and daily rentals.
 */
StationTotals compute_totals(const Station stations[], int station_count);

/**
 * @brief Searches for a station with the requested identifier.
 *
 * @param[in] stations Station records to search without modifying them.
 * @param[in] station_count Number of records stored in stations.
 * @param[in] station_id Identifier to find.
 * @return Pointer to the matching record, or NULL when no record matches.
 */
const Station *find_station_by_id(const Station stations[],
                                  int station_count,
                                  const char *station_id);

/**
 * @brief Selects the station with the greatest number of available bicycles.
 *
 * When several stations have the same number of available bicycles, the
 * station with the lexicographically smallest identifier is selected.
 *
 * @param[in] stations Station records to examine.
 * @param[in] station_count Number of records stored in stations; it must
 *                          be at least 1.
 * @return Index of the selected station in the original array.
 */
int find_most_available_station_index(const Station stations[],
                                      int station_count);

/**
 * @brief Collects the indices of stations with no available bicycles.
 *
 * The resulting indices are ordered by station identifier in ascending order.
 * The station records and their original order are not modified.
 *
 * @param[in] stations Station records to examine.
 * @param[in] station_count Number of records stored in stations.
 * @param[out] station_indices Array that receives the selected station indices.
 * @return Number of indices written to the output array.
 */
int collect_zero_availability_station_indexes(
    const Station stations[],
    int station_count,
    int station_indices[MAX_STATIONS]);

/**
 * @brief Builds a sorted index view of all station records.
 *
 * Indices are ordered by daily rentals in descending order and then by station
 * identifier in ascending order. The original station array is not modified.
 *
 * @param[in] stations Station records referenced by the sorted view.
 * @param[in] station_count Number of records stored in stations.
 * @param[out] station_indices Array that receives all station indices in
 *                            sorted order.
 */
void build_sorted_indexes_array(const Station stations[],
                                int station_count,
                                int station_indices[MAX_STATIONS]);

#endif
