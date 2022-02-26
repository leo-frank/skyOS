#ifndef LOCK_H
#define LOCK_H

#include "type.h"

struct spinlock {
  uint locked;
};

void init_lock(struct spinlock *);

// Acquire the spinlock
// Must be used with release()
void acquire(struct spinlock *);

// Release the spinlock
// Must be used with acquire()
void release(struct spinlock *);

#endif /* LOCK_H */
