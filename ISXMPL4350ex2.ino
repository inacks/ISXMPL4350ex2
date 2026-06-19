/*
 * IS4350 I2C Modbus TCP Server - Arduino Uno Example
 * ====================================================
 * This example is powered by the IS4350, a dedicated I2C Modbus TCP Server
 * chip that lets any microcontroller become a Modbus TCP server with just
 * two wires (SDA + SCL) and a few lines of code.
 *
 * No TCP/IP stack. No Ethernet library. No timers. No extra pins.
 * The IS4350 handles everything — your code just reads and writes registers
 * over I2C, and the chip does the rest.
 *
 * Perfect for sensors, actuators, and any industrial equipment that
 * needs to expose process data (speed, torque, current, status...) over
 * a standard Modbus TCP network.
 *
 * Learn more, get the datasheet, and buy at:
 *   https://www.inacks.com/is4350
 *
 * ---------------------------------------------------------------------------
 * What this example does:
 *   - Brings the Modbus server online via DHCP (no network config needed)
 *   - Waits until the server is online and ready
 *   - Every second writes an incrementing value to Holding Register 0 (HOLD_0)
 *
 * Wiring:
 *   Arduino Uno A4 (SDA) -> IS4350 SDA  (with pull-up resistor to 3.3V/5V)
 *   Arduino Uno A5 (SCL) -> IS4350 SCL  (with pull-up resistor to 3.3V/5V)
 *   ADR pin on IS4350 tied to GND -> I2C address 24 (0x18)
 *
 * Notes:
 *   - The IS4350 memory map uses 16-bit register addresses and 16-bit
 *     register values, transmitted MSB first (most significant byte first).
 *   - GO_ONLINE (register 65513) is only accessible via I2C and is NOT
 *     stored in flash, so it must be set to 1 every power-up.
 *   - The datasheet recommends leaving at least 100 ms between
 *     consecutive I2C operations.
 */

#include <Wire.h>

const uint8_t  IS4350_ADDR   = 24;    // I2C address (ADR pin tied to GND)
const uint16_t REG_HOLD_0    = 0;     // Holding Register 0
const uint16_t REG_GO_ONLINE = 65513; // Modbus server on/off (I2C only)
const uint16_t REG_STATUS    = 65514; // Network status (low byte: 5 = online)

uint16_t counter = 0;

void is4350WriteWord(uint16_t regAddr, uint16_t value) {
  Wire.beginTransmission(IS4350_ADDR);
  Wire.write((uint8_t)(regAddr >> 8));
  Wire.write((uint8_t)(regAddr & 0xFF));
  Wire.write((uint8_t)(value >> 8));
  Wire.write((uint8_t)(value & 0xFF));
  Wire.endTransmission();
}

uint16_t is4350ReadWord(uint16_t regAddr) {
  Wire.beginTransmission(IS4350_ADDR);
  Wire.write((uint8_t)(regAddr >> 8));
  Wire.write((uint8_t)(regAddr & 0xFF));
  Wire.endTransmission(false);
  Wire.requestFrom(IS4350_ADDR, (uint8_t)2);
  uint16_t value = 0;
  if (Wire.available() >= 2) {
    value = ((uint16_t)Wire.read() << 8) | Wire.read();
  }
  return value;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(1000); // Give some time for the IS4350 to be ready.
  is4350WriteWord(REG_GO_ONLINE, 1); // Bring the server online

  // Wait until the server is online (STATUS low byte == 5)
  while ((is4350ReadWord(REG_STATUS) & 0xFF) != 5) {
    Serial.print("Status: ");
    Serial.println(is4350ReadWord(REG_STATUS) & 0xFF);
    delay(500);
  }
  Serial.println("Server online.");
}

void loop() {
  is4350WriteWord(REG_HOLD_0, counter++);
  delay(1000);
}
