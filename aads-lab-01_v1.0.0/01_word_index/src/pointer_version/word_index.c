#include "word_index.h"
#include <ctype.h>

WordEntry *load_word_index(FILE *input, int *word_count) {
    /* TODO: allocate the records and every owned word. */
    if (word_count == NULL || input==NULL) {
        return NULL;
    }
    if (fscanf(input, "%d", word_count) != 1 || *word_count <= 0) {
        *word_count = 0;
        return NULL;
    }

    char *word;
    word = (char *) malloc ((MAX_WORD_LENGTH + 1)* sizeof(char));

    if (word == NULL) {
        return NULL;
    }
    WordEntry * word_entries;
    word_entries = (WordEntry *) malloc ((* word_count) * sizeof(WordEntry));

    if (word_entries == NULL) {
        free(word);
        return NULL;
    }

    for (int i = 0; i < *word_count; i++) {
        if (fscanf(input, "%31s", word) != 1) {
            free(word);
            destroy_word_index(word_entries, *word_count);
            return NULL;
        }
        (*(word_entries + i)).word = malloc(strlen(word) + 1);
        if ((*(word_entries + i)).word == NULL) {
            free(word);
            destroy_word_index(word_entries, *word_count);
            return NULL;
        }
        strcpy((*(word_entries + i)).word, word);
        (*(word_entries + i)).occurrences = 0;
    }
    free(word);
    return word_entries;
}

int find_word(const WordEntry word_entries[],
              int word_count,
              const char *word) {
    /* TODO: implement a case-insensitive search. */
    if (word_count <= 0) {
        return -1;
    }
    if (word == NULL || *word == '\0') {
        return -1;
    }
    size_t input_word_size = strlen(word);

    char * new_word = (char *) malloc ((input_word_size + 1) * sizeof(char));
    if (new_word == NULL) {
        return -1;
    }
    for (int i = 0; i < input_word_size; i++) {
        new_word[i] = tolower((unsigned char)word[i]);
    }
    new_word[input_word_size] = '\0';

    for (int i = 0; i < word_count; i++) {
        size_t word_entries_index_word_size = strlen((word_entries + i)->word);
        char * lower_word = (char *) malloc ((word_entries_index_word_size + 1) * sizeof(char));
        if (lower_word == NULL) {
            free(new_word);
            return -1;
        }
        for (int j = 0; j < word_entries_index_word_size; j++) {
            * (lower_word + j) = tolower((unsigned char)(*(word_entries + i)).word[j]);
        }
        lower_word[word_entries_index_word_size] = '\0';
        if (strcmp(new_word, lower_word) == 0) {
            free(new_word);
            free(lower_word);
            return i;
        }
        free(lower_word);
    }
    free(new_word);

    return -1;
}

int count_words(FILE *input, WordEntry word_entries[], int word_count) {
    /* TODO: scan the text and update occurrence counters. */
    char * sentence = (char *) malloc (1000 * sizeof(char));
    if (sentence == NULL) {
        return 0;
    }
    while (fgets(sentence, 1000, input) != NULL) {
        for (char * word = strtok(sentence, " .!,?;:'-/\n\t\r"); word != NULL; word = strtok(NULL, " .!,?;:'-/\n\t\r") ) {
            int i = find_word(word_entries, word_count, word);
            if (i >= 0) {
                word_entries[i].occurrences++;
            }

        }
    }
    if (ferror(input)) {
        free(sentence);
        return 0;  // Error
    }
    free(sentence);
    return 1;
}

void destroy_word_index(WordEntry word_entries[], int word_count) {
    /* TODO: release every word and then the record array. */
    if (word_entries == NULL) {
        return;
    }
    for (int i = 0; i < word_count; i++) {
        free((*(word_entries + i)).word);

    }
    free(word_entries);
}
