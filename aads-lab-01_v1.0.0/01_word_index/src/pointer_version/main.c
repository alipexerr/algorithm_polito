#include "word_index.h"

#include <stdio.h>
#include <stdlib.h>

/* Opens filename for reading; returns the stream, or NULL after reporting
   an opening failure to standard error. */
static FILE *open_input_file(const char *filename);

/* Prints the report heading and the word_count entries with their counts. */
static void print_word_report(const WordEntry word_entries[], int word_count);

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr,
                "Usage: word_index_pointer <word_list_file> <text_file>\n");
        return EXIT_FAILURE;
    }

    FILE *word_file = open_input_file(argv[1]);
    if (word_file == NULL) {
        return EXIT_FAILURE;
    }

    int word_count = 0;
    WordEntry *word_entries = load_word_index(word_file, &word_count);
    fclose(word_file);
    if (word_entries == NULL) {
        fprintf(stderr, "Cannot load word index.\n");
        return EXIT_FAILURE;
    }

    FILE *text_file = open_input_file(argv[2]);
    if (text_file == NULL) {
        destroy_word_index(word_entries, word_count);
        return EXIT_FAILURE;
    }

    if (!count_words(text_file, word_entries, word_count)) {
        fprintf(stderr, "Cannot read text file.\n");
        fclose(text_file);
        destroy_word_index(word_entries, word_count);
        return EXIT_FAILURE;
    }
    fclose(text_file);

    print_word_report(word_entries, word_count);

    destroy_word_index(word_entries, word_count);
    return EXIT_SUCCESS;
}

static FILE *open_input_file(const char *filename) {
    FILE *input = fopen(filename, "r");

    if (input == NULL) {
        fprintf(stderr, "Cannot open input file: %s\n", filename);
    }

    return input;
}

static void print_word_report(const WordEntry word_entries[], int word_count) {
    printf("WORD OCCURRENCES\n");
    for (int entry_index = 0; entry_index < word_count; ++entry_index) {
        printf("%s %d\n",
               word_entries[entry_index].word,
               word_entries[entry_index].occurrences);
    }
}
