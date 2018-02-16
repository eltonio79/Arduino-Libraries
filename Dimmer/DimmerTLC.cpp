#include "DimmerTLC.h"
#include "Tlc5940.h"
#include <..\MySensors\core\MyMessage.h>
#include <..\MySensors\core\MySensorsCore.h>

// Implementation of DimmerTLC class

byte DimmerTLC::EEPROM_DATA_SIZE = 3; // number of members stored in the EEPROM
unsigned int DimmerTLC::RAW_VALUE_MIN = 0;    // full OFF
unsigned int DimmerTLC::RAW_VALUE_MAX = 4095; // full ON

byte DimmerTLC::EEPROM_OFFSET = 0;                    // this identifier is the start address in the EEPROM to store the data
byte DimmerTLC::MYSENSORS_OFFSET = 0;                 // this identifier is the sensors type offset in MySensors register
MyMessage* DimmerTLC::MYMESSAGE_ACCESSOR = nullptr;   // reference to global message to controller, used to construct messages "on the fly"

DimmerTLC::DimmerTLC(byte pin /*= 0*/):
    DimmerEx(),
    _pin(pin)
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
}

void DimmerTLC::readEEPROM(bool notify)
{
    _value = loadState(DimmerTLC::EEPROM_OFFSET + DimmerTLC::EEPROM_DATA_SIZE * _sequenceNumber + 0);
    _lastValue = loadState(DimmerTLC::EEPROM_OFFSET + DimmerTLC::EEPROM_DATA_SIZE * _sequenceNumber + 1);
    _pin = loadState(DimmerTLC::EEPROM_OFFSET + DimmerTLC::EEPROM_DATA_SIZE * _sequenceNumber + 2);

    // Serial << "Read sn." << _sequenceNumber <<  " address: " << DimmerTLC::EEPROM_DATA_SIZE * _sequenceNumber + 0 << " _value = " << _value << endln;
    // Serial << "Read sn." << _sequenceNumber <<  " address: " << DimmerTLC::EEPROM_DATA_SIZE * _sequenceNumber + 1 << " _lastValue = " << _lastValue << endln;
    // Serial << "Read sn." << _sequenceNumber <<  " address: " << DimmerTLC::EEPROM_DATA_SIZE * _sequenceNumber + 2 << " _pin = " << _pin << endln;
    // Serial << endln;

    // Sanity checks (in case when EEPROM dies..)
    if (_value > DimmerEx::VALUE_MAX)
        _value = DimmerEx::VALUE_MAX;

    if (_lastValue > DimmerEx::VALUE_MAX)
        _lastValue = DimmerEx::VALUE_MAX;

    _fadeFromValue = _value;
    _fadeToValue = _value;

    // real, hardware change of setValue state (method can be overriden in derived classes)
    Tlc.set(_pin, getValueRaw());
    Tlc.update();

    if (notify)
        sendMessage_Controller(V_PERCENTAGE, getValue()); // send dimmer initial value to the controller
}

void DimmerTLC::saveEEPROM(bool notify)
{
    saveState(DimmerTLC::EEPROM_OFFSET + DimmerTLC::EEPROM_DATA_SIZE * _sequenceNumber + 0, _value);
    saveState(DimmerTLC::EEPROM_OFFSET + DimmerTLC::EEPROM_DATA_SIZE * _sequenceNumber + 1, _lastValue);
    saveState(DimmerTLC::EEPROM_OFFSET + DimmerTLC::EEPROM_DATA_SIZE * _sequenceNumber + 2, _pin);

    if (notify)
        sendMessage_Controller(V_PERCENTAGE, getValue()); // send dimmer initial value to the controller
}

void DimmerTLC::setValue(byte value, bool store)
{
    // Sets value with sanity check (%0 - 100%)
    DimmerEx::setValue(value, store);

    // real, hardware change of setValue state (method can be overriden in derived classes)
    // zamiast funkcji map u¿yj tabeli gamma 10-bit z pliku Gamma_LED.h
    Tlc.set(_pin, getValueRaw());
    Tlc.update();

    // update value status inside controller
    sendMessage_Controller(V_PERCENTAGE, getValue());

    if (store)
    {
        // save states to EEPROM and send actual dimming level to the controller
        saveState(DimmerTLC::EEPROM_OFFSET + DimmerTLC::EEPROM_DATA_SIZE * _sequenceNumber + 0, _value);
        saveState(DimmerTLC::EEPROM_OFFSET + DimmerTLC::EEPROM_DATA_SIZE * _sequenceNumber + 1, _lastValue);
    }
}

unsigned int DimmerTLC::getValueRaw() const
{
    // zamiast funkcji map u¿yj tabeli gamma 10-bit z pliku Gamma_LED.h
    return map(_value, DimmerEx::VALUE_MIN, DimmerEx::VALUE_MAX, DimmerTLC::RAW_VALUE_MIN, DimmerTLC::RAW_VALUE_MAX);
}

void DimmerTLC::setPin(byte pin, bool store)
{
    _pin = pin;

    // update value status inside controller (maby not needed here?)
    sendMessage_Controller(V_PERCENTAGE, getValue());

    if (store)
    {
        // save states to EEPROM and send actual dimming level to the controller
        saveState(DimmerTLC::EEPROM_OFFSET + DimmerTLC::EEPROM_DATA_SIZE * _sequenceNumber + 2, _pin);
    }
}

byte DimmerTLC::getPin() const
{
    return _pin;
}

void DimmerTLC::sendMessage_Controller(byte type, byte command)
{
    // send actual light status to controller (if DimmerTLC::MYMESSAGE_ACCESSOR was set)
    if (DimmerTLC::MYMESSAGE_ACCESSOR != NULL)
        send(DimmerTLC::MYMESSAGE_ACCESSOR->setSensor(DimmerTLC::MYSENSORS_OFFSET + _sequenceNumber + 1).setType(type).set(command));
}

// Static member functions

void DimmerTLC::setMyMessageAccessor(MyMessage* myMessageAccessor)
{
    DimmerTLC::MYMESSAGE_ACCESSOR = myMessageAccessor;
};

MyMessage* DimmerTLC::getMyMessageAccessor()
{
    return DimmerTLC::MYMESSAGE_ACCESSOR;
};

void DimmerTLC::setEEPROMOffset(byte value)
{
    DimmerTLC::EEPROM_OFFSET = value;
};

byte DimmerTLC::getEEPROMOffset()
{
    return DimmerTLC::EEPROM_OFFSET;
};

void DimmerTLC::setMySensorsOffset(byte value)
{
    DimmerTLC::MYSENSORS_OFFSET = value;
};

byte DimmerTLC::getMySensorsOffset()
{
    return DimmerTLC::MYSENSORS_OFFSET;
};