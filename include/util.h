#ifndef UTIL_H
#define UTIL_H

using namespace std;

#include <Arduino.h>
#include <AltSoftSerial.h>
#include <stdarg.h>

#define DEBUG 1

#ifdef DEBUG
#define DEBUGP(...)             \
  do {                          \
    fSerial1.printf(__VA_ARGS__); \
  } while (false)
#else
#define DEBUGP(...) \
  do {              \
  } while (false)
#endif

#define SERIAL1_TX_PIN 8
#define SERIAL1_RX_PIN 9

class Fprintable : public Print {
 public:
  Fprintable(Print &p);
  size_t printf(const char *fmt, ...);

  size_t write(uint8_t b) override { return this->_print.write(b); }
  size_t write(const uint8_t *buffer, size_t size) override {
    return this->_print.write(buffer, size);
  }
  int availableForWrite() override { return this->_print.availableForWrite(); }
  void flush() override { this->_print.flush(); }

 protected:
  Print &_print;
};

extern AltSoftSerial Serial1;
extern Fprintable fSerial1;

int freeMemory();

#endif
