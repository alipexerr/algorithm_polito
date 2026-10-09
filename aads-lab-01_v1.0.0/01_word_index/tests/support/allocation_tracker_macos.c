#include "allocation_tracker.h"

#include <stdlib.h>
#include <string.h>

static int outstanding_blocks = 0;

void *tracked_malloc(size_t size)
{
    void *block = malloc(size);
    if (block != NULL)
        outstanding_blocks++;
    return block;
}

void *tracked_calloc(size_t count, size_t size)
{
    void *block = calloc(count, size);
    if (block != NULL)
        outstanding_blocks++;
    return block;
}

void *tracked_realloc(void *block, size_t size)
{
    void *resized = realloc(block, size);

    if (block == NULL && resized != NULL)
        outstanding_blocks++;

    return resized;
}

void tracked_free(void *block)
{
    if (block != NULL)
        outstanding_blocks--;

    free(block);
}

char *tracked_strdup(const char *text)
{
    size_t size = strlen(text) + 1;
    char *copy = tracked_malloc(size);

    if (copy != NULL)
        memcpy(copy, text, size);

    return copy;
}

int outstanding_allocation_count(void)
{
    return outstanding_blocks;
}