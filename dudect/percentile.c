#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "percentile.h"

static inline int cmp(const int64_t *a, const int64_t *b)
{
    return (int) (*a - *b);
}

int64_t percentile(int64_t *a, double which, size_t size)
{
    qsort(a, size, sizeof(int64_t), (int (*)(const void *, const void *)) cmp);
    size_t pos = (size_t)(which * (double) size);

    assert(pos < size);
    return a[pos];
}
