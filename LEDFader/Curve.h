/*
 * File:   Curve.h
 * Author: cameron
 *
 * Created on October 22, 2013, 1:07 AM
 */

#ifndef CURVE_H
#define	CURVE_H

//#define __PROG_TYPES_COMPAT__

#include <avr/pgmspace.h>

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))

class Curve
{
    static const uint8_t etable[] PROGMEM;
public:
    static uint8_t exponential(uint8_t);
    static uint8_t linear(uint8_t);
    static uint8_t reverse(uint8_t);
};

#endif	// CURVE_H

