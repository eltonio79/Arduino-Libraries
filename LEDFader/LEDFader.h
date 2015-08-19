/*
 * LED.h
 *
 *  Created on: Sep 24, 2013
 *      Author: jeremy
 *
 *  Modified to be able to fade through a big periods of time
 *          on Oct 23, 2014
 *          by: eltonio
 */

#ifndef LEDFader_H_
#define LEDFader_H_

#include "Arduino.h"

// The minimum time (milliseconds) the program will wait between LED adjustments
// adjust this to modify performance.
#define MIN_INTERVAL 20

class LEDFader
{
public:
    // Who likes dealing with function pointers? (Ok, I do, but no one else does)
    typedef uint8_t(*curve_function)(uint8_t);

private:
    const uint8_t* pins;
    const uint8_t pinsCountTotal;
    uint8_t pinsCountActive;
    unsigned long last_step_time;
    unsigned long interval;
    uint8_t color;
    uint8_t to_color;
    unsigned long duration;
    float percent_done;
    curve_function curve;

public:
    // Create a new LED Fader for a pins collection
    LEDFader(const uint8_t* pwmPins, uint8_t pwmPinsCountTotal);

    // Set an LEDs to an absolute PWM value
    void set_value(uint8_t pwm);

    // Get the current LEDs PWM value
    uint8_t get_value();

    // Set active leds count
    void set_active_pins_count(uint8_t value);

    // Set curve to transform output
    void set_curve(curve_function);

    // Get the current curve function pointer
    curve_function get_curve();

    // Fade an LEDs to a PWM value over a duration of time (milliseconds)
    void fade(uint8_t pwm, unsigned long time);

    // Returns TRUE if there is an active fade process
    bool is_fading();

    // Stop the current fade where it's at
    void stop_fade();

    // Update the LEDs along the fade
    // Returns TRUE if a fade is still in process
    bool update();

    // Decrease the current fading speed by a number of milliseconds
    void slower(unsigned long by);

    // Increase the current fading speed by a number of milliseconds
    void faster(unsigned long by);

    // Returns how much of the fade is complete in a percentage between 0 - 100
    uint8_t get_progress();
};

#endif // LEDFader_H_