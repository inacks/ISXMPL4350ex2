# IS4350 I2C Modbus TCP Server – Arduino Uno Example

This example is powered by the IS4350, a dedicated I²C Modbus TCP Server chip that lets any microcontroller become a Modbus TCP server with just two wires (SDA + SCL) and a few lines of code.

No TCP/IP stack. No Ethernet library. No timers. No extra pins.  
The IS4350 handles everything — your code just reads and writes registers over I²C, and the chip does the rest.

Perfect for sensors, actuators, and any industrial equipment that needs to expose process data (speed, torque, current, status...) over a standard Modbus TCP network.

Learn more, get the datasheet, and buy at:  
https://www.inacks.com/is4350

---

## What this example does:
- Brings the Modbus server online via DHCP (no network configuration needed)
- Waits until the server is online and ready
- Every second writes an incrementing value to Holding Register 0 (HOLD_0)

---

## Wiring:
- Arduino Uno A4 (SDA) → IS4350 SDA (with pull-up resistor to 3.3V/5V)
- Arduino Uno A5 (SCL) → IS4350 SCL (with pull-up resistor to 3.3V/5V)
- ADR pin on IS4350 tied to GND → I²C address 24 (0x18)

---

## Notes:
- The IS4350 memory map uses 16-bit register addresses and 16-bit register values, transmitted MSB first (most significant byte first).
- `GO_ONLINE` (register 65513) is only accessible via I²C and is not stored in flash, so it must be set to 1 every power-up.
- The datasheet recommends leaving at least 100 ms between consecutive I²C operations.
