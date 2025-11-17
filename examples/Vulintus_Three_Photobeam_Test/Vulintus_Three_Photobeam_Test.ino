/*  

    Vulintus_Three_Photobeam_Test.ino

    copyright (c) 2023, Vulintus, Inc. All rights reserved.

    Example demonstrating functions in the Vulintus_Photodetector class.

    UPDATE LOG:
      2023-11-28 - Drew Sloan - Example first created.
      2025-06-06 - Drew Sloan - Library moved from Vulintus_OmniTrak to separate
                                repository. Example renamed to 
                                "Vulintus_Three_Photobeam_Test.ino".

*/


// Included libraries. // 
#include <Vulintus_Photodetector.h>     // Vulintus photodetector library.

// Serial communication constants. // 
#define SERIAL_BAUD_RATE  115200        // Serial baud rate.

// Pin assignments. // 
#define BEAM_L 	    A2
#define BEAM_C 	    A1
#define BEAM_R 	    A0 
#define BEAM_PWM    10

// Photobeams. // 
const uint8_t NUM_BEAMS = 3;            // Number of photobeams.
Vulintus_Photodetector beam[] = {
  Vulintus_Photodetector(BEAM_L, 0),
  Vulintus_Photodetector(BEAM_C, 1),
  Vulintus_Photodetector(BEAM_R, 2),
};                                      // Array of photobeam class instances.
const float CUTOFF_FREQ = 20;           // Low-pass filter cut-off frequency, in Hz (set to 0 to disable filtering).


// INITIALIZATION ************************************************************// 
void setup() {

  // Initialize serial data transmission.
  Serial.begin(SERIAL_BAUD_RATE);        

  // Initialize the photobeams.  
  for (uint8_t i = 0; i < NUM_BEAMS; i++ ) {  // Step through the photobeams.
    beam[i].min_range = 100;                  // Set the minimum range to 100 ticks.
    beam[i].set_sensitivity(0.8);             // Set the sensitivity to 80%.
    beam[i].lowpass_cutoff(CUTOFF_FREQ);      // Set the low-pass cutoff frequency.
    beam[i].begin();                          // Initialize each photobeam.    
  }
  beam[0].set_emitter_pin(BEAM_PWM);          // One IR emitter pin controls all photobeam emitters.

  Print_Photobeam_Status();                   // Print the photobeam status to the serial line.
}


// MAIN LOOP *****************************************************************// 
void loop() {

  bool change_flag = false;                   // Check for photobeam status changes.
  for (uint8_t i = 0; i < NUM_BEAMS; i++) {   // Step through the photobeams.
    change_flag |= beam[i].read();            // Trigger a reading on each photobeam.
  }
  if (change_flag) {                          // If any photobeam value changed...
    Print_Photobeam_Status();                 // Print the photobeam status to the serial line.
  }
}


// SUBFUNCTIONS **************************************************************// 

// Print the photobeam status to the serial line.
void Print_Photobeam_Status(void)
{
  Serial.print(millis());                     // Print a millisecond timestamp.
  Serial.print(" - ");                        // Print a spacer.
  Serial.print(beam[0].bitmask);              // Print the current bitmask value (shared across beam instances).
  Serial.print(" - [ ");                      // Print a spacer.
  for (uint8_t i = 0; i < NUM_BEAMS; i++ ) {  // Step through the photobeams.
    Serial.print("(");                        // Print a parenthesis.
    if (beam[i].is_blocked) {                 // If the photobeam is blocked...
      Serial.print("x");                      // Print an "x"".
    }
    else {                                    // Otherwise...
      Serial.print("o");                      // Print an "o".
    }
    Serial.print(") ");                       // Print a parenthesis.
  }
  Serial.println(" ]");                       // Print a carriage return.
}