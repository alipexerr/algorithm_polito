#ifndef ALLOCATION_TRACKER_H
#define ALLOCATION_TRACKER_H

/**
 * @brief Number of memory blocks acquired and not yet released.
 *
 * The count covers every allocation performed by the code linked into the test
 * executable, from the first instruction of the program.
 *
 * @return Zero when every acquired block has been released.
 */
int outstanding_allocation_count(void);

#endif
