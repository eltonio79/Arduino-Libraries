#include "RelayEx.h"
#include <..\MySensors\core\MyMessage.h>
#include <..\MySensors\core\MySensorsCore.h>

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
}

byte RelayEx::getSequenceNumber() const
{
    return _sequenceNumber;
}

void RelayEx::sendMessage_Controller(byte type, byte command)
{
    // send status to controller (if RelayEx::MYMESSAGE_ACCESSOR was set)
    if (RelayEx::MYMESSAGE_ACCESSOR != NULL)
        send(RelayEx::MYMESSAGE_ACCESSOR->setSensor(RelayEx::MYSENSORS_OFFSET + _sequenceNumber + 1).setType(type).set(command));
}

void RelayEx::On()
{
    // send actual relay status to controller
    sendMessage_Controller(V_STATUS, _isOn);
}

void RelayEx::Off()
{
    // send actual relay status to controller
    sendMessage_Controller(V_STATUS, _isOn);
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

void RelayEx::setMySensorsOffset(byte value)
{
    RelayEx::MYSENSORS_OFFSET = value;
};

byte RelayEx::getMySensorsOffset()
{
    return RelayEx::MYSENSORS_OFFSET;
}