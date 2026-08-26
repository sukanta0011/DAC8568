#include "DAC8568.h"
#include "Arduino.h"


//---------------------------------------//
//            Constructor                //
//---------------------------------------//
DAC8568::DAC8568(uint8_t chipSelectPin, uint32_t clockSpeed, uint8_t resolution)
{
    _csPin = chipSelectPin;
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);   // CS idle-high (active-low chip select)

    // SPI_MODE1 confirmed against real hardware.
    // For chip DAC8168, DAC7568, provide resolution value
    _spiSettings = SPISettings(clockSpeed, MSBFIRST, SPI_MODE1);
    if (resolution < 16){
        _data_shifting_bits = 16 - resolution;
    }
}


//---------------------------------------//
//          Public Methods               //
//---------------------------------------//

// Internal reference control — see file header for the external-reference
// warning before calling any of these four.
void    DAC8568::enableInternalRefStatic()
{
    if (_internalRefStatusFlexible)
    {
        // Switching Flexible -> Static also powers the reference DOWN
        // (see datasheet Table 12) — the second block below re-enables it.
        writeData(FLEXIBLE_TO_STATIC_SWITCH);
        _internalRefStatusFlexible = false;
    }
    if (!_internalRefStatusStatic)
    {
        writeData(INTERNAL_REF_ON_STATIC);
        _internalRefStatusStatic = true;
    }
}

void    DAC8568::disableInternalRefStatic()
{
    if (_internalRefStatusStatic)
    {
        writeData(INTERNAL_REF_OFF_STATIC);
        _internalRefStatusStatic = false;
    }
}

void    DAC8568::enableInternalRefFlexible()
{
    if (!_internalRefStatusFlexible)
    {
        writeData(INTERNAL_REF_ON_FLEXIBLE);
        _internalRefStatusFlexible = true;
        _internalRefStatusStatic = false;
    }
}

void    DAC8568::disableInternalRefFlexible()
{
    if (_internalRefStatusFlexible)
    {
        writeData(INTERNAL_REF_OFF_FLEXIBLE);
        _internalRefStatusFlexible = false;
    }
}

// DAC value writes — channel range-checked here, since this class is the
// one place that actually knows this specific chip has 8 channels.
void    DAC8568::setChannel(uint8_t channel, uint16_t value)
{
    if (channel > 7) { return; }
    setDacValue(channel, value);
}

void    DAC8568::powerUpChannel(uint8_t channel)
{
    if (channel > 7) { return; }
    setPowerState(POWER_UP, 1 << channel);
}

void    DAC8568::powerDownChannel(uint8_t channel)
{
    if (channel > 7) { return; }
    setPowerState(POWER_DOWN_100K, 1 << channel);
}

void    DAC8568::setAllChannel(uint16_t value) { setDacValue(ALL_CHANNEL_UP, value); }
void    DAC8568::powerUpAllChannel()           { setPowerState(POWER_UP, 0xFF); }
void    DAC8568::powerDownAllChannel()         { setPowerState(POWER_DOWN_100K, 0xFF); }


//---------------------------------------//
//         Private Methods               //
//---------------------------------------//

// Sends one 32-bit command word as 4 bytes, MSB first, CS framing the
// whole transaction. beginTransaction/endTransaction apply _spiSettings
// for just this transfer, so this DAC's mode/speed never leaks onto a
// shared bus if other SPI devices are present.
void    DAC8568::writeData(uint32_t data)
{
    // showDataBits(data);   // uncomment to print the outgoing word for
                              // comparison against the datasheet's bit tables
    SPI.beginTransaction(_spiSettings);
    digitalWrite(_csPin, LOW);
    SPI.transfer((data >> 24) & 0xFF);
    SPI.transfer((data >> 16) & 0xFF);
    SPI.transfer((data >> 8) & 0xFF);
    SPI.transfer(data & 0xFF);
    digitalWrite(_csPin, HIGH);
    SPI.endTransaction();
}

void    DAC8568::showDataBits(uint32_t data)
{
    uint32_t    mask_bit = 1;
    uint32_t    mask;

    for (uint8_t i = 32; i > 0; i--)
    {
        mask = mask_bit << (i - 1);
        Serial.print((bool)(data & mask));
        if ((i - 1) % 8 == 0)
            Serial.print(" ");
    }
    Serial.print("\n");
}


void DAC8568::setPowerState(uint32_t powerMode, uint8_t channelMask)
{
    uint32_t bitMap = 0;

    bitMap |= (CTRL_REG_POWER & 0x0F) << 24;
    bitMap |= (powerMode & 0x0F) << 8;
    bitMap |= channelMask & 0xFF;
    writeData(bitMap);
}


void DAC8568::setDacValue(uint8_t channelField, uint16_t value)
{
    uint32_t bitMap = 0;

    bitMap |= (CTRL_REG & 0x0F) << 24;
    bitMap |= (uint32_t)channelField << 20;
    bitMap |= (uint32_t)value << (4 + _data_shifting_bits);
    bitMap |= (FEAT_REF_UP & 0x0F);
    writeData(bitMap);
}
