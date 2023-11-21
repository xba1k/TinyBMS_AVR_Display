#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <AltSoftSerial.h>
#include <Arduino.h>

#include "tinybms.h"

#define btSerial (&Serial)

#define BT_BAUD 38400

int init_bluetooth(void);

#endif
