#ifndef SIMPLE_ALLOCATOR_H
#define SIMPLE_ALLOCATOR_H

#include "lock.h"

// why define in such a complicated way
// for simple in kalloc()
struct Page {
  char allocated;
};

void simple_allocator_init();
struct Page *simple_allocate();
void simple_free(struct Page *p);
void simple_test();

#endif /* SIMPLE_ALLOCATOR_H */