#include "DoorRelay.h"
#include "PCF8574.h"

// define static class members
DoorRelay::DoorRelay(PCF8574& expander, byte pinA, byte pinB) :
RelayEx(0), // better to call it instead of implicite call..
_expander(expander),
_pinA(pinA),
_pinB(pinB),
_maximumOnTime(7000)
{
    // nie dzia³a tu _expander.write(...) - wiesza MySensors / system !
}

DoorRelay::DoorRelay(const DoorRelay& other) :
RelayEx(other),
_expander(other._expander),
_pinA(other._pinA),
_pinB(other._pinB),
_maximumOnTime(other._maximumOnTime)
{
}

DoorRelay::~DoorRelay()
{
}

DoorRelay& DoorRelay::operator=(const DoorRelay& other)
{
    if (&other == this)
        return *this;

    RelayEx::operator=(other);

    _expander = other._expander;
    _pinA = other._pinA;
    _pinB = other._pinB;
    _maximumOnTime = other._maximumOnTime;

    return *this;
}

bool DoorRelay::switchOn(unsigned long seconds)
{
    return Relay::switchOnImpl(seconds > _maximumOnTime ? _maximumOnTime : seconds);
}

bool DoorRelay::switchOff(unsigned long seconds)
{
    return Relay::switchOffImpl();
}

bool DoorRelay::switchOn()
{
    return Relay::switchOnImpl(_maximumOnTime);
}

bool DoorRelay::switchOff()
{
    return Relay::switchOffImpl();
}

void DoorRelay::On()
{
    _expander.write(_pinA, LOW);
    _expander.write(_pinB, HIGH);

    RelayEx::On();
}

void DoorRelay::Off()
{
    _expander.write(_pinA, LOW);
    _expander.write(_pinB, LOW);

    RelayEx::Off();
}