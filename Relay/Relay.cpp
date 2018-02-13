#include "Relay.h"

Relay::Relay() :
_isOn(false),
_isTiming(false),
_triggerStartMillis(0),
_triggerDelayMillis(0),
_minToggleMillis(0)
{
}

Relay::Relay(const Relay& other)
{
    CopyFrom(other);
}

Relay::~Relay()
{
}

Relay& Relay::operator=(const Relay& other)
{
    if (&other == this)
        return *this;

    CopyFrom(other);

    return *this;
}

void Relay::CopyFrom(const Relay& other)
{
    _isOn = other._isOn;
    _isTiming = other._isTiming;
    _triggerStartMillis = other._triggerStartMillis;
    _triggerDelayMillis = other._triggerDelayMillis;
    _minToggleMillis = other._minToggleMillis;
}

bool Relay::isOn()
{
    return _isOn;
}

// wywal i zastap setMinToggleMilis? inicjalizacja jest teraz w metodzie readEEPROM()
// po co tu by³o zerowane _triggerStartMillis na koniec...
void Relay::init(bool bOn, unsigned long minToggleMillis)
{
    _minToggleMillis = minToggleMillis;

    _isOn = bOn;
    _isOn ? switchOn() : switchOff();

    _triggerStartMillis = 0; // po co ?
}

bool Relay::switchOn(unsigned long seconds)
{
    if (_isTiming || _isOn)
        return false;

    bool bSwitched = switchOn();

    if (bSwitched)
    {
        _triggerDelayMillis = seconds * 1000;
        _isTiming = true;
    }

    return bSwitched;
}

bool Relay::switchOff(unsigned long seconds)
{
    if (_isTiming || !_isOn)
        return false;

    bool bSwitched = switchOff();
    
    if (bSwitched)
    {
        _triggerDelayMillis = seconds * 1000;
        _isTiming = true;
    }

    return bSwitched;
}

bool Relay::switchOn()
{
    if (_isTiming)
        return false;

    if (_triggerStartMillis == 0 || millis() - _triggerStartMillis > _minToggleMillis)
    {
        _triggerStartMillis = millis();
        _isOn = true;

        On(); // relay-type dependant virtual function

        return true;
    }
    else
    {
        return false;
    }
}

bool Relay::switchOff()
{
    if (_isTiming)
        return false;

    if (_triggerStartMillis == 0 || millis() - _triggerStartMillis > _minToggleMillis)
    {
        _triggerStartMillis = millis();
        _isOn = false;

        Off(); // relay-type dependant virtual function

        return true;
    }
    else
    {
        return false;
    }
}

bool Relay::switchToggle()
{
    if (_isTiming)
        return false;

    return _isOn ? switchOff() : switchOn();
}

void Relay::update()
{
    if (_isTiming)
    {
        if (millis() - _triggerStartMillis > _triggerDelayMillis)
        {
            _triggerStartMillis = 0;
            _triggerDelayMillis = 0;

            _isTiming = false;
            switchToggle();
        }
    }
}