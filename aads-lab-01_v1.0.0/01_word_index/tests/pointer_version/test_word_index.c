#include "word_index.h"
#include "support/allocation_tracker.h"
#include "unity.h"

#include <stdio.h>

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

static void test_load_word_index_pointer_with_words_owns_data_and_clears_counts(
    void) {
    FILE *input = stream_from_text("3\nAlpha\nbeta\nGamma\n");
    int word_count = 0;
    WordEntry *word_index = load_word_index(input, &word_count);

    TEST_ASSERT_NOT_NULL(word_index);
    TEST_ASSERT_EQUAL_INT(3, word_count);
    TEST_ASSERT_NOT_NULL(word_index[0].word);
    TEST_ASSERT_NOT_NULL(word_index[1].word);
    TEST_ASSERT_TRUE(word_index[0].word != word_index[1].word);
    TEST_ASSERT_EQUAL_STRING("Alpha", word_index[0].word);
    TEST_ASSERT_EQUAL_STRING("beta", word_index[1].word);
    TEST_ASSERT_EQUAL_STRING("Gamma", word_index[2].word);
    TEST_ASSERT_EQUAL_INT(0, word_index[0].occurrences);
    TEST_ASSERT_EQUAL_INT(0, word_index[1].occurrences);
    TEST_ASSERT_EQUAL_INT(0, word_index[2].occurrences);

    destroy_word_index(word_index, word_count);
    fclose(input);
}

static void
test_find_word_pointer_with_case_variations_returns_matching_indexes(void) {
    char alpha_word[] = "alpha";
    char beta_word[] = "beta";
    WordEntry word_index[2] = {{alpha_word, 0}, {beta_word, 0}};

    TEST_ASSERT_EQUAL_INT(0, find_word(word_index, 2, "ALPHA"));
    TEST_ASSERT_EQUAL_INT(1, find_word(word_index, 2, "Beta"));
    TEST_ASSERT_EQUAL_INT(-1, find_word(word_index, 2, "gamma"));
}

static void test_count_words_pointer_with_mixed_words_updates_known_counts(
    void) {
    char alpha_word[] = "alpha";
    char beta_word[] = "beta";
    WordEntry word_index[2] = {{alpha_word, 0}, {beta_word, 0}};
    FILE *input = stream_from_text("ALPHA unknown beta alpha alpha");

    TEST_ASSERT_TRUE(count_words(input, word_index, 2));
    TEST_ASSERT_EQUAL_INT(3, word_index[0].occurrences);
    TEST_ASSERT_EQUAL_INT(1, word_index[1].occurrences);

    fclose(input);
}

static void test_count_words_pointer_with_empty_text_keeps_zero_counts(void) {
    char alpha_word[] = "alpha";
    char beta_word[] = "beta";
    WordEntry word_index[2] = {{alpha_word, 0}, {beta_word, 0}};
    FILE *input = stream_from_text("");

    TEST_ASSERT_TRUE(count_words(input, word_index, 2));
    TEST_ASSERT_EQUAL_INT(0, word_index[0].occurrences);
    TEST_ASSERT_EQUAL_INT(0, word_index[1].occurrences);

    fclose(input);
}

static void test_destroy_word_index_pointer_releases_memory_and_accepts_null(
    void) {
    FILE *input = stream_from_text("2\nalpha\nbeta\n");
    int blocks_before = outstanding_allocation_count();
    int word_count = 0;
    WordEntry *word_index = load_word_index(input, &word_count);

    TEST_ASSERT_NOT_NULL(word_index);
    destroy_word_index(word_index, word_count);
    destroy_word_index(NULL, 0);

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
    RUN_TEST(
        test_load_word_index_pointer_with_words_owns_data_and_clears_counts);
    RUN_TEST(
        test_find_word_pointer_with_case_variations_returns_matching_indexes);
    RUN_TEST(test_count_words_pointer_with_mixed_words_updates_known_counts);
    RUN_TEST(test_count_words_pointer_with_empty_text_keeps_zero_counts);
    RUN_TEST(test_destroy_word_index_pointer_releases_memory_and_accepts_null);
    return UNITY_END();
}
