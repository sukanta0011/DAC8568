/**
 * BasicUsage.ino
 *
 * Minimal example: power up channel 0, write a value, read it back through
 * an ADC pin wired to the DAC's output for verification, power the channel
 * back down.
 *
 * Board tested: Arduino Nano (ATmega328P)
 *   MOSI/DIN -> pin 11 (hardware SPI, fixed on this board)
 *   SCK      -> pin 13 (hardware SPI, fixed on this board)
 *   CS       -> pin 10 (any digital pin works — see DAC8568.h)
 *
 * Assumes the DAC's VREFIN/VREFOUT is wired to an EXTERNAL reference — the
 * internal reference is deliberately not enabled here. If your board uses
 * the internal reference instead, see DAC8568.h for the warning about
 * never having both enabled at once.
 */

#include <SPI.h>
#include "DAC8568.h"

const uint8_t chipSelectPin = 10;
const int     analogPin = A0;   // wire to the DAC's channel-0 output to verify
uint16_t      rawValue = 0;
float         voltage = 0;
DAC8568       dac1(chipSelectPin);

void show_analog_voltage()
{
    rawValue = analogRead(analogPin);
    voltage = rawValue * (5.0 / 1023.0);   // 10-bit ADC, 5V reference — adjust
                                             // for your board's actual ADC
    Serial.println(voltage);
    delay(100);
}

void setup() {
    Serial.begin(9600);
    SPI.begin();

    dac1.powerUpChannel(0);
    delay(500);

    dac1.setChannel(0, 10000);
    delay(500);
    show_analog_voltage();
    delay(1000);

    dac1.powerDownChannel(0);
    delay(500);
    show_analog_voltage();   // should read ~0V now
}

void loop() {
    delay(1000);
}
