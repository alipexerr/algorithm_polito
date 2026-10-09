#ifndef ALLOCATION_TRACKER_MACOS_H
#define ALLOCATION_TRACKER_MACOS_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void *tracked_malloc(size_t size);
void *tracked_calloc(size_t count, size_t size);
void *tracked_realloc(void *block, size_t size);
void tracked_free(void *block);
char *tracked_strdup(const char *text);

#define malloc  tracked_malloc
#define calloc  tracked_calloc
#define realloc tracked_realloc
#define free    tracked_free
#define strdup  tracked_strdup

#endif