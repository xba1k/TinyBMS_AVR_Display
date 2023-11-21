#include "tinybms.h"

#include "util.h"

ModbusMaster *modbus;

void init_tinybms() {
  modbus = new ModbusMaster();

  // Init the modbus interface
  serial->begin(MODBUS_BAUD);
  modbus->begin(TINYBMS_DEVICE_ID, *serial);
}

int readRegistersWithRetry(uint16_t idx, uint16_t count, uint16_t *dest,
                           uint8_t retrcnt) {
  do {
    uint8_t result = 0;

    if ((result = modbus->readHoldingRegisters(idx, count)) ==
        modbus->ku8MBSuccess) {
      uint16_t j = 0;
      for (j = 0; j < count; j++) {
        dest[j] = modbus->getResponseBuffer(j);
        DEBUGP("register %hhu = %hx\r\n", j, dest[j]);
      }
      break;
    } else {
      // this may be too chatty
      DEBUGP("modbus error %hhx\r\n", result);
    }

    retrcnt--;
    delay(MODBUS_RETRY_INTERVAL);

  } while (retrcnt > 0);

  return retrcnt - 1;
}

int load_battery_voltage(Battery_voltage *voltage) {
  int result = 1;

  if (readRegistersWithRetry(PACK_VOLTAGE_REGISTER_0, 2,
                             voltage->pack_voltage.ivoltage,
                             MODBUS_RETRY_COUNT) <= 0)
    result = -1;

  delay(MODBUS_INTERVAL);

  if (readRegistersWithRetry(MIN_CELL_VOLTAGE_REGISTER, 1,
                             &voltage->min_cell_voltage,
                             MODBUS_RETRY_COUNT) <= 0)
    result = -1;

  delay(MODBUS_INTERVAL);

  if (readRegistersWithRetry(MAX_CELL_VOLTAGE_REGISTER, 1,
                             &voltage->max_cell_voltage,
                             MODBUS_RETRY_COUNT) <= 0)
    result = -1;

  delay(MODBUS_INTERVAL);

  return result;
}

int load_battery_current(Battery_current *current) {
  int result = 1;

  if (readRegistersWithRetry(PACK_CURRENT_REGISTER_0, 2,
                             (uint16_t *)&current->pack_current,
                             MODBUS_RETRY_COUNT) <= 0)
    result = -1;

  delay(MODBUS_INTERVAL);

  return result;
}

int load_battery_soc(Battery_soc *soc) {
  int result = 1;

  if (readRegistersWithRetry(PACK_SOC_REGISTER_0, 2,
                             (uint16_t *)&(soc->stateOfChargeHp),
                             MODBUS_RETRY_COUNT) <= 0)
    result = -1;

  delay(MODBUS_INTERVAL);

  return result;
}

int load_battery_temp(Battery_temp *temp) {
  int result = 1;

  if (readRegistersWithRetry(TEMP_SENSOR_0_REGISTER, 1, &temp->temp_sensor0,
                             MODBUS_RETRY_COUNT) <= 0)
    result = -1;

  delay(MODBUS_INTERVAL);

  if (readRegistersWithRetry(TEMP_SENSOR_1_REGISTER, 1, &temp->temp_sensor1,
                             MODBUS_RETRY_COUNT) <= 0)
    result = -1;

  delay(MODBUS_INTERVAL);

  if (readRegistersWithRetry(TEMP_SENSOR_2_REGISTER, 1, &temp->temp_sensor2,
                             MODBUS_RETRY_COUNT) <= 0)
    result = -1;

  delay(MODBUS_INTERVAL);

  return result;
}
