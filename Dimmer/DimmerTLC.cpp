#include "DimmerTLC.h"
#include "Tlc5940.h"
#include <..\MySensors\core\MyMessage.h>
#include <..\MySensors\core\MySensorsCore.h>

// Implementation of DimmerTLC class

DimmerTLC::DimmerTLC(byte pin /*= 0*/):
    DimmerEx(),
    _pin(pin),
    _sequenceNumber(0),
    _eepromOffset(0),
    _mySensorsOffset(0),
    _myMessageAccessor(NULL)
{
}

DimmerTLC::DimmerTLC(const DimmerTLC& other):
DimmerEx(other)
{
    CopyFrom(other);
}

DimmerTLC::~DimmerTLC()
{
}

DimmerTLC& DimmerTLC::operator=(const DimmerTLC& other)
{
    if (&other == this)
        return *this;

    DimmerEx::operator=(other);

    CopyFrom(other);

    return *this;
}

void DimmerTLC::CopyFrom(const DimmerTLC& other)
{
    _pin = other._pin;
    _sequenceNumber = other._sequenceNumber;
    _eepromOffset = other._eepromOffset;
    _mySensorsOffset = other._mySensorsOffset;
    _myMessageAccessor = other._myMessageAccessor;
}

void DimmerTLC::readEEPROM(bool notify)
{
    _value = loadState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 0);
    _lastValue = loadState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 1);

    // Serial << "Read sn." << _sequenceNumber <<  " address: " << EEPROM_DATA_SIZE * _sequenceNumber + 0 << " _value = " << _value << endln;
    // Serial << "Read sn." << _sequenceNumber <<  " address: " << EEPROM_DATA_SIZE * _sequenceNumber + 1 << " _lastValue = " << _lastValue << endln;
    // Serial << endln;

    _fadeFromValue = _value;
    _fadeToValue = _value;

    // switch the light level to last known value (in the real device)
    sendMessage_I2C(getValueRaw());

    if (notify)
        sendMessage_Controller(V_PERCENTAGE, getValue()); // send dimmer initial value to the controller
}

void DimmerTLC::saveEEPROM(bool notify)
{
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 0, _value);
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 1, _lastValue);

    if (notify)
        sendMessage_Controller(V_PERCENTAGE, getValue()); // send dimmer initial value to the controller
}

void DimmerTLC::setValue(byte value, bool store)
{
    DimmerEx::setValue(value, store);

    // real, hardware change of setValue state (method can be overriden in derived classes)
    // zamiast funkcji map u¿yj tabeli gamma 10-bit z pliku Gamma_LED.h
    Tlc.set(_pin, map(value, 0, 100, 0, 4095));
    Tlc.update();

    if (store)
    {
        // save states to EEPROM and send actual dimming level to the controller
        saveEEPROM(true);
    }
}

void DimmerTLC::setPin(byte pin)
{
    _pin = pin;
}

byte DimmerTLC::getPin() const
{
    return _pin;
}

void DimmerTLC::setMyMessageAccessor(MyMessage* myMessageAccessor)
{
    _myMessageAccessor = myMessageAccessor;
};

MyMessage* DimmerTLC::getMyMessageAccessor() const
{
    return _myMessageAccessor;
};

void DimmerTLC::setSequenceNumber(byte value)
{
    _sequenceNumber = value;
};

byte DimmerTLC::getSequenceNumber() const
{
    return _sequenceNumber;
};

void DimmerTLC::setEEPROMOffset(byte value)
{
    _eepromOffset = value;
};

byte DimmerTLC::getEEPROMOffset() const
{
    return _eepromOffset;
};

void DimmerTLC::setMySensorsOffset(byte value)
{
    _mySensorsOffset = value;
};

byte DimmerTLC::getMySensorsOffset() const
{
    return _mySensorsOffset;
};

void DimmerTLC::sendMessage_Controller(byte type, byte command)
{
    // send actual light status to controller (if _myMessageAccessor was set)
    if (_myMessageAccessor != NULL)
        send(_myMessageAccessor->setSensor(_mySensorsOffset + _sequenceNumber + 1).setType(type).set(command));
}