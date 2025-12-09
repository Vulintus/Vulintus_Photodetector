/*  
    Vulintus_Photodetector.cpp

    copyright Vulintus, Inc., 2024

    See "Vulintus_Photodetector.h" for documentation and change log.

*/


#include "./Vulintus_Photodetector.h"


// STATIC VARIABLE INITIALIZATIONS ***********************************************************************************//
uint8_t Vulintus_Photodetector::bitmask = 0;    // Initialize the photobeam status bitmask.


// CLASS FUNCTIONS ***************************************************************************************************//

// Class constructor.
Vulintus_Photodetector::Vulintus_Photodetector(uint8_t pin_detector, uint8_t beam_index, bool blocked_val)
        : _pin_detector(pin_detector), index(beam_index), polarity(blocked_val)
{
    _lowpass_filter = new Vulintus_LowPass_Filter();  // Create a new low-pass filter instance with default parameters.
}


// Class destructor.
Vulintus_Photodetector::~Vulintus_Photodetector(void)
{
    delete _lowpass_filter;                          // Delete the low-pass filter instance.
}


// Initialization.
void Vulintus_Photodetector::begin(void)
{
    pinMode(_pin_detector, INPUT);                          // ADC input.
    uint32_t clock_time = millis();                         // Grab the current milliseconds clock time.
    _minmax_timer[0] = clock_time + _BOOTUP_RESET_DELAY;    // Set the minimum reset time to 1 second to let the ADC charge up.
    _minmax_timer[1] = clock_time + _BOOTUP_RESET_DELAY;    // Set the maximum reset time to 1 second to let the ADC charge up.
}


// Check the current blocked/unblocked state.
bool Vulintus_Photodetector::read(void)
{
    bool change_flag = false;               // Check for a change in the photobeam state.
    bool cur_state = false;                 // Assume the current photobeam state is unblocked.

    reading_raw = analogRead(_pin_detector);    // Read the ADC value from the detector.
    read_time = micros();                       // Grab the microsecond clock time for the reading.

    if (_lowpass_cutoff > 0) {                                              // If the low-pass filter is enabled...                   
        float temp_float = _lowpass_filter->input(reading_raw, read_time);  // Run the reading through the filter.
        temp_float = max((float) 0.0, temp_float);                          // Ensure the filtered reading is non-negative.
        reading = (uint16_t) temp_float;                                    // Convert the filtered reading to an integer.
    }
    else {                                                                  // If the low-pass filter is disabled...
        reading = reading_raw;                                              // Use the raw ADC reading.
    }

    uint32_t clock_time = millis();         // Grab the millisecond clock time.

    // If the reading is less than or equal to the historical minimum or the 
    // minimum reset timer timed out...
    if ((reading <= history[0]) || ((reset_timeout > 0) && (clock_time > _minmax_timer[0]))) {                  
        history[0] = reading;                           // Update the historical minimum.
        _minmax_timer[0] = clock_time + reset_timeout;  // Reset the minimum reset timer.
    }

    // If the reading is greater than or equal to the historical maximum or the 
    // maximum reset timer timed out...
    if ((reading >= history[1]) || ((reset_timeout > 0) && (clock_time > _minmax_timer[1]))) {        
        history[1] = reading;                           // Update the historical maximum.
        _minmax_timer[1] = clock_time + reset_timeout;  // Reset the minimum reset timer.
    }

    uint16_t ir_range = history[1] - history[0];    // Calculate the current range.
    if (auto_thresh) {                              // If the threshold is being set dynamically...
      float temp_float = ir_range * _sensitivity;   // Calculate the threshold within the range.
      temp_float += history[0];                     // Add the minimum back to the threshold.
      _cur_thresh = (uint16_t) temp_float;          // Update the photobeam threshold.
    }

    if ((ir_range >= min_range) || (!auto_thresh)) {    // If the range exceeds the minimum.
        cur_state = (reading >= _cur_thresh);           // Check to see if the photobeam is blocked.
        if (!polarity) {                                // If the polarity is reversed...
            cur_state = !cur_state;                     // Flip the logical vlue.
        }
    }

    if (cur_state != is_blocked) {                  // If the blocked/unblocked state has changed...
        change_flag = true;                         // Set the change flag to true.
    }
    is_blocked = cur_state;                         // Update the photobeam state.

    bitmask &= ~(1 << index);                       // Clear the bit for this photobeam.
    bitmask |= (is_blocked << index);               // Set the bit for this photobeam.

    return change_flag;                             // Return the change flag.
}                         


// Set the emitter control pin.
void Vulintus_Photodetector::set_emitter_pin(uint8_t pin_emitter)
{
    _pin_emitter = pin_emitter;         // Copy the specified pin to the private variable.
    pinMode(_pin_emitter, OUTPUT);      // Set the pin mode to output.
    set_emitter_pwm(_pwm_val);          // Apply the current PWM value.
}


// Set the emitter PWM value.
void Vulintus_Photodetector::set_emitter_pwm(uint8_t pwm_val)
{
    _pwm_val = pwm_val;                                 // Copy the specified PWM value to the private variable.
    if (_pin_emitter != 255) {                          // If an emitter pin as been set...
        switch (_pwm_val) {                             // Switch between specified PWM values.
            case 255:                                   // For maximum PWM...
                digitalWrite(_pin_emitter, HIGH);       // Use digitalWrite to set the output high.
                break;
            case 0:                                     // For minimum PWM...
                digitalWrite(_pin_emitter, LOW);        // Use digitalWrite to set the output high.
                break;
            default:                                    // For all other PWM values...
                analogWrite(_pin_emitter, _pwm_val);    // Use analogWrite to these the output value.
                break;
        }
    }
    
}

// Get the current emitter PWM value.
uint8_t Vulintus_Photodetector::get_emitter_pwm(void)
{
    if (_pin_emitter != 255) {          // If an emitter pin as been set...
        return _pwm_val;                // Return the current PWM value.
    }
    else {                              // Otherwise...
        return 0;                       // Return a 0.
    }
}           


// Set the threshold, in ADC ticks.
void Vulintus_Photodetector::set_thresh(uint16_t thresh_adc)
{
    _cur_thresh = thresh_adc;           // Set the threshold to the specified value.
    auto_thresh = false;                // Disable auto-thresholding.
} 


// Get the current threshold, in ADC ticks.
uint16_t Vulintus_Photodetector::get_thresh(void)
{
    return _cur_thresh;                 // Return the current threshold.
}


// Set the auto-thresholding sensitivity.
void Vulintus_Photodetector::set_sensitivity(float thresh_fl)
{
    _sensitivity = thresh_fl;           // Set the sensitivity value.
    auto_thresh = true;                 // Enable auto-thresholding.
}


// Get the current auto-thresholding sensitivity.
float Vulintus_Photodetector::get_sensitivity(void)
{
    return _sensitivity;                // Return the current sensitivity value.
}


// Get the current low-pass filter cutoff frequency, in Hz.  
float Vulintus_Photodetector::lowpass_cutoff(void)
{
    return _lowpass_cutoff;             // Return the current low-pass filter cutoff frequency.
}


// Set the low-pass filter cutoff frequency, in Hz.
float Vulintus_Photodetector::lowpass_cutoff(float new_freq)
{
    _lowpass_cutoff = _lowpass_filter->cutoff_frequency(new_freq);  // Set the new cutoff frequency.
    return _lowpass_cutoff;                                         // Return the current low-pass filter cutoff frequency.
}  


// Reset the photobeam history.
uint8_t Vulintus_Photodetector::reset(void)
{
    history[0] = 65535;                 // Reset the historical minimum.
    history[1] = 0;                     // Reset the historical maximum.
    _cur_thresh = 0;                    // Reset the threshold.
    auto_thresh = true;                 // Enable auto-thresholding.
}             