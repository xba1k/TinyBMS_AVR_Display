# TinyBMS_AVR_Display

## Overview

This is the AVR version of the [TinyBMS_ESP32_Display](https://github.com/xba1k/TinyBMS_ESP32_Display) TinyBMS monitor. A bit more compact and quirky.

## Setup

This project was created using VSCode/PlatformIO. BMS BT address is harcoded in bluetooth.h. Since the BT module will remember the paired BMS, it can be configured externally before use. Otherwise the module should be placed into AT mode (using a button, or KEY/EN pin), and microcontroller then reset to initialize the BT module. Serial console is at 9600bps.

Since the BT module is connected to the hardware serial RX/TX lines, BT module has to be powered off when firmware is flashed.

# Hardware

This was developed primarily for use with Arduino Nano clones. HC-05 module provides Bluetooth connectivity. Additional (optional) USB to Serial adapter is needed to watch the log output.

Pinout :

A4 -> OLED SDA  
A5 -> OLED SCK  
  
D0 (TX) -> BT RX  
D1 (RX) -> BT TX  
  
D9 -> USB serial RX  

D0 should be connected via a divider or a logic level shifter since HC-05 expects 3.3V input. ESP-01 adapter can be used as well.

![alt text](https://github.com/xba1k/TinyBMS_AVR_Display/blob/main/tinybmsdisplay.jpg?raw=true)

# Todo

There are some Modbus reliability issues. It's worked around with retries, but that results in slower than realtime display updates.

