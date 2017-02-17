/* safeMalloc.h
 * Some wrappers around alloc functions that check if the allocation succeeded.
 */

#ifndef _SAFE_MALLOC_H
#define _SAFE_MALLOC_H

#include <stdlib.h>

void* safeMalloc(size_t num);

void* safeCalloc(size_t num, size_t size);

void* safeRealloc(void* p, size_t num);

#endif
