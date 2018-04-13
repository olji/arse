#include "panic.h"

void panic(){
  long *segfault = (long*)0x0;
  *segfault = 3;
}
