#include "RelayEx.h"
#include <..\MySensors\core\MyMessage.h>
#include <..\MySensors\core\MySensorsCore.h>

RelayEx::RelayEx() :
_sequenceNumber(0),
_eepromOffset(100),
_mySensorsOffset(0),
_myMessageAccessor(NULL)
{
}

RelayEx::RelayEx(const RelayEx& other):
Relay()
{
    CopyFrom(other);
}

RelayEx::~RelayEx()
{
}

RelayEx& RelayEx::operator=(const RelayEx& other)
{
    if (&other == this)
        return *this;

    Relay::operator=(other);

    CopyFrom(other);

    return *this;
}

void RelayEx::CopyFrom(const RelayEx& other)
{
    _sequenceNumber = other._sequenceNumber;
    _eepromOffset = other._eepromOffset;
    _mySensorsOffset = other._mySensorsOffset;
    _myMessageAccessor = other._myMessageAccessor;
}

void RelayEx::setSequenceNumber(byte value)
{
    _sequenceNumber = value;
};

byte RelayEx::getSequenceNumber() const
{
    return _sequenceNumber;
};

void RelayEx::setEEPROMOffset(byte value)
{
    _eepromOffset = value;
};

byte RelayEx::getEEPROMOffset() const
{
    return _eepromOffset;
};

void RelayEx::setMySensorsOffset(byte value)
{
    _mySensorsOffset = value;
};

byte RelayEx::getMySensorsOffset() const
{
    return _mySensorsOffset;
};

void RelayEx::readEEPROM(bool notify)
{
    _isOn = static_cast<bool>(loadState(_eepromOffset + _sequenceNumber));

    // Serial << "Read sn." << _sequenceNumber <<  " address: " << _eepromOffset + _sequenceNumber << " _value = " << _value << endln;

    // check how works receiving last state from the controller instead of EEPROM ( ... request(i + 1, V_STATUS); ...)

    // switch the relay level to last known value
    _isOn ? switchOn() : switchOff();

    if (notify)
        sendMessage_Controller(V_STATUS, _isOn); // send actual relay status to controller
}

void RelayEx::saveEEPROM(bool notify)
{
    // save actual relay state to the EEPROM
    saveState(_eepromOffset + _sequenceNumber, static_cast<int>(_isOn));

    if (notify)
        sendMessage_Controller(V_STATUS, _isOn); // send actual relay status to controller
}

void RelayEx::setMyMessageAccessor(MyMessage* myMessageAccessor)
{
    _myMessageAccessor = myMessageAccessor;
}

MyMessage* RelayEx::getMyMessageAccessor() const
{
    return _myMessageAccessor;
}

void RelayEx::sendMessage_Controller(byte type, byte command)
{
    // send status to controller (if _myMessageAccessor was set)
    if (_myMessageAccessor != NULL)
        send(_myMessageAccessor->setSensor(_mySensorsOffset + _sequenceNumber + 1).setType(type).set(command));
}

void RelayEx::On()
{
    // send actual relay status to controller
    sendMessage_Controller(V_STATUS, _isOn);

    // save actual relay state to the EEPROM
    saveState(_eepromOffset + _sequenceNumber, static_cast<int>(_isOn));
}

void RelayEx::Off()
{
    // send actual relay status to controller
    sendMessage_Controller(V_STATUS, _isOn);

    // save actual relay state to the EEPROM
    saveState(_eepromOffset + _sequenceNumber, static_cast<int>(_isOn));
}