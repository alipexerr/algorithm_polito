#include "word_index.h"

WordEntry *load_word_index(FILE *input, int *word_count) {
    /* TODO: allocate the records and every owned word. */
    return NULL;
}

int find_word(const WordEntry word_entries[],
              int word_count,
              const char *word) {
    /* TODO: implement a case-insensitive search. */
    return -1;
}

int count_words(FILE *input, WordEntry word_entries[], int word_count) {
    /* TODO: scan the text and update occurrence counters. */
    return 0;
}

void destroy_word_index(WordEntry word_entries[], int word_count) {
    /* TODO: release every word and then the record array. */
}
