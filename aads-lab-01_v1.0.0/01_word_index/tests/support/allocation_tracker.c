/*
 * Test infrastructure. It counts the memory blocks that are still allocated,
 * so that a test can check that everything acquired has been released.
 *
 * This file uses language constructs that come later in the course. You never
 * need to read it or to change it.
 */

#include "allocation_tracker.h"

#include <stdlib.h>
#include <string.h>

static int outstanding_blocks = 0;

void *__real_malloc(size_t size);
void *__real_calloc(size_t count, size_t size);
void *__real_realloc(void *block, size_t size);
void __real_free(void *block);

void *__wrap_malloc(size_t size) {
    void *block = __real_malloc(size);

    if (block != NULL) {
        outstanding_blocks++;
    }
    return block;
}

void *__wrap_calloc(size_t count, size_t size) {
    void *block = __real_calloc(count, size);

    if (block != NULL) {
        outstanding_blocks++;
    }
    return block;
}

void *__wrap_realloc(void *block, size_t size) {
    void *resized_block = __real_realloc(block, size);

    if (block == NULL && resized_block != NULL) {
        outstanding_blocks++;
    }
    return resized_block;
}

void __wrap_free(void *block) {
    if (block != NULL) {
        outstanding_blocks--;
    }
    __real_free(block);
}

char *__wrap_strdup(const char *text) {
    char *copy = __wrap_malloc(strlen(text) + 1);

    if (copy != NULL) {
        strcpy(copy, text);
    }
    return copy;
}

int outstanding_allocation_count(void) {
    return outstanding_blocks;
}
