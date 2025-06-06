/*  Vulintus_Photobeam_Filter_Test.ino

    copyright 2025, Vulintus, Inc.

    Example demonstrating the effects of the optional low-pass filter in the 
    Vulintus_Photodetector class.

    UPDATE LOG:
      2025-06-06 - Drew Sloan - Example first created.

*/


// Included libraries. // 
#include <Vulintus_Photodetector.h>     // Vulintus photodetector library.

// Serial communication constants. // 
#define SERIAL_BAUD_RATE  115200        // Serial baud rate.

// Pin assignments. // 
#define BEAM_IN 	A0 
#define BEAM_PWM    10

// Photobeam. // 
const uint8_t NUM_BEAMS = 3;            // Number of photobeams.
uint8_t beam_mask;                      // Photobeam status bitmask.
Vulintus_Photodetector beam(BEAM_IN);   // Photodetector class object for a single photobeam.
const float CUTOFF_FREQ = 20;           // Low-pass filter cut-off frequency, in Hz (set to 0 to disable filtering).

// Sampling timing. //
const uint16_t SAMPLE_PERIOD = 50;      // Sampling period, in milliseconds (50 ms = 20 Hz).
uint32_t next_sample;                   // Next sample time, in milliseconds.    


// INITIALIZATION ************************************************************// 
void setup() {

    // Initialize serial data transmission.
    Serial.begin(SERIAL_BAUD_RATE);        

    // Initialize the photobeams.
    beam.min_range = 100;                  // Set the minimum range to 100 ticks.
    beam.set_sensitivity(0.8);             // Set the sensitivity to 80%.
    beam.lowpass_cutoff(CUTOFF_FREQ);      // Set the low-pass cutoff frequency.
    beam.begin();                          // Initialize each photobeam.
    #ifdef BEAM_PWM
        beam.set_emitter_pin(BEAM_PWM);    // One IR emitter pin controls all photobeam emitters.
    #endif

    // Initialize the sampling timing.
    next_sample = millis();                 // Set the first sample time.
}


// MAIN LOOP *****************************************************************// 
void loop() {

    if (millis() > next_sample) {               // If it's time to sample...

        beam.read();                            // Read the photobeam status.

        Serial.print("MIN:");                   // Print a label for the Serial Plotter.
        Serial.print(beam.history[0]);          // Print the historical minimum.
        Serial.print(",");                      // Print a comma to separate values in the Serial Plotter.
        Serial.print("MAX:");                   // Print a label for the Serial Plotter.       
        Serial.print(beam.history[1]);          // Print the historical maximum.
        Serial.print(",");                      // Print a comma to separate values in the Serial Plotter.
        Serial.print("THRESHOLD:");             // Print a label for the Serial Plotter.       
        Serial.print(beam.get_thresh());        // Print the current threshold.
        Serial.print(",");                      // Print a comma to separate values in the Serial Plotter.
        Serial.print("CURRENT:");               // Print a label for the Serial Plotter.    
        Serial.print(beam.reading);             // Print the current value.
        Serial.println();                       // Print a carriage return.

        next_sample += SAMPLE_PERIOD;           // Set the next sample time.

    }

}