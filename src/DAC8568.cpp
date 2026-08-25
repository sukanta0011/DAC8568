#include "DAC8568.h"


//---------------------------------------//
//            Constructor                //
//---------------------------------------//
DAC8568::DAC8568(uint8_t chipSelectPin)
{
    _csPin = chipSelectPin;
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);
}


//---------------------------------------//
//          Public Methods               //
//---------------------------------------//
void    DAC8568::enableInternalRefStatic()
{
    if (_internalRefStatusFlexible)
    {
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

void    DAC8568::setChannel(uint8_t channel, uint16_t value)
{
    uint32_t    bitMap = 0;

    bitMap |= (CTRL_REG & 0x0F) << 24;
    bitMap |= channel << 20;
    bitMap |= value << 4;
    bitMap |= (FEAT_REF_UP & 0x0F);
    writeData(bitMap);
}

void    DAC8568::setAllChannel(uint16_t value)
{
    uint32_t    bitMap = 0;

    bitMap |= (CTRL_REG & 0x0F) << 24;
    bitMap |= ALL_CHANNEL_UP << 20;
    bitMap |= value << 4;
    bitMap |= (FEAT_REF_UP & 0x0F);
    writeData(bitMap);
}

void    DAC8568::powerDownChannel(uint8_t channel)
{
    uint32_t    bitMap = 0;
    bitMap |= (CTRL_REG_POWER & 0x0F) << 24;
    bitMap |= (POWER_DOWN_100K & 0x0F) << 8;
    bitMap |= (1 << channel) & 0xFF;
    writeData(bitMap);
}

void    DAC8568::powerDownAllChannel()
{
    uint32_t    bitMap = 0;
    bitMap |= (CTRL_REG_POWER & 0x0F) << 24;
    bitMap |= (POWER_DOWN_100K & 0x0F) << 8;
    bitMap |= 0xFF;
    writeData(bitMap);
}


//---------------------------------------//
//         Private Methods               //
//---------------------------------------//
void    DAC8568::writeData(uint32_t data)
{
    digitalWrite(_csPin, LOW);
    SPI.transfer((data >> 24) & 0xFF);
    SPI.transfer((data >> 16) & 0xFF);
    SPI.transfer((data >> 8) & 0xFF);
    SPI.transfer(data & 0xFF);
    digitalWrite(_csPin, HIGH);
}