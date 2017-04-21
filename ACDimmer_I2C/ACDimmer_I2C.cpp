#include "ACDimmer_I2C.h"
#include <EEPROM.h>
#include <Wire.h>
//#include <core/MyMessage.h>
//#include <core/MySensorsCore.h>

// Implementation of ACDimmer_I2C class

ACDimmer_I2C::ACDimmer_I2C(byte slaveI2CAddress /* = 0 */, byte minimumValue /* = 128 */, byte maximumValue /*= 0 */) :
    _value(minimumValue),
    _lastValue(maximumValue),
    _minimumValue(minimumValue),
    _maximumValue(maximumValue),
    _slaveI2CAddress(slaveI2CAddress),
    _fadeFromValue(_value),
    _fadeToValue(0),
    _fadeInterval(0),
    _fadeDuration(0),
    _fadeLastStepTime(0),
    _sequenceNumber(0),
    _myMessageAccessor(NULL)
{
};

ACDimmer_I2C::ACDimmer_I2C(const ACDimmer_I2C& other) :
    _value(other._value),
    _lastValue(other._lastValue),
    _minimumValue(other._minimumValue),
    _maximumValue(other._maximumValue),
    _slaveI2CAddress(other._slaveI2CAddress),
    _fadeFromValue(other._fadeFromValue),
    _fadeToValue(other._fadeToValue),
    _fadeInterval(other._fadeInterval),
    _fadeDuration(other._fadeDuration),
    _fadeLastStepTime(other._fadeLastStepTime),
    _sequenceNumber(other._sequenceNumber),
    _myMessageAccessor(other._myMessageAccessor)
{
};

ACDimmer_I2C::~ACDimmer_I2C()
{
};

void ACDimmer_I2C::setValue(byte value, bool store /* = false */)
{
    // sanity check
    if (value > 100)
    {
        value = 100;
    }

    _value = value;

    // convert value to RAW value and send it to I2C slave dimmer (Attiny85)
    SendI2CCommand();

    if (store)
    {
        EEPROM.write(EEPROM_DATA_SIZE * _sequenceNumber + 0, _value);

        // last value should be greater than 0 always
        // it is used only for switching it "back" to last known dimming state
        if (_value > 0)
        {
            _lastValue = _value;
            EEPROM.write(EEPROM_DATA_SIZE * _sequenceNumber + 1, _lastValue);
        }
    }
};

byte ACDimmer_I2C::getValue() const
{
    return _value;
};

void ACDimmer_I2C::setMinimumValue(byte value)
{
    _minimumValue = value;
    EEPROM.write(EEPROM_DATA_SIZE * _sequenceNumber + 1, _minimumValue);
};

byte ACDimmer_I2C::getMinimumValue() const
{
    return _minimumValue;
};

void ACDimmer_I2C::setMaximumValue(byte value)
{
    _maximumValue = value;
    EEPROM.write(EEPROM_DATA_SIZE * _sequenceNumber + 2, _maximumValue);
};

byte ACDimmer_I2C::getMaximumValue() const
{
    return _maximumValue;
};

void ACDimmer_I2C::setSlaveI2CAddress(byte value)
{
    _slaveI2CAddress = value;
    EEPROM.write(EEPROM_DATA_SIZE * _sequenceNumber + 3, _slaveI2CAddress);
};

byte ACDimmer_I2C::getSlaveI2CAddress() const
{
    return _slaveI2CAddress;
};
/*
void ACDimmer_I2C::setMyMessageAccessor(MyMessage* myMessageAccessor)
{
    _myMessageAccessor = myMessageAccessor;
};

MyMessage* ACDimmer_I2C::getMyMessageAccessor() const
{
    return _myMessageAccessor;
};
*/
void ACDimmer_I2C::setSequenceNumber(byte value)
{
    _sequenceNumber = value;
};

byte ACDimmer_I2C::getSequenceNumber() const
{
    return _sequenceNumber;
};

void ACDimmer_I2C::readEEPROM()
{
    _value = EEPROM.read(EEPROM_DATA_SIZE * _sequenceNumber + 0);
    _lastValue = EEPROM.read(EEPROM_DATA_SIZE * _sequenceNumber + 1);
    _minimumValue = EEPROM.read(EEPROM_DATA_SIZE * _sequenceNumber + 2);
    _maximumValue = EEPROM.read(EEPROM_DATA_SIZE * _sequenceNumber + 3);
    _slaveI2CAddress = EEPROM.read(EEPROM_DATA_SIZE * _sequenceNumber + 4);

    _fadeFromValue = _value;
}

void ACDimmer_I2C::writeEEPROM()
{
    EEPROM.write(EEPROM_DATA_SIZE * _sequenceNumber + 0, _value);
    EEPROM.write(EEPROM_DATA_SIZE * _sequenceNumber + 1, _lastValue);
    EEPROM.write(EEPROM_DATA_SIZE * _sequenceNumber + 2, _minimumValue);
    EEPROM.write(EEPROM_DATA_SIZE * _sequenceNumber + 3, _maximumValue);
    EEPROM.write(EEPROM_DATA_SIZE * _sequenceNumber + 4, _slaveI2CAddress);
}

bool ACDimmer_I2C::isSwitchedOn() const
{
    return _value > 0;
}

void ACDimmer_I2C::switchOn()
{
    stopFade();
    setValue(_lastValue); // not need to store to EEPROM here

    // send actual light status to controller
    //if (_myMessageAccessor != NULL)
    //    send(_myMessageAccessor->setSensor(_sequenceNumber + 1).setType(V_STATUS).set(isSwitchedOn()));
};

void ACDimmer_I2C::switchOff()
{
    stopFade();
    setValue(0); // not need to store to EEPROM here

    // send actual light status to controller
    //if (_myMessageAccessor != NULL)
    //    send(_myMessageAccessor->setSensor(_sequenceNumber + 1).setType(V_STATUS).set(isSwitchedOn()));
};

bool ACDimmer_I2C::isFading() const
{
    return _fadeDuration > 0;
}

void ACDimmer_I2C::startFade(byte fadeToValue, unsigned long duration)
{
    // sanity check
    if (fadeToValue > 100)
    {
        fadeToValue = 100;
    }

    // light value hasn't changed
    if (_value == fadeToValue)
    {
        return;
    }

    stopFade();

    // fade time is to short
    if (duration <= MIN_FADE_INTERVAL)
    {
        setValue(fadeToValue, true);
        return;
    }

    _fadeDuration = duration;
    _fadeFromValue = _value;
    _fadeToValue = fadeToValue;

    // Figure out what the interval should be so that we're chaning the color by at least 1 each cycle
    // Minimum fade interval is MIN_FADE_INTERVAL

    float fadeDiff = abs(_value - _fadeToValue);
    _fadeInterval = round(static_cast<float>(duration) / fadeDiff);

    if (_fadeInterval < MIN_FADE_INTERVAL)
    {
        _fadeInterval = MIN_FADE_INTERVAL;
    }

    _fadeLastStepTime = millis();
};

void ACDimmer_I2C::stopFade()
{
    _fadeDuration = 0;
}

byte ACDimmer_I2C::getFadeProgress() const
{
    return map(_value, _fadeFromValue, _fadeToValue, 0, 100);
}

// function has to be called in every processor tick
// it is used to non-blocking lights up / down fade effect
void ACDimmer_I2C::update()
{
    // No fade
    if (_fadeDuration == 0)
    {
        return;
    }

    unsigned long now = millis();
    unsigned long timeDiff = now - _fadeLastStepTime;

    // Interval hasn't passed yet
    if (timeDiff < _fadeInterval)
    {
        return;
    }

    // How far along have we gone since last update
    float percent = static_cast<float>(timeDiff) / static_cast<float>(_fadeDuration);

    // We've hit 100 %, set lights level to the final value
    if (percent >= 1)
    {
        stopFade();
        setValue(_fadeToValue, true); // store fading values to EEPROM

        // send actual dimming level to controller
        //if (_myMessageAccessor != NULL)
        //    send(_myMessageAccessor->setSensor(_sequenceNumber + 1).setType(V_PERCENTAGE).set(getValue()));

        return;
    }

    // change _value to where it should be
    float fadeDiff = _fadeToValue - _value;
    int increment = round(fadeDiff * percent);

    setValue(_value + increment); // save EEPROM write cycles not writing it here, during dimming

    // Update time and finish
    _fadeDuration -= timeDiff;
    _fadeLastStepTime = millis();

    return;
}

void ACDimmer_I2C::SendI2CCommand()
{
    // calculate RAW dimming value
    byte valueRaw = 0;

    if (_value == 0)
    {
        valueRaw = 128; // turn OFF message
    }
    else if (_value == 100)
    {
        valueRaw = 0;  // turn ON message
    }
    else
    {
        valueRaw = map(_value, 1, 100, _minimumValue, _maximumValue); // DIM message (inside allowed range)
    }

    // send RAW dimming value through I2C to slave AC dimmer
    Wire.beginTransmission(_slaveI2CAddress);
    Wire.write(valueRaw);
    Wire.endTransmission();
}