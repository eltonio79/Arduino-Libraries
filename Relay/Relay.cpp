#include "Relay.h"

Relay::~Relay()
{
}

Relay::Relay(byte pin) :
_pin(pin),
_isOn(false),
_isTiming(false),
_triggerStartMillis(0),
_triggerDelayMillis(0),
_minToggleMillis(0)
{
}

bool Relay::isOn()
{
    return _isOn;
}

void Relay::init(bool bOn, unsigned long minToggleMillis)
{
    _minToggleMillis = minToggleMillis;

    _isOn = bOn;
    _isOn ? switchOn() : switchOff();

    _triggerStartMillis = 0;
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
        digitalWrite(_pin, LOW);

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
        digitalWrite(_pin, HIGH);

        return true;
    }
    else
    {
        return false;
    }
}

bool Relay::toggle()
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
            toggle();
        }
    }
}