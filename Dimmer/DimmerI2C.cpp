#include "DimmerI2C.h"
#include <Wire.h>
#include <Streaming.h>
#include <..\MySensors\core\MyMessage.h>
#include <..\MySensors\core\MySensorsCore.h>

// Implementation of DimmerI2C class

byte DimmerI2C::EEPROM_DATA_SIZE = 5; // number of members stored in the EEPROM
byte DimmerI2C::RAW_VALUE_MIN = 128;  // full OFF
byte DimmerI2C::RAW_VALUE_MAX = 0;    // full ON

byte DimmerI2C::EEPROM_OFFSET = 0;                    // this identifier is the start address in the EEPROM to store the data
byte DimmerI2C::MYSENSORS_OFFSET = 0;                 // this identifier is the sensors type offset in MySensors register
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

void DimmerI2C::readEEPROM(bool notify)
{
    _value = loadState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 0);
    _lastValue = loadState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 1);
    _minimumValue = loadState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 2);
    _maximumValue = loadState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 3);
    _slaveI2CAddress = loadState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 4);

    // Serial << "Read sn." << _sequenceNumber <<  " address: " << DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 0 << " _value = " << _value << endln;
    // Serial << "Read sn." << _sequenceNumber <<  " address: " << DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 1 << " _lastValue = " << _lastValue << endln;
    // Serial << "Read sn." << _sequenceNumber <<  " address: " << DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 2 << " _minimumValue = " << _minimumValue << endln;
    // Serial << "Read sn." << _sequenceNumber <<  " address: " << DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 3 << " _maximumValue = " << _maximumValue << endln;
    // Serial << "Read sn." << _sequenceNumber <<  " address: " << DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 4 << " _slaveI2CAddress = " << _slaveI2CAddress << endln;
    // Serial << endln;

    // Sanity checks (in case when EEPROM dies..)
    if (_value > DimmerEx::VALUE_MAX)
        _value = DimmerEx::VALUE_MAX;

    if (_lastValue > DimmerEx::VALUE_MAX)
        _lastValue = DimmerEx::VALUE_MAX;

    _fadeFromValue = _value;
    _fadeToValue = _value;

    // switch the light level to last known value (in the real device)
    sendMessage_I2C(getValueRaw());

    if (notify)
        sendMessage_Controller(V_PERCENTAGE, getValue()); // send dimmer initial value to the controller
}

void DimmerI2C::saveEEPROM(bool notify)
{
    saveState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 0, _value);
    saveState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 1, _lastValue);
    saveState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 2, _minimumValue);
    saveState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 3, _maximumValue);
    saveState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 4, _slaveI2CAddress);

    if (notify)
        sendMessage_Controller(V_PERCENTAGE, getValue()); // send dimmer initial value to the controller
}

void DimmerI2C::setValue(byte value, bool store)
{
    DimmerEx::setValue(value, store);

    // real, hardware change of setValue state (method can be overriden in derived classes)
    sendMessage_I2C(getValueRaw());

    // update value status inside controller
    sendMessage_Controller(V_PERCENTAGE, getValue());

    if (store)
    {
        // save states to EEPROM and send actual dimming level to the controller
        saveState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 0, _value);
        saveState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 1, _lastValue);
    }
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

void DimmerI2C::setMinimumValue(byte value, bool store)
{
    if (value > DimmerI2C::RAW_VALUE_MAX)
        value = DimmerI2C::RAW_VALUE_MAX;
    if (value < DimmerI2C::RAW_VALUE_MIN)
        value = DimmerI2C::RAW_VALUE_MIN;

    _minimumValue = value;

    // update value status inside controller
    sendMessage_Controller(V_PERCENTAGE, getValue());

    if (store)
    {
        // save states to EEPROM and send actual dimming level to the controller
        saveState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 2, _minimumValue);
    }
};

byte DimmerI2C::getMinimumValue() const
{
    return _minimumValue;
};

void DimmerI2C::setMaximumValue(byte value, bool store)
{
    if (value > DimmerI2C::RAW_VALUE_MAX)
        value = DimmerI2C::RAW_VALUE_MAX;
    if (value < DimmerI2C::RAW_VALUE_MIN)
        value = DimmerI2C::RAW_VALUE_MIN;

    _maximumValue = value;

    // update value status inside controller
    sendMessage_Controller(V_PERCENTAGE, getValue());

    if (store)
    {
        // save states to EEPROM and send actual dimming level to the controller
        saveState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 3, _maximumValue);
    }
};

byte DimmerI2C::getMaximumValue() const
{
    return _maximumValue;
};

void DimmerI2C::setSlaveI2CAddress(byte value, bool store)
{
    _slaveI2CAddress = value;

    // update value status inside controller (maby not needed here?)
    sendMessage_Controller(V_PERCENTAGE, getValue());

    if (store)
    {
        // save states to EEPROM and send actual dimming level to the controller
        saveState(DimmerI2C::EEPROM_OFFSET + DimmerI2C::EEPROM_DATA_SIZE * _sequenceNumber + 4, _slaveI2CAddress);
    }
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
        send(DimmerI2C::MYMESSAGE_ACCESSOR->setSensor(DimmerI2C::MYSENSORS_OFFSET + _sequenceNumber + 1).setType(type).set(command));
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

void DimmerI2C::setEEPROMOffset(byte value)
{
    DimmerI2C::EEPROM_OFFSET = value;
};

byte DimmerI2C::getEEPROMOffset()
{
    return DimmerI2C::EEPROM_OFFSET;
};

void DimmerI2C::setMySensorsOffset(byte value)
{
    DimmerI2C::MYSENSORS_OFFSET = value;
};

byte DimmerI2C::getMySensorsOffset()
{
    return DimmerI2C::MYSENSORS_OFFSET;
};