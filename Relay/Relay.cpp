#include "Relay.h"
#include <..\MySensors\core\MyMessage.h>
#include <..\MySensors\core\MySensorsCore.h>

Relay::Relay(byte pin) :
_isOn(false),
_pin(pin),
_isTiming(false),
_triggerStartMillis(0),
_triggerDelayMillis(0),
_minToggleMillis(0),
_sequenceNumber(0),
_eepromOffset(100),
_sensorsTypeOffset(0),
_myMessageAccessor(NULL)
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
    _pin = other._pin;
    _isTiming = other._isTiming;
    _triggerStartMillis = other._triggerStartMillis;
    _triggerDelayMillis = other._triggerDelayMillis;
    _minToggleMillis = other._minToggleMillis;
    _sequenceNumber = other._sequenceNumber;
    _eepromOffset = other._eepromOffset;
    _sensorsTypeOffset = other._sensorsTypeOffset;
    _myMessageAccessor = other._myMessageAccessor;
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

        // send actual relay status to controller
        sendMessage_Controller(V_STATUS, _isOn);

        // save actual relay state to the EEPROM
        saveState(_eepromOffset + _sequenceNumber, static_cast<int>(_isOn));

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

        // send actual relay status to controller
        sendMessage_Controller(V_STATUS, _isOn);

        // save actual relay state to the EEPROM
        saveState(_eepromOffset + _sequenceNumber, static_cast<int>(_isOn));

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

void Relay::On()
{
    digitalWrite(_pin, LOW);
}

void Relay::Off()
{
    digitalWrite(_pin, HIGH);
}

void Relay::setSequenceNumber(byte value)
{
    _sequenceNumber = value;
};

byte Relay::getSequenceNumber() const
{
    return _sequenceNumber;
};

void Relay::setEEPROMOffset(byte value)
{
    _eepromOffset = value;
};

byte Relay::getEEPROMOffset() const
{
    return _eepromOffset;
};

void Relay::setSensorsTypeOffset(byte value)
{
    _sensorsTypeOffset = value;
};

byte Relay::getSensorsTypeOffset() const
{
    return _sensorsTypeOffset;
};

void Relay::readEEPROM()
{
    _isOn = static_cast<bool>(loadState(_eepromOffset + _sequenceNumber));

    // Serial << "Read sn." << _sequenceNumber <<  " address: " << _eepromOffset + _sequenceNumber << " _value = " << _value << endln;

    // check how works receiving last state from the controller instead of EEPROM ( ... request(i + 1, V_STATUS); ...)

    // switch the relay level to last known value
    _isOn ? switchOn() : switchOff();

    // send relay initial value to the controller
    sendMessage_Controller(V_STATUS, _isOn);
}

void Relay::writeEEPROM()
{
    // save actual relay state to the EEPROM
    saveState(_eepromOffset + _sequenceNumber, static_cast<int>(_isOn));
}

void Relay::setMyMessageAccessor(MyMessage* myMessageAccessor)
{
    _myMessageAccessor = myMessageAccessor;
}

MyMessage* Relay::getMyMessageAccessor() const
{
    return _myMessageAccessor;
}

void Relay::sendMessage_Controller(byte type, byte command)
{
    // send status to controller (if _myMessageAccessor was set)
    if (_myMessageAccessor != NULL)
        send(_myMessageAccessor->setSensor(_sensorsTypeOffset + _sequenceNumber + 1).setType(type).set(command));
}