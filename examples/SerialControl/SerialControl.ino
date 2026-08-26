/**
 * SerialControl.ino
 *
 * Sets a DAC channel value from the Serial Monitor: type "<channel> <value>"
 * and press enter, e.g. "3 25000". Also continuously reports the analog
 * voltage read back on analogPin, without blocking on Serial input.
 *
 * Non-blocking by design (no delay() in loop() other than the debug print
 * inside show_analog_voltage): a small AVR board like the Nano has too
 * little RAM to run an RTOS, so responsiveness comes from the millis()-based
 * polling pattern below, not real threads.
 */

#include <SPI.h>
#include "DAC8568.h"

const uint8_t   chipSelectPin = 10;
const int       analogPin = A0;
uint32_t        lastVoltageCheck = 0;
const uint32_t  voltageCheckInterval = 100;
DAC8568         dac1(chipSelectPin);

bool is_serial_ready(uint8_t timeout = 5000)
{
    uint32_t startTime = millis();

    while (!Serial)
    {
        if ((millis() - startTime) > (uint32_t)timeout)
        {
            return false;
        }
        delay(10);
    }
    return true;
}

void show_analog_voltage()
{
    uint16_t rawValue = analogRead(analogPin);
    float    voltage = rawValue * (5.0 / 1023.0);
    Serial.println(voltage);
    delay(100);
}

void setup() {
    Serial.begin(9600);
    SPI.begin();

    if (is_serial_ready())
    {
        dac1.powerUpAllChannel();
        delay(500);
    }
}

void loop() {
    if (Serial.available() > 0)
    {
        // Locals, not globals: only ever needed for the duration of this
        // one command, no reason to permanently reserve RAM for them.
        uint8_t  channel;
        uint16_t value;

        String line = Serial.readStringUntil('\n');
        int matched = sscanf(line.c_str(), "%hhu %hu", &channel, &value);
        if (matched == 2) {
            dac1.setChannel(channel, value);
        } else {
            Serial.println("Invalid command format — expected: <channel> <value>");
        }
    }

    if (millis() - lastVoltageCheck >= voltageCheckInterval) {
        show_analog_voltage();
        lastVoltageCheck = millis();
    }
}
