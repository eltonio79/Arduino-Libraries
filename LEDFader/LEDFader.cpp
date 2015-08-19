/*
 * LED.cpp
 *
 *  Created on: Sep 24, 2013
 *      Author: jeremy
 */

#include "LEDFader.h"

LEDFader::LEDFader(const uint8_t* pwmPins, uint8_t pwmPinsCount) :
    pins(pwmPins),
    pinsCountTotal(pwmPinsCount),
    pinsCountActive(pwmPinsCount),
    last_step_time(0),
    interval(0),
    color(0),
    to_color(0),
    duration(0),
    percent_done(0),
    curve((curve_function)NULL)
{
}

void LEDFader::set_value(uint8_t value)
{
    if (pins == NULL || pinsCountActive == 0)
        return;

    color = (uint8_t)constrain(value, 0, 255);

    for (uint8_t i = 0; i < pinsCountActive; ++i)
    {
        if (curve)
            analogWrite(pins[i], curve(color));
        else
            analogWrite(pins[i], color);
    }
}

uint8_t LEDFader::get_value()
{
    return color;
}

void LEDFader::set_active_pins_count(uint8_t value)
{
    if (value <= pinsCountTotal)
    {
        // turn off higher leds if neccessary
        if (value < pinsCountActive)
        {
            for (int i = value; i < pinsCountTotal; ++i)
            {
                analogWrite(pins[i], 0);
            }
        }

        pinsCountActive = value;
    }
}

// Set curve to transform output
void LEDFader::set_curve(curve_function c)
{
    curve = c;
}

// Get the current curve function pointer
LEDFader::curve_function LEDFader::get_curve()
{
    return curve;
}

void LEDFader::slower(unsigned long by)
{
    float cached_percent = percent_done;
    duration += by;
    fade(to_color, duration);
    percent_done = cached_percent;
}

void LEDFader::faster(unsigned long by)
{
    float cached_percent = percent_done;

    // Ends the fade
    if (duration <= by)
    {
        stop_fade();
        set_value(to_color);
    }
    else
    {
        duration -= by;
        fade(to_color, duration);
    }
    percent_done = cached_percent;
}

void LEDFader::fade(uint8_t value, unsigned long time)
{
    stop_fade();
    percent_done = 0;

    // No pins defined
    if (pins == NULL || pinsCountActive == 0)
    {
        return;
    }

    // Color hasn't changed
    if (value == color)
    {
        return;
    }

    if (time <= MIN_INTERVAL)
    {
        set_value(value);
        return;
    }

    duration = time;
    to_color = (uint8_t)constrain(value, 0, 255);

    // Figure out what the interval should be so that we're chaning the color by at least 1 each cycle
    // (minimum interval is MIN_INTERVAL)
    float color_diff = abs(color - to_color);
    interval = round((float)duration / color_diff);
    if (interval < MIN_INTERVAL)
    {
        interval = MIN_INTERVAL;
    }

    last_step_time = millis();
}

bool LEDFader::is_fading()
{
    if (pins == NULL || pinsCountActive == 0)
        return false;

    if (duration > 0)
        return true;

    return false;
}

void LEDFader::stop_fade()
{
    percent_done = 100;
    duration = 0;
}

uint8_t LEDFader::get_progress()
{
    return round(percent_done);
}

bool LEDFader::update()
{
    // No pins defined
    if (pins == NULL || pinsCountActive == 0)
    {
        return false;
    }

    // No fade
    if (duration == 0)
    {
        return false;
    }

    unsigned long now = millis();
    unsigned long time_diff = now - last_step_time;

    // Interval hasn't passed yet
    if (time_diff < interval)
    {
        return true;
    }

    // How far along have we gone since last update
    float percent = (float)time_diff / (float)duration;
    percent_done += percent;

    // We've hit 100%, set LED to the final color
    if (percent >= 1)
    {
        stop_fade();
        set_value(to_color);
        return false;
    }

    // Move color to where it should be
    int color_diff = to_color - color;
    int increment = round(color_diff * percent);

    set_value(color + increment);

    // Update time and finish
    duration -= time_diff;
    last_step_time = millis();
    return true;
}
