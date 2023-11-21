#include "bluetooth.h"

#include <Arduino.h>

#include "util.h"

int bt_cmd(const char *cmd, char *resp) {
  int bytes = 0;

  btSerial->write(cmd);
  btSerial->flush();
  delay(100);

  int availBytes = btSerial->available();

  if (availBytes > 0) {
    bytes = btSerial->readBytes(resp, availBytes);
    resp[bytes] = '\0';
    fSerial1.printf("%s\r\n", resp);
  }

  return bytes;
}

#define BT_BUFSIZ 64

int init_bluetooth(void) {

  char *bt_buf = (char *)malloc(BT_BUFSIZ);

  DEBUGP(F("init_bt\r\n"));

  btSerial->begin(BT_BAUD);

  // flush input
  while (btSerial->available() > 0) {
    btSerial->read();
  }

  // Check if in the AT mode
  if (bt_cmd("AT\r\n", bt_buf) > 0) {
    if (strncmp(bt_buf, "OK\r\n", BT_BUFSIZ) != 0) {
      fSerial1.printf(F("Unexpected BT response: %s\r\n"), bt_buf);
      free(bt_buf);
      return -1;
    }
  } else {
      fSerial1.println(F("BT already in data mode?\r\n"));
      free(bt_buf);
      return 0;
  }

  // Check the version, just in case...
  bt_cmd("AT+VERSION\r\n", bt_buf);

  // Set role to Master
  // bt_cmd("AT+ROLE=1\r\n", bt_buf);

  bt_cmd("AT+BIND?\r\n", bt_buf);
  // Bind to target address
  sprintf(bt_buf, "AT+BIND=%s\r\n", TINYBMS_ADDRESS);
  bt_cmd(bt_buf, bt_buf);

  bt_cmd("AT+PSWD?\r\n", bt_buf);
  // Set pin
  sprintf(bt_buf, "AT+PSWD=\"%s\"\r\n", TINYBMS_PIN);
  bt_cmd(bt_buf, bt_buf);

  bt_cmd("AT+UART?\r\n", bt_buf);
  // Set the connection speed
  sprintf(bt_buf, "AT+UART=%li,0,0\r\n", MODBUS_BAUD);
  bt_cmd(bt_buf, bt_buf);

  // Request a connection
  bt_cmd("AT+CMODE=0\r\n", bt_buf);

  // Set role to Master
  bt_cmd("AT+ROLE=1\r\n", bt_buf);

  // Get out of the command mode
  bt_cmd("AT+RESET\r\n", bt_buf);

  free(bt_buf);
  return 1;
}
