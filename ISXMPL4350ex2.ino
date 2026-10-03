/*
 * ISXMPL4350ex2: IS4350 - Simple Arduino Uno Example
 * --------------------------------------------------
 * - Uses DHCP (default, no configuration needed)
 * - Brings the Modbus server online (GO_ONLINE = 1)
 * - Increments Holding Register 0 (HOLD_0) by 1 every second
 *
 * Wiring:
 *   Arduino Uno A4 (SDA) -> IS4350 SDA (4.7 kOhm pull-up to 5 V)
 *   Arduino Uno A5 (SCL) -> IS4350 SCL (4.7 kOhm pull-up to 5 V)
 *   Arduino Uno GND      -> IS4350 VSS
 *   ADR pin on IS4350 tied to GND -> I2C address 24 (0x18)
 *   SPD pin on IS4350 tied to GND -> 100 kHz (Arduino Wire default)
 *
 * Notes:
 *   - Register addresses and values are 16-bit, sent MSB first.
 *   - GO_ONLINE is 0 after every power-up, so set it at startup.
 *   - Leave a few milliseconds between consecutive I2C operations.
 */

#include <Wire.h>

const uint8_t  IS4350_ADDR   = 24;    // I2C address (ADR pin tied to GND)
const uint16_t REG_HOLD_0    = 0;     // Holding Register 0
const uint16_t REG_GO_ONLINE = 65513; // Modbus server on/off (I2C only)
const uint16_t REG_STATUS    = 65514; // Server status (low byte: 5 = ready)

uint16_t counter = 0;

void is4350WriteRegister(uint16_t regAddr, uint16_t value) {
  Wire.beginTransmission(IS4350_ADDR);
  Wire.write((uint8_t)(regAddr >> 8));    // Register address, MSB first
  Wire.write((uint8_t)(regAddr & 0xFF));
  Wire.write((uint8_t)(value >> 8));      // Value, MSB first
  Wire.write((uint8_t)(value & 0xFF));
  Wire.endTransmission();
  delay(10);                              // A few ms between I2C operations
}

uint16_t is4350ReadRegister(uint16_t regAddr) {
  Wire.beginTransmission(IS4350_ADDR);
  Wire.write((uint8_t)(regAddr >> 8));    // Register address, MSB first
  Wire.write((uint8_t)(regAddr & 0xFF));
  Wire.endTransmission(false);            // Repeated start (no stop)
  Wire.requestFrom(IS4350_ADDR, (uint8_t)2);
  uint8_t msb = Wire.read();              // Value, MSB first
  uint8_t lsb = Wire.read();
  delay(10);                              // A few ms between I2C operations
  return ((uint16_t)msb << 8) | lsb;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(1000);                               // Let the IS4350 start up

  is4350WriteRegister(REG_HOLD_0, counter);  // Valid data before going online
  is4350WriteRegister(REG_GO_ONLINE, 1);     // Bring the server online

  // Wait until the server is ready and listening (STATUS low byte = 5)
  uint8_t status = 0;
  while (status != 5) {
    delay(500);
    status = is4350ReadRegister(REG_STATUS) & 0xFF;
    Serial.print("STATUS: ");
    Serial.println(status);
  }
  Serial.println("Server online.");
}

void loop() {
  counter++;
  is4350WriteRegister(REG_HOLD_0, counter);
  Serial.print("HOLD_0 = ");
  Serial.println(counter);
  delay(1000);
}