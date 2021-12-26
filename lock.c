
#include "lock.h"

void init_lock(struct spinlock * s) {
    s->locked = 0;
}

// TODO:
// Acquire the spinlock
// Must be used with release()
void acquire(struct spinlock* s) {
    ;
}

// TODO:
// Release the spinlock 
// Must be used with acquire()
void release(struct spinlock* s) {
    ;
}