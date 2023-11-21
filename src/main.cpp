#include <Arduino.h>
#include <U8g2lib.h>
#include <U8x8lib.h>
#include <unistd.h>

#include "bluetooth.h"
#include "tinybms.h"
#include "util.h"
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define SERIAL_BAUD 9600
#define POLL_INTERVAL 1000

#define DISPLAY_BUTTON_PIN 7
#define DISPLAY_TIMEOUT 60000

Battery_voltage battery_voltage;
Battery_current battery_current;
Battery_soc battery_soc;
Battery_temp battery_temp;

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/* reset=*/U8X8_PIN_NONE);

Fprintable fu8x8(u8x8);

AltSoftSerial Serial1(SERIAL1_RX_PIN, SERIAL1_TX_PIN);

bool displayPowerSaveEnable = true;
unsigned long displayWakeCall;

void display_nobt_msg();

void setup() {
  Serial1.begin(SERIAL_BAUD);
  DEBUGP(F("Debug output enabled\r\n"));

  fSerial1.print(F("Starting TinyBMSDisplay\r\n"));
  u8x8.begin();

  while (init_bluetooth() < 0) {
    fSerial1.print(
        F("Bluetooth initialization failure. Retrying after sleep\r\n"));
    display_nobt_msg();
    delay(1000);
  }

  init_tinybms();
  u8x8.clear();

  pinMode(DISPLAY_BUTTON_PIN, INPUT);

  fSerial1.print(F("Init OK\r\n"));
}

void load_battery_data() {
  load_battery_voltage(&battery_voltage);
  load_battery_current(&battery_current);
  load_battery_soc(&battery_soc);
  load_battery_temp(&battery_temp);
}

#define float2str1(x) dtostrf(x, 3, 2, tmp)
#define float2str2(x) dtostrf(x, 3, 2, tmp1)

void display_data() {
  char tmp[6], tmp1[6];

  u8x8.setFont(u8x8_font_8x13B_1x2_f);
  u8x8.setCursor(0, 0);
  fu8x8.printf(F("SOC:     %hu%%   "), battery_soc.stateOfChargeHp / 1000000);

  u8x8.setFont(u8x8_font_pcsenior_r);

  u8x8.setCursor(0, 2);
  fu8x8.printf(F("Current: %sA "), float2str1(battery_current.pack_current));

  u8x8.setCursor(0, 3);
  fu8x8.printf(F("Voltage: %sV "),
               float2str1(battery_voltage.pack_voltage.fvoltage));

  u8x8.setFont(u8x8_font_chroma48medium8_u);

  u8x8.setCursor(0, 5);
  fu8x8.printf(F("CELL: %s-%s"),
               float2str1(battery_voltage.min_cell_voltage / 1000.0),
               float2str2(battery_voltage.max_cell_voltage / 1000.0));

  u8x8.setCursor(0, 6);
  fu8x8.printf(F("TEMP: %u %u %u C"), battery_temp.temp_sensor0 / 10,
               battery_temp.temp_sensor1 / 10, battery_temp.temp_sensor2 / 10);
}

void display_nobt_msg() {
  u8x8.clear();
  u8x8.setFont(u8x8_font_7x14B_1x2_f);
  u8x8.setCursor(0, 10);
  u8x8.print(F("No BT Connection"));
}

void check_display_wake() {
  
  if(digitalRead(DISPLAY_BUTTON_PIN) == HIGH) {
    DEBUGP(F("Button press detected\r\n"));
    displayWakeCall = millis();
  }

}

void loop() {
  check_display_wake();

  if(displayPowerSaveEnable && millis() - displayWakeCall > DISPLAY_TIMEOUT) {
    DEBUGP(F("Disabling display"));
    u8x8.setPowerSave(true);
  } else {
    DEBUGP(F("Enabling display"));
    u8x8.setPowerSave(false);
  }

  load_battery_data();
  display_data();
  check_display_wake();

  fSerial1.printf(F("Uptime: %us\r\n"), millis() / 1000);
  fSerial1.printf(F("Free memory: %u\r\n\r\n"), freeMemory());
  delay(POLL_INTERVAL);
}
