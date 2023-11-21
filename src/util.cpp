#include "util.h"

#include <Arduino.h>

Fprintable fSerial1(Serial1);

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char *sbrk(int incr);
#else   // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char *>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else   // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

Fprintable::Fprintable(Print &p) : _print(p) {}

size_t Fprintable::printf(const char *fmt, ...) {
  size_t result = 0;
  char *buf = (char *)malloc(128);
  if (buf) {
    va_list argp;
    va_start(argp, fmt);
    vsprintf(buf, fmt, argp);
    result = _print.print(buf);
    va_end(argp);
    free(buf);
  }
  return result;
}
