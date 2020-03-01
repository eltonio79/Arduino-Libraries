#include "DimmerI2C.h"
#include <Wire.h>
#include "..\..\MySensors\core\MyMessage.h"
#include "..\..\MySensors\core\MySensorsCore.h"

// Implementation of DimmerI2C class

byte DimmerI2C::RAW_VALUE_MIN = 128;  // full OFF
byte DimmerI2C::RAW_VALUE_MAX = 0;    // full ON

MyMessage* DimmerI2C::MYMESSAGE_ACCESSOR = nullptr;   // reference to global message to controller, used to construct messages "on the fly"

DimmerI2C::DimmerI2C(byte slaveI2CAddress /* = 0   */,
                     byte minimumValue    /* = 128 */,
                     byte maximumValue    /*= 0    */) :
    DimmerEx(),
    _minimumValue(minimumValue),
    _maximumValue(maximumValue),
    _slaveI2CAddress(slaveI2CAddress)
{
}

DimmerI2C::DimmerI2C(const DimmerI2C& other):
DimmerEx(other)
{
    CopyFrom(other);
}

DimmerI2C::~DimmerI2C()
{
}

DimmerI2C& DimmerI2C::operator=(const DimmerI2C& other)
{
    if (&other == this)
        return *this;

    DimmerEx::operator=(other);

    CopyFrom(other);

    return *this;
}

void DimmerI2C::CopyFrom(const DimmerI2C& other)
{
    _minimumValue = other._minimumValue;
    _maximumValue = other._maximumValue;
    _slaveI2CAddress = other._slaveI2CAddress;
}

bool DimmerI2C::setValue(byte value)
{
    if (DimmerEx::setValue(value))
    {
        // real, hardware change of setValue state (method can be overriden in derived classes)
        sendMessage_I2C(getValueRaw());

        // update value status inside controller
        sendMessage_Controller(V_PERCENTAGE, getValue());

        return true;
    }

    return false;
}

unsigned int DimmerI2C::getValueRaw() const
{
    // calculate RAW dimming value
    unsigned int valueRaw = DimmerI2C::RAW_VALUE_MIN;

    if (_value <= DimmerEx::VALUE_MIN)
        valueRaw = DimmerI2C::RAW_VALUE_MIN; // turn OFF
    else if (_value >= DimmerEx::VALUE_MAX)
        valueRaw = DimmerI2C::RAW_VALUE_MAX;  // turn ON
    else
        valueRaw = map(_value, 1, DimmerEx::VALUE_MAX, _minimumValue, _maximumValue); // DIM message (inside allowed range)

    return valueRaw;
}

void DimmerI2C::setMinimumValue(byte value)
{
    if (value > DimmerI2C::RAW_VALUE_MAX)
        value = DimmerI2C::RAW_VALUE_MAX;
    if (value < DimmerI2C::RAW_VALUE_MIN)
        value = DimmerI2C::RAW_VALUE_MIN;

    _minimumValue = value;
};

byte DimmerI2C::getMinimumValue() const
{
    return _minimumValue;
};

void DimmerI2C::setMaximumValue(byte value)
{
    if (value > DimmerI2C::RAW_VALUE_MAX)
        value = DimmerI2C::RAW_VALUE_MAX;
    if (value < DimmerI2C::RAW_VALUE_MIN)
        value = DimmerI2C::RAW_VALUE_MIN;

    _maximumValue = value;
};

byte DimmerI2C::getMaximumValue() const
{
    return _maximumValue;
};

void DimmerI2C::setSlaveI2CAddress(byte value)
{
    _slaveI2CAddress = value;
};

byte DimmerI2C::getSlaveI2CAddress() const
{
    return _slaveI2CAddress;
};

void DimmerI2C::sendMessage_I2C(byte command)
{
    // send RAW dimming value through I2C to slave AC dimmer
    Wire.beginTransmission(_slaveI2CAddress);
    Wire.write(command);
    Wire.endTransmission();
}

void DimmerI2C::sendMessage_Controller(byte type, byte command)
{
    // send actual light status to controller (if DimmerI2C::MYMESSAGE_ACCESSOR was set)
    if (DimmerI2C::MYMESSAGE_ACCESSOR != NULL)
        send(DimmerI2C::MYMESSAGE_ACCESSOR->setSensor(_mySensorId).setType(type).set(command));
}

// Static member functions

void DimmerI2C::setMyMessageAccessor(MyMessage* myMessageAccessor)
{
    DimmerI2C::MYMESSAGE_ACCESSOR = myMessageAccessor;
};

MyMessage* DimmerI2C::getMyMessageAccessor()
{
    return DimmerI2C::MYMESSAGE_ACCESSOR;
};
