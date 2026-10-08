#include "word_index.h"
#include <stdlib.h>
#include <string.h>

WordEntry *load_word_index(FILE *input, int *word_count) {
    /* TODO: allocate and load the array representation. */
    if (fscanf(input, "%d", word_count) != 1 || *word_count <= 0) {
        return NULL;
    }
    char * word;
    word = (char *) malloc ((MAX_WORD_LENGTH + 1)* sizeof(char));
    WordEntry *wordEntry_list;
    wordEntry_list = (WordEntry *) malloc (*word_count * sizeof(WordEntry));

    for (int i = 0; i < *word_count; i++) {
        
        if (fscanf(input, "%31s", word) != 1) {
            return NULL;
        }
        strcpy(wordEntry_list[i].word, word + i);
        wordEntry_list[i].occurrences = 0;
        
    }
    return wordEntry_list;
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

void destroy_word_index(WordEntry word_entries[]) {
    /* TODO: release the array representation. */
}
