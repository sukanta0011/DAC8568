# DAC8568

Arduino driver for the Texas Instruments **DAC8568** (also compatible with
the pin/register-identical **DAC8168** and **DAC7568**) — a 16-bit,
8-channel, SPI-controlled DAC. Written for our lab's data acquisition
system PCB.

Datasheet: [TI SBAS430F](https://www.ti.com/lit/ds/symlink/dac8568.pdf)

## Status

`v0.1.0` — tested and working on real hardware (Arduino Nano, external
reference), not yet used in a long-running deployment. Version will move to
`1.0.0` once it's been running reliably in the actual DAQ system for a
while.

## Features

- Write any channel (`0`–`7`) or all channels at once, full 16-bit range
- Per-channel and all-channel power up/power down
- Internal reference control (Static and Flexible modes) — for boards that
  use the internal reference instead of an external one
- Copy-safety: `DAC8568` objects can't be accidentally copied (see
  [Design notes](#design-notes))
- Channel bounds are checked inside the library, not left to the caller

## Wiring

| DAC8568 pin | Connects to |
|---|---|
| `DIN` (MOSI) | Board's hardware MOSI pin |
| `SCLK` | Board's hardware SCLK pin |
| `SYNC` (CS) | Any digital pin — passed to the constructor |
| `VREFIN`/`VREFOUT` | External reference, **or** left unconnected if using the internal reference |

> **Do not** wire an external reference to `VREFIN/VREFOUT` and also call
> `enableInternalRefStatic()`/`enableInternalRefFlexible()`. The datasheet
> explicitly warns against driving that pin from both sources at once
> (section 8.3.1) — pick one.

On classic AVR boards (Uno, Nano, ...), `MOSI`/`MISO`/`SCK` are fixed to
specific pins by the hardware and cannot be reassigned in software; only
`CS` is freely chosen. On boards with flexible pin muxing (e.g. ESP32),
`SPI.begin()` can optionally take explicit pin arguments — see your core's
SPI documentation.

## Installation

Copy this folder into your `Arduino/libraries/` directory, then restart
the Arduino IDE. `#include "DAC8568.h"` becomes available in any sketch.

## Usage

```cpp
#include <SPI.h>
#include "DAC8568.h"

DAC8568 dac(10);   // CS on pin 10, default 1MHz SPI clock

void setup() {
    SPI.begin();
    dac.powerUpChannel(0);
    dac.setChannel(0, 30000);   // channel 0, mid-scale-ish value
}

void loop() {}
```

See `examples/BasicUsage` for a fuller walkthrough (power up, write, read
back via an ADC pin, power down), and `examples/SerialControl` for a
non-blocking Serial command interface (`"<channel> <value>"`).

## Design notes

A few decisions worth knowing if you're extending this:

- **Copying is disabled** (`= delete` on the copy constructor and copy
  assignment operator). Two `DAC8568` objects both believing they own the
  same physical CS pin — or independently tracking reference state for the
  same physical chip — is a real, silent bug source, most likely to happen
  by accident (e.g. passing a `DAC8568` by value into a function instead
  of by reference).
- **Channel validation lives in this library, not in calling code** — this
  class is the one place that actually knows this specific chip has 8
  channels; pushing that check out to every call site would mean
  re-implementing it everywhere, and it'd need updating in every one of
  those places if the chip were ever swapped for a different DAC.
- **SPI mode is `SPI_MODE1`**, confirmed against real hardware — the DAC
  did not respond correctly under the default `SPI_MODE0`, despite the
  outgoing command word being independently verified correct (see
  `showDataBits()`). If you're adapting this for a different board/wiring,
  re-verify against a scope or logic analyzer before assuming mode1 is
  still correct.
- **All internal 32-bit constants are shift-safe on AVR.** On AVR, `int`
  is only 16 bits, and shifting a promoted `uint8_t`/`uint16_t` value left
  by 16+ bits is undefined behavior — it silently drops the upper bits
  rather than erroring. Every macro that gets shifted uses a `UL` suffix,
  and every runtime `channel`/`value` parameter is explicitly cast to
  `uint32_t` before shifting, for exactly this reason. This was found and
  fixed against real hardware, not caught by the compiler — this class of
  bug is worth watching for in any new code added here.

## License

TODO
