#ifndef DAC8568_H
# define DAC8568_H

# include <cstdint>
# include <SPI.h>

# define CTRL_REG                   0x03
# define FEAT_REG                   0x00
# define FEAT_REF_UP                0x01
# define CTRL_REG_UP                0x08
# define CTRL_REG_POWER             0x04

# define ALL_CHANNEL_UP             0x0F
# define POWER_UP                   0x00
# define POWER_DOWN_1K              0x01
# define POWER_DOWN_100K            0x02
# define POWER_DOWN_HZ              0x03

# define INTERNAL_REF_ON_STATIC     0x08000001
# define INTERNAL_REF_OFF_STATIC    0x08000000
# define INTERNAL_REF_ON_FLEXIBLE   0x09080000
# define INTERNAL_REF_OFF_FLEXIBLE  0x090C0000
# define FLEXIBLE_TO_STATIC_SWITCH  0x09000000


class DAC8568
{
    private:
        uint8_t _csPin;
        bool    _internalRefStatusStatic = false;
        bool    _internalRefStatusFlexible = false;
        void    writeData(uint32_t data);
    
    public:
        DAC8568(uint8_t chipSelectPin);
        DAC8568(const DAC8568&) = delete;               //Prevent copying via construction
        DAC8568& operator=(const DAC8568&) = delete;    //Prevent copying via assignment

        void    enableInternalRefStatic();
        void    disableInternalRefStatic();
        void    enableInternalRefFlexible();
        void    disableInternalRefFlexible();
        void    setChannel(uint8_t channel, uint16_t value);
        void    setAllChannel(uint16_t value);
        void    powerDownChannel(uint8_t channel);
        void    powerDownAllChannel();
};

#endif
