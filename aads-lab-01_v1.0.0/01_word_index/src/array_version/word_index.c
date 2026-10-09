#include "word_index.h"
#include <string.h>
#include <ctype.h>

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
        
        if (fscanf(input, "%s", word) != 1) {
            return NULL;
        }
        strcpy((*(wordEntry_list + i)).word, word);
        wordEntry_list[i].occurrences = 0;
        
    }
    free(word);
    return wordEntry_list;
}

int find_word(const WordEntry word_entries[],
              int word_count,
              const char *word) {
    /* TODO: implement a case-insensitive search. */
    char * new_word = (char *) malloc ((strlen(word) + 1) * sizeof(char));
    for (int i = 0; i < strlen(word); i++) {
        new_word[i] = tolower(word[i]);
    }
    new_word[strlen(word)] = '\0';

    for (int i = 0; i < word_count; i++) {
        char * lower_word = (char *) malloc ((strlen(((word_entries + i)->word)) + 1) * sizeof(char));
        for (int j = 0; j < strlen((*(word_entries + i)).word); j++) {
            * (lower_word + j) = tolower((*(word_entries + i)).word[j]);
        }
        lower_word[strlen(word_entries[i].word)] = '\0';
        if (strcmp(new_word, lower_word) == 0) {
            return i;
        }
        free(lower_word);
    }
    free(new_word);
    return -1;

}

int count_words(FILE *input, WordEntry word_entries[], int word_count) {
    /* TODO: scan the text and update occurrence counters. */
    char * sentence = (char *) malloc (200 * sizeof(char));
    while (fgets(sentence, 200, input) != NULL) {
        for (char * word = strtok(sentence, " .!,?;"); word != NULL; word = strtok(NULL, " ") ) {
            int i = find_word(word_entries, word_count, word);
            if (i != -1) {
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

void destroy_word_index(WordEntry word_entries[]) {
    /* TODO: release the array representation. */
    free(word_entries);
}
