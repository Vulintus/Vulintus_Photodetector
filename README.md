# Vulintus_Photodetector

An Arduino-compatible library for implementing light-based (typically infrared) beam-break and reflectance sensors.

_Disclaimer: This README file was AI-generated, but checked for accuracy by a human._

## Features

- **Auto-Thresholding**: Automatically adjusts detection thresholds based on ambient conditions
- **Digital Low-Pass Filtering**: Optional filtering to reduce noise and false triggers
- **Configurable Sensitivity**: Adjustable detection sensitivity from 0-100%
- **Multi-Photobeam Support**: Manage multiple photodetectors with shared status bitmasks
- **Polarity Control**: Support for both active-high and active-low sensor configurations
- **PWM Emitter Control**: Optional PWM control for IR emitter brightness
- **Historical Min/Max Tracking**: Monitors signal history for adaptive thresholding

## Dependencies

This library requires the [Vulintus_Digital_Filter](https://github.com/Vulintus/Vulintus_Digital_Filter) library. Install it before using this library.

## Installation

1. Download or clone this repository into your Arduino libraries folder
2. Download and install the [Vulintus_Digital_Filter](https://github.com/Vulintus/Vulintus_Digital_Filter) library
3. Restart the Arduino IDE

## Quick Start

### Single Photobeam Example

```cpp
#include <Vulintus_Photodetector.h>

#define BEAM_IN A0
Vulintus_Photodetector beam(BEAM_IN);

void setup() {
  Serial.begin(115200);
  
  beam.set_sensitivity(0.8);      // 80% sensitivity
  beam.lowpass_cutoff(20);        // 20 Hz low-pass filter
  beam.begin();
}

void loop() {
  if (beam.read()) {              // Returns true if status changed
    Serial.print("Beam ");
    Serial.println(beam.is_blocked ? "BLOCKED" : "CLEAR");
  }
}
```

### Multiple Photobeams with Bitmask

```cpp
#include <Vulintus_Photodetector.h>

#define BEAM_L A2
#define BEAM_C A1
#define BEAM_R A0

const uint8_t NUM_BEAMS = 3;

Vulintus_Photodetector beam[] = {
  Vulintus_Photodetector(BEAM_L, 0),
  Vulintus_Photodetector(BEAM_C, 1),
  Vulintus_Photodetector(BEAM_R, 2),
};

void setup() {
  Serial.begin(115200);
  
  for (uint8_t i = 0; i < NUM_BEAMS; i++) {
    beam[i].set_sensitivity(0.8);
    beam[i].begin();
  }
}

void loop() {
  bool changed = false;
  for (uint8_t i = 0; i < NUM_BEAMS; i++) {
    changed |= beam[i].read();
  }
  
  if (changed) {
    Serial.print("Bitmask: ");
    Serial.println(beam[0].bitmask, BIN);  // Static variable shared across all instances
  }
}
```

## API Reference

### Constructor

```cpp
Vulintus_Photodetector(uint8_t pin_detector, uint8_t beam_index = 0, bool blocked_val = HIGH)
```

- `pin_detector`: Analog input pin for the photodetector
- `beam_index`: Index for multi-beam configurations (0-7)
- `blocked_val`: Polarity setting - `HIGH` (default) means blocked beam reads high, `LOW` means blocked beam reads low

### Core Functions

#### `void begin()`
Initialize the photodetector. Call this in `setup()` after configuring parameters.

#### `bool read()`
Update the photodetector status. Returns `true` if the status changed, `false` otherwise.

### Configuration Functions

#### `void set_sensitivity(float thresh_fl)`
Set detection sensitivity (0.0 to 1.0, where higher values are more sensitive). Default is 0.5.

#### `void set_thresh(uint16_t thresh_adc)`
Manually set the threshold in ADC ticks. Disables auto-thresholding.

#### `uint16_t get_thresh()`
Get the current threshold value.

#### `float lowpass_cutoff(float new_freq)`
Set the low-pass filter cutoff frequency in Hz. Set to 0 to disable filtering.

#### `float lowpass_cutoff()`
Get the current low-pass filter cutoff frequency.

#### `void set_emitter_pin(uint8_t pin_emitter)`
Assign a PWM pin to control the IR emitter.

#### `void set_emitter_pwm(uint8_t pwm_val)`
Set emitter brightness (0-255).

#### `uint8_t reset()`
Reset the historical min/max values. Useful when ambient conditions change significantly.

### Public Variables

- `bool is_blocked`: Current blocked/unblocked status
- `static uint8_t bitmask`: Shared status bitmask across all photodetector instances (bit positions correspond to beam_index)
- `uint8_t index`: Beam index for multi-beam configurations (0-7)
- `bool polarity`: Detection polarity - `HIGH` means blocked beam reads high, `LOW` means blocked beam reads low (default: `HIGH`)
- `uint16_t reading`: Current ADC reading
- `uint32_t read_time`: Timestamp of last reading (microseconds)
- `uint16_t history[2]`: Historical minimum [0] and maximum [1] ADC values
- `uint16_t min_range`: Minimum required range between min and max (default 100)
- `uint16_t reset_timeout`: Timeout for resetting history in milliseconds (default 30000)
- `bool auto_thresh`: Enable/disable auto-thresholding (default true)

## Examples

The library includes two examples:

1. **Vulintus_Photobeam_Filter_Test**: Demonstrates the low-pass filter feature with serial plotter output
2. **Vulintus_Three_Photobeam_Test**: Shows multi-beam configuration with status bitmask

Access them via `File > Examples > Vulintus Photodetector Library` in the Arduino IDE.

## How Auto-Thresholding Works

The library continuously tracks the minimum and maximum ADC values from the photodetector. The threshold is automatically calculated as:

```
threshold = min + (max - min) × sensitivity
```

This allows the system to adapt to varying ambient light conditions and sensor characteristics without manual calibration.

## Hardware Compatibility

- Compatible with all Arduino-compatible boards
- Requires analog input capability
- Tested with infrared photobeam sensors and reflectance sensors

## License

Copyright (c) 2024-2025, Vulintus, Inc. All rights reserved.

See [license.txt](license.txt) for details.

## Support

For issues, questions, or contributions:
- GitHub: [https://github.com/Vulintus/Vulintus_Photodetector](https://github.com/Vulintus/Vulintus_Photodetector)
- Email: info@vulintus.com

## Version History

- **0.1.0** (2025-06-06): Initial release as standalone library (previously part of Vulintus_OmniTrak)
