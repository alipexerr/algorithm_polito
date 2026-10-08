#ifndef WORD_INDEX_POINTER_H
#define WORD_INDEX_POINTER_H

#include <stdio.h>

/** Maximum number of characters in an indexed word. */
#define MAX_WORD_LENGTH 30

/** @brief Stores one indexed word reached through a pointer and its count. */
typedef struct {
    char *word;
    int occurrences;
} WordEntry;

/**
 * @brief Loads the indexed words from an already-open stream.
 *
 * @param[in] input Stream positioned at the word count.
 * @param[out] word_count Number of loaded entries on success; zero on failure.
 * @return A dynamically allocated array in file order, or NULL on read or
 *         allocation failure. The caller owns the returned array; each record
 *         returned by this function owns an independently allocated word.
 *         Every occurrence counter starts at zero.
 */
WordEntry *load_word_index(FILE *input, int *word_count);

/**
 * @brief Finds a word without regard to letter case.
 *
 * @param[in] word_entries Array of indexed words.
 * @param[in] word_count Number of valid array elements.
 * @param[in] word Word to search for.
 * @return The matching array index, or -1 when the word is absent.
 */
int find_word(const WordEntry word_entries[], int word_count, const char *word);

/**
 * @brief Counts all indexed words read from an already-open text stream.
 *
 * @param[in] input Stream positioned at the beginning of the text.
 * @param[in,out] word_entries Entries whose occurrence counters are
 *                             incremented.
 * @param[in] word_count Number of valid array elements.
 * @return 1 after reaching end of file, or 0 after a read error.
 */
int count_words(FILE *input, WordEntry word_entries[], int word_count);

/**
 * @brief Releases every owned word and then the allocated record array.
 *
 * @param[in] word_entries Array returned by load_word_index; NULL is accepted.
 * @param[in] word_count Number of valid array elements.
 */
void destroy_word_index(WordEntry word_entries[], int word_count);

#endif
