#include "Relay.h"

Relay::Relay(unsigned long minToggleMillis /*= 0 */) :
_isOn(false),
_store(false),
_isTiming(false),
_triggerStartMillis(0),
_triggerDelayMillis(0),
_minToggleMillis(minToggleMillis)
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
    _store = other._store;
    _isTiming = other._isTiming;
    _triggerStartMillis = other._triggerStartMillis;
    _triggerDelayMillis = other._triggerDelayMillis;
    _minToggleMillis = other._minToggleMillis;
}

bool Relay::isOn()
{
    return _isOn;
}

bool Relay::switchOn(unsigned long seconds, bool store)
{
    if (_isTiming || _isOn)
        return false;

    bool bSwitched = switchOn(store);

    if (bSwitched)
    {
        _triggerDelayMillis = seconds * 1000;
        _isTiming = true;
        _store = store;
    }

    return bSwitched;
}

bool Relay::switchOff(unsigned long seconds, bool store)
{
    if (_isTiming || !_isOn)
        return false;

    bool bSwitched = switchOff(store);

    if (bSwitched)
    {
        _triggerDelayMillis = seconds * 1000;
        _isTiming = true;
        _store - store;
    }

    return bSwitched;
}

bool Relay::switchOn(bool store)
{
    if (_isTiming)
        return false;

    if (_triggerStartMillis == 0 || millis() - _triggerStartMillis > _minToggleMillis)
    {
        _triggerStartMillis = millis();
        _isOn = true;

        On(store); // relay-type dependant virtual function

        return true;
    }
    else
    {
        return false;
    }
}

bool Relay::switchOff(bool store)
{
    if (_isTiming)
        return false;

    if (_triggerStartMillis == 0 || millis() - _triggerStartMillis > _minToggleMillis)
    {
        _triggerStartMillis = millis();
        _isOn = false;

        Off(store); // relay-type dependant virtual function

        return true;
    }
    else
    {
        return false;
    }
}

bool Relay::switchToggle(bool store)
{
    if (_isTiming)
        return false;

    return _isOn ? switchOff(store) : switchOn(store);
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
            switchToggle(_store);
            _store = false;
        }
    }
}