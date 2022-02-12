
#include "lock.h"

#include "log.h"

void init_lock(struct spinlock* s) { s->locked = 0; }

// Check whether this cpu is holding the lock.
// Interrupts must be off.
int holding(struct spinlock* s) { return s->locked; }

// Acquire the spinlock
// Must be used with release()
void acquire(struct spinlock* s) {
  if (holding(s)) {
    panic("re-lock");
  }
  while (__sync_lock_test_and_set(&s->locked, 1) != 0)
    ;
  __sync_synchronize();
}

// Release the spinlock
// Must be used with acquire()
void release(struct spinlock* s) {
  if (!holding(s)) {
    panic("re-release");
  };
  __sync_synchronize();
  __sync_lock_release(&s->locked);
}