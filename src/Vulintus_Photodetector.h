/*  
    Vulintus_Photodetector.h

    copyright (c) 2024, Vulintus, Inc.

    Control library for photodetectors (i.e. photobeams) with analog signals that
    can be variably thresholded to increase/decrease detection sensitivity.

    UPDATE LOG:
      2024-01-21 - Drew Sloan - Library first created.
      2025-06-06 - Drew Sloan - Library moved from Vulintus_OmniTrak to separate
                                repository and renamed to 
                                "Vulintus_Photodetector".


*/


#ifndef _VULINTUS_PHOTODETECTOR_H_
#define _VULINTUS_PHOTODETECTOR_H_

#include <Arduino.h>                    //Standard Arduino header.

// Vulintus digital filters library.
// When included in an umbrella library with submodule dependencies, load this library from the submodule.
#if __has_include(<dependencies/Vulintus_Digital_Filter/src/Vulintus_Digital_Filter.h>)
    #if defined(VULINTUS_PREPROCESSOR_MESSAGES)                     
        #pragma message("Vulintus_Photodetector.h -> using 'Vulintus_Digital_Filter' submodule.")
    #endif
    #include "../../Vulintus_Digital_Filter/src/Vulintus_Digital_Filter.h"
#else    
    #if defined(VULINTUS_PREPROCESSOR_MESSAGES)    
        #pragma message("Vulintus_Photodetector.h -> using 'Vulintus_Digital_Filter' from sketchbook.")
    #endif
    #include <Vulintus_Digital_Filter.h>
#endif


// CLASSES ***********************************************************************************************************// 
 class Vulintus_Photodetector {

    public:

        // Constructor. //        
        Vulintus_Photodetector(uint8_t pin_detector, uint8_t beam_index = 0, bool blocked_val = HIGH);

        // Destructor. //
        ~Vulintus_Photodetector(void);

        // Public Variables. //
        bool is_blocked;                            // Current blocked/unblocked status.
        static uint8_t bitmask;                     // Photobeam status bitmask (shared between all photobeam instances).
        uint8_t index = 0;                          // Photobeam index, for multi-photobeam modules.
        bool polarity = HIGH;                       // Detection polarity (default HIGH means a blocked photobeam input goes high).      

        uint16_t reading;                           // Current ADC reading.
        uint32_t read_time;			                // Microsecond timestamp of last sensor readings.    
                
        bool auto_thresh = true;                    // Auto-thresholding flag (default on).
        uint16_t min_range = 100;                   // Minimum range between the historical maximum and minimum.
        uint16_t history[2] = {0xFFFF, 0};          // Historical minimum and maximum ADC values.
        uint16_t reset_timeout = 30000;             // Time-out duration for resetting the minimum and maximum history, in milliseconds.

        // Public Functions. //
		void begin(void); 					        // Initialization.

        bool read(void);                            // Update the current blocked/unblocked state, return a change flag

        void set_emitter_pin(uint8_t pin_emitter);  // Set the emitter control pin.
        void set_emitter_pwm(uint8_t pwm_val);      // Set the emitter PWM value.
        uint8_t get_emitter_pwm(void);              // Get the current emitter PWM value.

        void set_thresh(uint16_t thresh_adc);       // Set the threshold, in ADC ticks.
        uint16_t get_thresh(void);                  // Get the current threshold, in ADC ticks.
        void set_sensitivity(float thresh_fl);      // Set the auto-thresholding sensitivity.
        float get_sensitivity(void);                // Get the current auto-thresholding sensitivity.

        float lowpass_cutoff(void);                 // Get the current low-pass filter cutoff frequency, in Hz.  
        float lowpass_cutoff(float new_freq);       // Set the low-pass filter cutoff frequency, in Hz.

        uint8_t reset(void);                        // Reset the photobeam history.

    private:

        // Private Variables. //
        uint8_t _pin_detector;                      // Detector pin.
        uint8_t _pin_emitter = 255;                 // Emitter pin (255 value indicates no microcontroller control).
        uint8_t _pwm_val = 255;                     // PWM value of the emitter.        
        
        float _sensitivity = 0.5;                   // Threshold sensitivity setting (higher is more sensitive).        
        uint16_t _cur_thresh;                       // Current threshold.        
        uint32_t _minmax_timer[2];                  // Timer for resetting the minimum and maximum history.
        const uint16_t _BOOTUP_RESET_DELAY = 1000;  // Delay before history reset after boot-up, in milliseconds.

        Vulintus_LowPass_Filter *_lowpass_filter;	// Digital low-pass filter.
        float _lowpass_cutoff = 0;                  // Low-pass filter cutoff frequency, in Hz (set to 0 to disable the filter).

 };


#endif          // #ifndef _VULINTUS_PHOTODETECTOR_H_