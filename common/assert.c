#include "panic.h"
void assert(int correct) {
  if (correct == 1) {
    ;
  } else {
    panic("assert failed");
  }
}