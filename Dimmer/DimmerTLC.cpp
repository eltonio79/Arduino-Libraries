#include "DimmerTLC.h"
#include "Tlc5940.h"
#include <..\MySensors\core\MyMessage.h>
#include <..\MySensors\core\MySensorsCore.h>

// Implementation of DimmerTLC class

unsigned int DimmerTLC::RAW_VALUE_MIN = 0;    // full OFF
unsigned int DimmerTLC::RAW_VALUE_MAX = 4095; // full ON

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

void DimmerTLC::setValue(byte value)
{
    // Sets value with sanity check (%0 - 100%)
    DimmerEx::setValue(value);

    // real, hardware change of setValue state (method can be overriden in derived classes)
    // zamiast funkcji map u¿yj tabeli gamma 10-bit z pliku Gamma_LED.h
    Tlc.set(_pin, getValueRaw());
    Tlc.update();

    // update value status inside controller
    sendMessage_Controller(V_PERCENTAGE, getValue());
}

unsigned int DimmerTLC::getValueRaw() const
{
    // zamiast funkcji map u¿yj tabeli gamma 10-bit z pliku Gamma_LED.h
    return map(_value, DimmerEx::VALUE_MIN, DimmerEx::VALUE_MAX, DimmerTLC::RAW_VALUE_MIN, DimmerTLC::RAW_VALUE_MAX);
}

void DimmerTLC::setPin(byte pin)
{
    _pin = pin;
}

byte DimmerTLC::getPin() const
{
    return _pin;
}

void DimmerTLC::sendMessage_Controller(byte type, byte command)
{
    // send actual light status to controller (if DimmerTLC::MYMESSAGE_ACCESSOR was set)
    if (DimmerTLC::MYMESSAGE_ACCESSOR != NULL)
        send(DimmerTLC::MYMESSAGE_ACCESSOR->setSensor(_mySensorId).setType(type).set(command));
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
