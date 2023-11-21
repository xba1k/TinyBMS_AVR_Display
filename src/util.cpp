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

#define PRINT_BUFSIZ 16
size_t Fprintable::printf(const char *fmt, va_list args) {
  size_t result = 0;
  char *buf = (char *)malloc(PRINT_BUFSIZ);
  if (buf) {
    result = vsnprintf(
        buf, PRINT_BUFSIZ, fmt,
        args);
    if (result > PRINT_BUFSIZ - 1) {
      buf = (char *)realloc(buf, result + 1);
      vsnprintf(buf, result + 1, fmt, args);
    }

    result = _print.write(buf);
    free(buf);
  }
  return result;
}

size_t Fprintable::printf(const char *fmt, ...) {
  size_t result = 0;
  va_list argp;
  va_start(argp, fmt);
  result = this->printf(fmt, argp);
  va_end(argp);
  return result;
}

size_t Fprintable::printf(const __FlashStringHelper *ifsh, ...) {
  size_t result = 0;
  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
  char *buf = (char *)malloc(PRINT_BUFSIZ);
  if (buf) {
    va_list argp;
    va_start(argp, ifsh);
    result = vsnprintf_P(
        buf, PRINT_BUFSIZ, p,
        argp);
    va_end(argp);
    if (result > PRINT_BUFSIZ - 1) {
      buf = (char *)realloc(buf, result + 1);
      va_start(argp, ifsh);
      vsnprintf_P(buf, result + 1, p, argp);
      va_end(argp);
    }

    result = _print.write(buf);
    free(buf);
  }
  return result;
}
