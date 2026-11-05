#include "planner/planner.h"
#include "planner/planner_io.h"
#include "support/allocation_tracker.h"
#include "unity.h"

#include <stdio.h>
#include <string.h>

static Planner planner;

void setUp(void) {
    planner = (Planner) {0};
}

void tearDown(void) {
    planner_destroy(&planner);
}

static FILE *stream_from_text(const char *text) {
    FILE *stream = tmpfile();
    TEST_ASSERT_NOT_NULL_MESSAGE(stream, "temporary input stream failed");
    TEST_ASSERT_NOT_EQUAL_MESSAGE(EOF,
                                  fputs(text, stream),
                                  "temporary input write failed");
    rewind(stream);
    return stream;
}

static void assert_task(const Task *task,
                        const char *expected_id,
                        int expected_date,
                        const char *expected_label) {
    TEST_ASSERT_NOT_NULL_MESSAGE(task, "expected task is missing");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_id,
                                     task->identifier,
                                     "unexpected task identifier");
    TEST_ASSERT_EQUAL_INT_MESSAGE(expected_date,
                                  task->due_date,
                                  "unexpected task due date");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_label,
                                     task->label,
                                     "unexpected task label");
}

static void test_add_orders_tasks_by_date_then_identifier(void) {
    TEST_ASSERT_EQUAL_INT(
        PLANNER_OK,
        planner_add_task(&planner, "T12", 20260909, "send_material"));
    TEST_ASSERT_EQUAL_INT(
        PLANNER_OK,
        planner_add_task(&planner, "T17", 20260907, "prepare_slides"));
    TEST_ASSERT_EQUAL_INT(
        PLANNER_OK,
        planner_add_task(&planner, "T08", 20260915, "write_report"));
    TEST_ASSERT_EQUAL_INT(
        PLANNER_OK,
        planner_add_task(&planner, "T04", 20260909, "correct_exercises"));

    const char *expected_ids[] = {"T17", "T04", "T12", "T08"};
    const Task *task = planner.pending_tasks;
    for (int task_index = 0; task_index < 4; task_index++) {
        TEST_ASSERT_NOT_NULL_MESSAGE(task, "ordered traversal ended early");
        TEST_ASSERT_EQUAL_STRING_MESSAGE(expected_ids[task_index],
                                         task->identifier,
                                         "incorrect ordered task position");
        task = task->next;
    }
    TEST_ASSERT_NULL_MESSAGE(task, "ordered traversal contains extra tasks");
}

static void test_add_owns_label_and_rejects_duplicate_identifier(void) {
    char label[] = "draft";
    TEST_ASSERT_EQUAL_INT(PLANNER_OK,
                          planner_add_task(&planner, "T01", 20260908, label));
    strcpy(label, "other");

    const Task *stored_task = planner_find_task(&planner, "T01");
    assert_task(stored_task, "T01", 20260908, "draft");

    TEST_ASSERT_EQUAL_INT(
        PLANNER_DUPLICATE,
        planner_add_task(&planner, "T01", 20270101, "replacement"));
    TEST_ASSERT_EQUAL_INT(1, planner.pending_task_count);
    assert_task(planner_find_task(&planner, "T01"), "T01", 20260908, "draft");
}

static void test_find_returns_exact_task_and_missing_returns_null(void) {
    TEST_ASSERT_EQUAL_INT(PLANNER_OK,
                          planner_add_task(&planner, "T01", 20260908, "first"));
    TEST_ASSERT_EQUAL_INT(
        PLANNER_OK,
        planner_add_task(&planner, "T02", 20260909, "second"));

    assert_task(planner_find_task(&planner, "T02"), "T02", 20260909, "second");
    TEST_ASSERT_NULL_MESSAGE(planner_find_task(&planner, "T99"),
                             "missing identifier should not match");
}

static void test_remove_handles_head_internal_tail_and_missing_tasks(void) {
    TEST_ASSERT_EQUAL_INT(PLANNER_OK,
                          planner_add_task(&planner, "T01", 20260901, "head"));
    TEST_ASSERT_EQUAL_INT(
        PLANNER_OK,
        planner_add_task(&planner, "T02", 20260902, "internal"));
    TEST_ASSERT_EQUAL_INT(PLANNER_OK,
                          planner_add_task(&planner, "T03", 20260903, "tail"));

    TEST_ASSERT_EQUAL_INT(PLANNER_OK, planner_remove_task(&planner, "T02"));
    TEST_ASSERT_EQUAL_STRING("T01", planner.pending_tasks->identifier);
    TEST_ASSERT_EQUAL_STRING("T03", planner.pending_tasks->next->identifier);

    TEST_ASSERT_EQUAL_INT(PLANNER_OK, planner_remove_task(&planner, "T01"));
    TEST_ASSERT_EQUAL_STRING("T03", planner.pending_tasks->identifier);

    TEST_ASSERT_EQUAL_INT(PLANNER_OK, planner_remove_task(&planner, "T03"));
    TEST_ASSERT_EQUAL_INT(0, planner.pending_task_count);
    TEST_ASSERT_NULL(planner.pending_tasks);

    TEST_ASSERT_EQUAL_INT(PLANNER_NOT_FOUND,
                          planner_remove_task(&planner, "T99"));
}

static void test_loads_non_chronological_input_in_order(void) {
    FILE *input_file = stream_from_text("T12 20260909 send_material\n"
                                        "T17 20260907 prepare_slides\n"
                                        "T04 20260909 correct_exercises\n");

    TEST_ASSERT_EQUAL_INT(PLANNER_OK,
                          planner_io_load_tasks(&planner, input_file));
    fclose(input_file);

    TEST_ASSERT_EQUAL_INT(3, planner.pending_task_count);
    const Task *task = planner.pending_tasks;
    TEST_ASSERT_EQUAL_STRING("T17", task->identifier);
    task = task->next;
    TEST_ASSERT_EQUAL_STRING("T04", task->identifier);
    task = task->next;
    TEST_ASSERT_EQUAL_STRING("T12", task->identifier);
}

static void test_load_handles_empty_and_single_task_streams(void) {
    FILE *empty_input = stream_from_text("");
    TEST_ASSERT_EQUAL_INT(PLANNER_OK,
                          planner_io_load_tasks(&planner, empty_input));
    fclose(empty_input);
    TEST_ASSERT_EQUAL_INT(0, planner.pending_task_count);
    TEST_ASSERT_NULL(planner.pending_tasks);

    FILE *single_input = stream_from_text("\nONLY 20260908 prepare\n\n   \n");
    TEST_ASSERT_EQUAL_INT(PLANNER_OK,
                          planner_io_load_tasks(&planner, single_input));
    fclose(single_input);
    TEST_ASSERT_EQUAL_INT(1, planner.pending_task_count);
    assert_task(planner.pending_tasks, "ONLY", 20260908, "prepare");
    TEST_ASSERT_NULL(planner.pending_tasks->next);
}

static void test_load_failure_releases_partial_collection(void) {
    TEST_ASSERT_EQUAL_INT(
        PLANNER_OK,
        planner_add_task(&planner, "OLD", 20250101, "old_task"));
    FILE *input_file = stream_from_text("T01 20260908 valid\n"
                                        "malformed row\n");

    TEST_ASSERT_EQUAL_INT(PLANNER_INVALID_DATA,
                          planner_io_load_tasks(&planner, input_file));
    fclose(input_file);

    TEST_ASSERT_EQUAL_INT_MESSAGE(0,
                                  planner.pending_task_count,
                                  "failed load must leave planner empty");
    TEST_ASSERT_NULL(planner.pending_tasks);
}

static void test_load_rejects_duplicate_identifiers(void) {
    FILE *input_file = stream_from_text("T01 20260908 first\n"
                                        "T01 20260909 duplicate\n");

    TEST_ASSERT_EQUAL_INT(PLANNER_DUPLICATE,
                          planner_io_load_tasks(&planner, input_file));
    fclose(input_file);
    TEST_ASSERT_EQUAL_INT(0, planner.pending_task_count);
}

static void test_every_allocated_block_is_released(void) {
    FILE *input_file = stream_from_text(
        "T12 20260909 send_material\nT17 20260907 prepare_slides\n");
    int blocks_before = outstanding_allocation_count();
    Planner local_planner = {0};

    TEST_ASSERT_EQUAL_INT(PLANNER_OK,
                          planner_io_load_tasks(&local_planner, input_file));
    TEST_ASSERT_EQUAL_INT(PLANNER_OK,
                          planner_remove_task(&local_planner, "T12"));
    planner_destroy(&local_planner);

    int leaked_blocks = outstanding_allocation_count() - blocks_before;
    fclose(input_file);
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        0,
        leaked_blocks,
        "memory blocks are still allocated: every malloc, calloc and "
        "realloc needs a matching free");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_add_orders_tasks_by_date_then_identifier);
    RUN_TEST(test_add_owns_label_and_rejects_duplicate_identifier);
    RUN_TEST(test_find_returns_exact_task_and_missing_returns_null);
    RUN_TEST(test_remove_handles_head_internal_tail_and_missing_tasks);
    RUN_TEST(test_loads_non_chronological_input_in_order);
    RUN_TEST(test_load_handles_empty_and_single_task_streams);
    RUN_TEST(test_load_failure_releases_partial_collection);
    RUN_TEST(test_load_rejects_duplicate_identifiers);
    RUN_TEST(test_every_allocated_block_is_released);
    return UNITY_END();
}
