/**
 * DAC8568.h
 *
 * Arduino driver for the Texas Instruments DAC8568 (and pin/register-
 * compatible DAC7568 / DAC8168) — a 16-bit, 8-channel, SPI-controlled DAC.
 *
 * Datasheet: TI SBAS430F
 *
 * Wiring:
 *   MOSI/DIN  -> board's hardware MOSI pin
 *   SCK       -> board's hardware SCK pin
 *   CS/SYNC   -> any digital pin, passed to the constructor
 *   VREFIN/VREFOUT -> tie to your external reference, OR leave unconnected
 *                     and use the internal reference (see below)
 *
 * IMPORTANT: if VREFIN/VREFOUT is wired to an external reference, do NOT
 * call enableInternalRefStatic()/enableInternalRefFlexible() — driving the
 * internal and an external reference onto the same pin simultaneously is
 * not supported by the chip (see datasheet section 8.3.1).
 */

#ifndef DAC8568_H
# define DAC8568_H

# include <SPI.h>

// -- Control register values --------------------------------------------
# define CTRL_REG                   0x03UL   // "write to input/DAC register" command
# define FEAT_REG                   0x00UL
# define FEAT_REF_UP                0x01UL   // feature-bit: keep reference powered during this write
# define CTRL_REG_UP                0x08UL
# define CTRL_REG_POWER             0x04UL   // "power up/down channels" command

# define ALL_CHANNEL_UP             0x0FUL   // channel-address field meaning "all channels"
# define POWER_UP                   0x00UL
# define POWER_DOWN_1K              0x01UL   // power-down via 1k pulldown
# define POWER_DOWN_100K            0x02UL   // power-down via 100k pulldown
# define POWER_DOWN_HZ              0x03UL   // power-down, output high-impedance

// -- Internal reference commands (see datasheet 8.3.1) -------------------
# define INTERNAL_REF_ON_STATIC     0x08000001UL
# define INTERNAL_REF_OFF_STATIC    0x08000000UL
# define INTERNAL_REF_ON_FLEXIBLE   0x09080000UL
# define INTERNAL_REF_OFF_FLEXIBLE  0x090C0000UL
# define FLEXIBLE_TO_STATIC_SWITCH  0x09000000UL   // also always powers the reference DOWN

class DAC8568
{
    private:
        uint8_t     _csPin;
        SPISettings _spiSettings;
        // Software-side bookkeeping only — mirrors what commands we've SENT to
        // the chip, not a read-back of the chip's actual state (this chip's
        // SPI interface is write-only, there is no way to query it back).
        bool        _internalRefStatusStatic = false;
        bool        _internalRefStatusFlexible = false;

        // Sends one 32-bit command word, MSB first, framed by CS low/high.
        void        writeData(uint32_t data);

        // Debug helper: prints a 32-bit value as space-grouped binary,
        // for comparing against the datasheet's bit tables by eye.
        void        showDataBits(uint32_t data);

        // Shared bit-packing for the two power-state commands.
        void        setPowerState(uint32_t powerMode, uint8_t channelMask);

        // Shared bit-packing for the two "write DAC value" commands.
        void        setDacValue(uint8_t channelField, uint16_t value);

    public:
        // clockSpeed defaults to 1MHz; raise it if the datasheet's max SPI
        // clock for your supply voltage allows and you need faster updates.
        DAC8568(uint8_t chipSelectPin, uint32_t clockSpeed = 1000000);

        // Copying is disabled: two DAC8568 objects both believing they own
        // the same physical CS pin (and independently tracking reference
        // state for the same physical chip) is a real, silent bug source.
        DAC8568(const DAC8568&) = delete;
        DAC8568& operator=(const DAC8568&) = delete;

        // -- Internal reference control (see file header re: external ref) --
        void    enableInternalRefStatic();
        void    disableInternalRefStatic();
        void    enableInternalRefFlexible();
        void    disableInternalRefFlexible();

        // -- DAC value writes --
        // channel must be 0-7; out-of-range calls are silently ignored.
        void    setChannel(uint8_t channel, uint16_t value);
        void    setAllChannel(uint16_t value);

        // -- Channel power control --
        void    powerDownChannel(uint8_t channel);
        void    powerDownAllChannel();
        void    powerUpChannel(uint8_t channel);
        void    powerUpAllChannel();
};

#endif
