#ifndef CURVE_H
#define	CURVE_H

//#define __PROG_TYPES_COMPAT__

#include <avr/pgmspace.h>

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))

 // przyklad uzycia "dimming curve ze wzoru" // https://diarmuid.ie/blog/pwm-exponential-led-fading-on-arduino-or-other-platforms/
 // The number of Steps between the output being on and off
 //const int dimIntervals = 100; // DimmerEx::VALUE_MAX - DimmerEx::VALUE_MIN
 // The R value in the graph equation
 // float R = 0.0f;
 // Calculate the R variable (only needs to be done once - at setup())
 //R = (dimIntervals * log10(2)) / (log10(255));
 //for (int interval = 0; interval <= dimIntervals; interval++)
 // {
 //    // Calculate the required PWM value for this interval step
 //    brightness = pow(2, (interval / R)) - 1;
 //    // Set the LED output to the calculated brightness
 //    analogWrite(transistorPin, brightness);
 //    delay(7);                                   // delay (n) where n is time to full brightness in milliseconds
 // }

class Curve
{
    static const uint8_t etable[] PROGMEM;
public:
    static uint8_t exponential(uint8_t);
    static uint8_t linear(uint8_t);
    static uint8_t reverse(uint8_t);
};

#endif	// CURVE_H

