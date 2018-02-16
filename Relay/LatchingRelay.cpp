#include "LatchingRelay.h"
#include "PCF8574.h"

// define static class members
const byte LatchingRelay::_switchingOffsetTimeMillis = 15;

LatchingRelay::LatchingRelay(PCF8574& expander, byte pinA, byte pinB) :
RelayEx(0), // better to call it instead of implicite call..
_expander(expander),
_pinA(pinA),
_pinB(pinB)
{
    // we want to have neutral state of the pins by default (LOW, LOW)
    _expander.write(_pinA, LOW);
    _expander.write(_pinB, LOW);
}

LatchingRelay::LatchingRelay(const LatchingRelay& other) :
RelayEx(other),
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

    RelayEx::operator=(other);

    _expander = other._expander;
    _pinA = other._pinA;
    _pinB = other._pinB;

    return *this;
}

void LatchingRelay::On(bool store)
{
    _expander.write(_pinA, LOW);
    _expander.write(_pinB, HIGH);
    delay(LatchingRelay::_switchingOffsetTimeMillis);
    _expander.write(_pinA, LOW);
    _expander.write(_pinB, LOW);

    RelayEx::On(store);
}

void LatchingRelay::Off(bool store)
{
    _expander.write(_pinA, HIGH);
    _expander.write(_pinB, LOW);
    delay(LatchingRelay::_switchingOffsetTimeMillis);
    _expander.write(_pinA, LOW);
    _expander.write(_pinB, LOW);

    RelayEx::Off(store);
}