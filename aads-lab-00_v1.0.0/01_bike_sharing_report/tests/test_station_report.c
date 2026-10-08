#include "station_report.h"
#include "unity.h"

#include <stdio.h>
#include <string.h>

static const Station STATIONS[] = {{"Z09", "South", 20, 0, 8},
                                   {"S02", "East", 12, 7, 12},
                                   {"S01", "North", 15, 7, 12},
                                   {"A01", "West", 10, 0, 5}};

static FILE *stream_from_text(const char *text) {
    FILE *stream = tmpfile();
    if (stream == NULL) {
        return NULL;
    }

    if (fputs(text, stream) < 0) {
        fclose(stream);
        return NULL;
    }

    rewind(stream);
    return stream;
}

static void assert_minimal_valid_snapshot_loads(void) {
    FILE *input_file = stream_from_text("1\n"
                                        "S01 Central 20 5 12\n"
                                        "S01\n");
    TEST_ASSERT_NOT_NULL(input_file);

    Station stations[MAX_STATIONS];
    char query_station_id[STATION_TEXT_CAPACITY];
    int station_count;

    int snapshot_loaded =
        load_snapshot(input_file, stations, &station_count, query_station_id);
    fclose(input_file);

    TEST_ASSERT_EQUAL_INT_MESSAGE(
        1,
        snapshot_loaded,
        "load_snapshot must load valid data before validation is tested");
}

void setUp(void) {
}

void tearDown(void) {
}

static void test_load_snapshot_reads_complete_valid_snapshot(void) {
    FILE *input_file = stream_from_text("4\n"
                                        "S03 Riverside 20 0 12\n"
                                        "S01 Central 24 11 35\n"
                                        "S04 University 18 7 35\n"
                                        "S02 Museum 16 4 19\n"
                                        "S04\n");
    TEST_ASSERT_NOT_NULL(input_file);

    Station stations[MAX_STATIONS];
    char query_station_id[STATION_TEXT_CAPACITY];
    int station_count;

    int snapshot_loaded =
        load_snapshot(input_file, stations, &station_count, query_station_id);
    fclose(input_file);

    TEST_ASSERT_EQUAL_INT(1, snapshot_loaded);
    TEST_ASSERT_EQUAL_INT(4, station_count);
    TEST_ASSERT_EQUAL_STRING("S03", stations[0].id);
    TEST_ASSERT_EQUAL_STRING("Riverside", stations[0].name);
    TEST_ASSERT_EQUAL_INT(20, stations[0].docks);
    TEST_ASSERT_EQUAL_INT(0, stations[0].available_bikes);
    TEST_ASSERT_EQUAL_INT(12, stations[0].daily_rentals);
    TEST_ASSERT_EQUAL_STRING("S01", stations[1].id);
    TEST_ASSERT_EQUAL_STRING("University", stations[2].name);
    TEST_ASSERT_EQUAL_INT(4, stations[3].available_bikes);
    TEST_ASSERT_EQUAL_INT(19, stations[3].daily_rentals);
    TEST_ASSERT_EQUAL_STRING("S04", query_station_id);
}

static void test_load_snapshot_duplicate_identifier_returns_failure(void) {
    assert_minimal_valid_snapshot_loads();

    FILE *input_file = stream_from_text("2\n"
                                        "S01 Central 20 5 12\n"
                                        "S01 Riverside 16 4 9\n"
                                        "S01\n");
    TEST_ASSERT_NOT_NULL(input_file);

    Station stations[MAX_STATIONS];
    char query_station_id[STATION_TEXT_CAPACITY];
    int station_count;

    int snapshot_loaded =
        load_snapshot(input_file, stations, &station_count, query_station_id);
    fclose(input_file);

    TEST_ASSERT_EQUAL_INT(0, snapshot_loaded);
}

static void test_load_snapshot_too_many_available_bikes_returns_failure(void) {
    assert_minimal_valid_snapshot_loads();

    FILE *input_file = stream_from_text("1\n"
                                        "S01 Central 20 21 12\n"
                                        "S01\n");
    TEST_ASSERT_NOT_NULL(input_file);

    Station stations[MAX_STATIONS];
    char query_station_id[STATION_TEXT_CAPACITY];
    int station_count;

    int snapshot_loaded =
        load_snapshot(input_file, stations, &station_count, query_station_id);
    fclose(input_file);

    TEST_ASSERT_EQUAL_INT(0, snapshot_loaded);
}

static void test_load_snapshot_incomplete_record_returns_failure(void) {
    assert_minimal_valid_snapshot_loads();

    FILE *input_file = stream_from_text("1\n"
                                        "S01 Central 20 5\n"
                                        "S01\n");
    TEST_ASSERT_NOT_NULL(input_file);

    Station stations[MAX_STATIONS];
    char query_station_id[STATION_TEXT_CAPACITY];
    int station_count;

    int snapshot_loaded =
        load_snapshot(input_file, stations, &station_count, query_station_id);
    fclose(input_file);

    TEST_ASSERT_EQUAL_INT(0, snapshot_loaded);
}

static void test_load_snapshot_overlong_identifier_returns_failure(void) {
    assert_minimal_valid_snapshot_loads();

    FILE *input_file =
        stream_from_text("1\n"
                         "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA Central 20 5 12\n"
                         "S01\n");
    TEST_ASSERT_NOT_NULL(input_file);

    Station stations[MAX_STATIONS];
    char query_station_id[STATION_TEXT_CAPACITY];
    int station_count;

    int snapshot_loaded =
        load_snapshot(input_file, stations, &station_count, query_station_id);
    fclose(input_file);

    TEST_ASSERT_EQUAL_INT(0, snapshot_loaded);
}

static void test_compute_totals_sums_all_station_values(void) {
    StationTotals totals = compute_totals(STATIONS, 4);

    TEST_ASSERT_EQUAL_INT(57, totals.total_docks);
    TEST_ASSERT_EQUAL_INT(14, totals.total_available);
    TEST_ASSERT_EQUAL_INT(37, totals.total_rentals);
}

static void test_find_station_returns_matching_station_or_null(void) {
    const Station *matching_station = find_station_by_id(STATIONS, 4, "S02");

    TEST_ASSERT_NOT_NULL(matching_station);
    TEST_ASSERT_EQUAL_STRING("S02", matching_station->id);
    TEST_ASSERT_EQUAL_PTR(&STATIONS[0], find_station_by_id(STATIONS, 4, "Z09"));
    TEST_ASSERT_EQUAL_PTR(&STATIONS[3], find_station_by_id(STATIONS, 4, "A01"));
    TEST_ASSERT_NULL(find_station_by_id(STATIONS, 4, "missing"));
}

static void test_find_most_available_station_index_uses_identifier_tie_break(
    void) {
    int most_available_station_index =
        find_most_available_station_index(STATIONS, 4);

    TEST_ASSERT_EQUAL_INT(2, most_available_station_index);
    TEST_ASSERT_EQUAL_STRING("S01", STATIONS[most_available_station_index].id);
}

static void test_collect_zero_available_returns_indices_in_identifier_order(
    void) {
    int zero_available_indices[MAX_STATIONS];

    int zero_available_count =
        collect_zero_availability_station_indexes(STATIONS,
                                                  4,
                                                  zero_available_indices);

    TEST_ASSERT_EQUAL_INT(2, zero_available_count);
    TEST_ASSERT_EQUAL_INT(3, zero_available_indices[0]);
    TEST_ASSERT_EQUAL_INT(0, zero_available_indices[1]);
}

static void test_build_sorted_index_orders_view_without_modifying_stations(
    void) {
    Station original_stations[4];
    memcpy(original_stations, STATIONS, sizeof STATIONS);

    int sorted_station_indices[MAX_STATIONS] = {0};
    build_sorted_indexes_array(STATIONS, 4, sorted_station_indices);

    TEST_ASSERT_EQUAL_INT(2, sorted_station_indices[0]);
    TEST_ASSERT_EQUAL_INT(1, sorted_station_indices[1]);
    TEST_ASSERT_EQUAL_INT(0, sorted_station_indices[2]);
    TEST_ASSERT_EQUAL_INT(3, sorted_station_indices[3]);
    TEST_ASSERT_EQUAL_MEMORY(original_stations, STATIONS, sizeof STATIONS);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_load_snapshot_reads_complete_valid_snapshot);
    RUN_TEST(test_load_snapshot_duplicate_identifier_returns_failure);
    RUN_TEST(test_load_snapshot_too_many_available_bikes_returns_failure);
    RUN_TEST(test_load_snapshot_incomplete_record_returns_failure);
    RUN_TEST(test_load_snapshot_overlong_identifier_returns_failure);
    RUN_TEST(test_compute_totals_sums_all_station_values);
    RUN_TEST(test_find_station_returns_matching_station_or_null);
    RUN_TEST(test_find_most_available_station_index_uses_identifier_tie_break);
    RUN_TEST(test_collect_zero_available_returns_indices_in_identifier_order);
    RUN_TEST(test_build_sorted_index_orders_view_without_modifying_stations);

    return UNITY_END();
}