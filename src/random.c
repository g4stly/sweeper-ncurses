#include "random.h"

int randomInt(int n) {
  int divisor = RAND_MAX/(n+1);
  int rv;
  do {
    rv = rand() / divisor;
  } while (rv > n);
  return rv;
}

int randomBit() {
  return randomInt(100) % 2;
}
