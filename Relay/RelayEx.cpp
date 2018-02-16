#include "RelayEx.h"
#include <..\MySensors\core\MyMessage.h>
#include <..\MySensors\core\MySensorsCore.h>

byte RelayEx::EEPROM_OFFSET = 0;                    // this identifier is the start address in the EEPROM to store the data
byte RelayEx::MYSENSORS_OFFSET = 0;                 // this identifier is the sensors type offset in MySensors register
MyMessage* RelayEx::MYMESSAGE_ACCESSOR = nullptr;   // reference to global message to controller, used to construct messages "on the fly"

RelayEx::RelayEx(unsigned long minToggleMillis /* = 0*/) :
Relay(minToggleMillis),
_sequenceNumber(0)
{
}

RelayEx::RelayEx(const RelayEx& other):
Relay(other)
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
}

void RelayEx::setSequenceNumber(byte value)
{
    _sequenceNumber = value;
};

byte RelayEx::getSequenceNumber() const
{
    return _sequenceNumber;
};

void RelayEx::readEEPROM(bool notify)
{
    _isOn = static_cast<bool>(loadState(RelayEx::EEPROM_OFFSET + _sequenceNumber));

    // Serial << "Read sn." << _sequenceNumber <<  " address: " << RelayEx::EEPROM_OFFSET + _sequenceNumber << " _value = " << _value << endln;

    // check how works receiving last state from the controller instead of EEPROM ( ... request(i + 1, V_STATUS); ...)

    // switch the relay level to last known value
    _isOn ? switchOn(false) : switchOff(false);

    if (notify)
        sendMessage_Controller(V_STATUS, _isOn); // send actual relay status to controller
}

void RelayEx::saveEEPROM(bool notify)
{
    // save actual relay state to the EEPROM
    saveState(RelayEx::EEPROM_OFFSET + _sequenceNumber, static_cast<int>(_isOn));

    if (notify)
        sendMessage_Controller(V_STATUS, _isOn); // send actual relay status to controller
}

void RelayEx::sendMessage_Controller(byte type, byte command)
{
    // send status to controller (if RelayEx::MYMESSAGE_ACCESSOR was set)
    if (RelayEx::MYMESSAGE_ACCESSOR != NULL)
        send(RelayEx::MYMESSAGE_ACCESSOR->setSensor(RelayEx::MYSENSORS_OFFSET + _sequenceNumber + 1).setType(type).set(command));
}

void RelayEx::On(bool store)
{
    // send actual relay status to controller
    sendMessage_Controller(V_STATUS, _isOn);

    if (store)
    {
        // save actual relay state to the EEPROM
        saveState(RelayEx::EEPROM_OFFSET + _sequenceNumber, static_cast<int>(_isOn));
    }
}

void RelayEx::Off(bool store)
{
    // send actual relay status to controller
    sendMessage_Controller(V_STATUS, _isOn);

    if (store)
    {
        // save actual relay state to the EEPROM
        saveState(RelayEx::EEPROM_OFFSET + _sequenceNumber, static_cast<int>(_isOn));
    }
}

// Static member functions

void RelayEx::setMyMessageAccessor(MyMessage* myMessageAccessor)
{
    RelayEx::MYMESSAGE_ACCESSOR = myMessageAccessor;
}

MyMessage* RelayEx::getMyMessageAccessor()
{
    return RelayEx::MYMESSAGE_ACCESSOR;
}

void RelayEx::setEEPROMOffset(byte value)
{
    RelayEx::EEPROM_OFFSET = value;
};

byte RelayEx::getEEPROMOffset()
{
    return RelayEx::EEPROM_OFFSET;
};

void RelayEx::setMySensorsOffset(byte value)
{
    RelayEx::MYSENSORS_OFFSET = value;
};

byte RelayEx::getMySensorsOffset()
{
    return RelayEx::MYSENSORS_OFFSET;
};