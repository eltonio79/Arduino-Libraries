#include "DimmerI2C.h"
#include <Wire.h>
#include <Streaming.h>
#include <..\MySensors\core\MyMessage.h>
#include <..\MySensors\core\MySensorsCore.h>

// Implementation of DimmerI2C class

// single I2C AC DIMMER module range
byte DimmerI2C::RAW_VALUE_MIN = 128; // full OFF
byte DimmerI2C::RAW_VALUE_MAX = 0;   // full ON

DimmerI2C::DimmerI2C(byte slaveI2CAddress /* = 0   */,
                     byte minimumValue    /* = 128 */,
                     byte maximumValue    /*= 0    */) :
    DimmerEx(),
    _minimumValue(minimumValue),
    _maximumValue(maximumValue),
    _slaveI2CAddress(slaveI2CAddress),
    _sequenceNumber(0),
    _eepromOffset(0),
    _mySensorsOffset(0),
    _myMessageAccessor(NULL)
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
    _sequenceNumber = other._sequenceNumber;
    _eepromOffset = other._eepromOffset;
    _mySensorsOffset = other._mySensorsOffset;
    _myMessageAccessor = other._myMessageAccessor;
}

void DimmerI2C::readEEPROM(bool notify)
{
    _value = loadState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 0);
    _lastValue = loadState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 1);
    _minimumValue = loadState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 2);
    _maximumValue = loadState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 3);
    _slaveI2CAddress = loadState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 4);

    // Serial << "Read sn." << _sequenceNumber <<  " address: " << EEPROM_DATA_SIZE * _sequenceNumber + 0 << " _value = " << _value << endln;
    // Serial << "Read sn." << _sequenceNumber <<  " address: " << EEPROM_DATA_SIZE * _sequenceNumber + 1 << " _lastValue = " << _lastValue << endln;
    // Serial << "Read sn." << _sequenceNumber <<  " address: " << EEPROM_DATA_SIZE * _sequenceNumber + 2 << " _minimumValue = " << _minimumValue << endln;
    // Serial << "Read sn." << _sequenceNumber <<  " address: " << EEPROM_DATA_SIZE * _sequenceNumber + 3 << " _maximumValue = " << _maximumValue << endln;
    // Serial << "Read sn." << _sequenceNumber <<  " address: " << EEPROM_DATA_SIZE * _sequenceNumber + 4 << " _slaveI2CAddress = " << _slaveI2CAddress << endln;
    // Serial << endln;

    _fadeFromValue = _value;
    _fadeToValue = _value;

    // switch the light level to last known value (in the real device)
    sendMessage_I2C(getValueRaw());

    if (notify)
        sendMessage_Controller(V_PERCENTAGE, getValue()); // send dimmer initial value to the controller
}

void DimmerI2C::saveEEPROM(bool notify)
{
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 0, _value);
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 1, _lastValue);
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 2, _minimumValue);
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 3, _maximumValue);
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 4, _slaveI2CAddress);

    if (notify)
        sendMessage_Controller(V_PERCENTAGE, getValue()); // send dimmer initial value to the controller
}

void DimmerI2C::setValue(byte value, bool store)
{
    DimmerEx::setValue(value, store);

    // real, hardware change of setValue state (method can be overriden in derived classes)
    sendMessage_I2C(getValueRaw());

    if (store)
    {
        // save states to EEPROM and send actual dimming level to the controller
        saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 0, _value);
        saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 1, _lastValue);
        sendMessage_Controller(V_PERCENTAGE, getValue());
    }
}

byte DimmerI2C::getValueRaw() const
{
    // calculate RAW dimming value
    byte valueRaw = DimmerI2C::RAW_VALUE_MIN;

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

    if (store)
    {
        // save states to EEPROM and send actual dimming level to the controller
        saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 2, _minimumValue);
        sendMessage_Controller(V_PERCENTAGE, getValue());
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

    if (store)
    {
        // save states to EEPROM and send actual dimming level to the controller
        saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 3, _maximumValue);
        sendMessage_Controller(V_PERCENTAGE, getValue());
    }
};

byte DimmerI2C::getMaximumValue() const
{
    return _maximumValue;
};

void DimmerI2C::setSlaveI2CAddress(byte value, bool store)
{
    _slaveI2CAddress = value;

    if (store)
    {
        // save states to EEPROM and send actual dimming level to the controller
        saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 4, _slaveI2CAddress);
        sendMessage_Controller(V_PERCENTAGE, getValue());
    }
};

byte DimmerI2C::getSlaveI2CAddress() const
{
    return _slaveI2CAddress;
};

void DimmerI2C::setMyMessageAccessor(MyMessage* myMessageAccessor)
{
    _myMessageAccessor = myMessageAccessor;
};

MyMessage* DimmerI2C::getMyMessageAccessor() const
{
    return _myMessageAccessor;
};

void DimmerI2C::setSequenceNumber(byte value)
{
    _sequenceNumber = value;
};

byte DimmerI2C::getSequenceNumber() const
{
    return _sequenceNumber;
};

void DimmerI2C::setEEPROMOffset(byte value)
{
    _eepromOffset = value;
};

byte DimmerI2C::getEEPROMOffset() const
{
    return _eepromOffset;
};

void DimmerI2C::setMySensorsOffset(byte value)
{
    _mySensorsOffset = value;
};

byte DimmerI2C::getMySensorsOffset() const
{
    return _mySensorsOffset;
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
    // send actual light status to controller (if _myMessageAccessor was set)
    if (_myMessageAccessor != NULL)
        send(_myMessageAccessor->setSensor(_mySensorsOffset + _sequenceNumber + 1).setType(type).set(command));
}