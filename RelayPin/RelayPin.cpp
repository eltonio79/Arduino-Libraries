#include "PinRelay.h"

PinRelay::PinRelay(byte pin, unsigned long minToggleMillis /* = 0*/) :
RelayEx(minToggleMillis),
_pin(pin)
{
}

PinRelay::PinRelay(const PinRelay& other) :
RelayEx(other),
_pin(other._pin)
{
}

PinRelay::~PinRelay()
{
}

PinRelay& PinRelay::operator=(const PinRelay& other)
{
    if (&other == this)
        return *this;

    RelayEx::operator=(other);

    _pin = other._pin;

    return *this;
}

void PinRelay::On()
{
    digitalWrite(_pin, HIGH);

    RelayEx::On();
}

void PinRelay::Off()
{
    digitalWrite(_pin, LOW);

    RelayEx::Off();
}
