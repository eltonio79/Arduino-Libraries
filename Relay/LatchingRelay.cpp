#include "LatchingRelay.h"
#include "PCF8574.h"

// define static class members
const byte LatchingRelay::_switchingOffsetTimeMillis = 15;

LatchingRelay::LatchingRelay(PCF8574& expander, byte pinA, byte pinB) :
Relay(pinA), // better to call it instead of implicite call..
_expander(expander),
_pinA(pinA),
_pinB(pinB)
{
    // we want to have neutral state of the pins by default (LOW, LOW)
    _expander.write(_pinA, LOW);
    _expander.write(_pinB, LOW);
}

LatchingRelay::LatchingRelay(const LatchingRelay& other) :
Relay(other),
_expander(other._expander),
_pinA(other._pinA),
_pinB(other._pinB)
{
}

LatchingRelay::~LatchingRelay()
{
}

LatchingRelay& LatchingRelay::operator=(const LatchingRelay& other)
{
    if (&other == this)
        return *this;

    Relay::operator=(other);

    _expander = other._expander;
    _pinA = other._pinA;
    _pinB = other._pinB;

    return *this;
}

void LatchingRelay::On()
{
    _expander.write(_pinA, LOW);
    _expander.write(_pinB, HIGH);
    delay(LatchingRelay::_switchingOffsetTimeMillis);
    _expander.write(_pinA, LOW);
    _expander.write(_pinB, LOW);
}

void LatchingRelay::Off()
{
    _expander.write(_pinA, HIGH);
    _expander.write(_pinB, LOW);
    delay(LatchingRelay::_switchingOffsetTimeMillis);
    _expander.write(_pinA, LOW);
    _expander.write(_pinB, LOW);
}