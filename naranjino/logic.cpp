#include "logic.h"
#include <WProgram.h>

int get_free_memory() {
  int size = 1024; // Use 2048 with ATmega328
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);
  return size;
}
