#include "ACDimmer_I2C.h"
#include <Wire.h>
#include <Streaming.h>
#include <..\MySensors\core\MyMessage.h>
#include <..\MySensors\core\MySensorsCore.h>

// Implementation of ACDimmer_I2C class

ACDimmer_I2C::ACDimmer_I2C(byte slaveI2CAddress /* = 0 */,
                           byte minimumValue /* = 128 */,
                           byte maximumValue /*= 0 */) :
    _value(0),
    _lastValue(50), // last value has to be from 1 to 99 %
    _minimumValue(minimumValue),
    _maximumValue(maximumValue),
    _slaveI2CAddress(slaveI2CAddress),
    _fadeFromValue(_value),
    _fadeToValue(0),
    _fadeInterval(0),
    _fadeDuration(0),
    _fadeLastStepTime(0),
    _sequenceNumber(0),
    _eepromOffset(0),
    _sensorsTypeOffset(0),
    _myMessageAccessor(NULL)
{
}

ACDimmer_I2C::ACDimmer_I2C(const ACDimmer_I2C& other)
{
    CopyFrom(other);
}

ACDimmer_I2C::~ACDimmer_I2C()
{
}

ACDimmer_I2C& ACDimmer_I2C::operator=(const ACDimmer_I2C& other)
{
    if (&other == this)
        return *this;

    CopyFrom(other);

    return *this;
}

void ACDimmer_I2C::CopyFrom(const ACDimmer_I2C& other)
{
    _value = other._value;
    _lastValue = other._lastValue;
    _minimumValue = other._minimumValue;
    _maximumValue = other._maximumValue;
    _slaveI2CAddress = other._slaveI2CAddress;
    _fadeFromValue = other._fadeFromValue;
    _fadeToValue = other._fadeToValue;
    _fadeInterval = other._fadeInterval;
    _fadeDuration = other._fadeDuration;
    _fadeLastStepTime = other._fadeLastStepTime;
    _sequenceNumber = other._sequenceNumber;
    _eepromOffset = other._eepromOffset;
    _sensorsTypeOffset = other._sensorsTypeOffset;
    _myMessageAccessor = other._myMessageAccessor;
}

void ACDimmer_I2C::setValue(byte value, bool store /* = false */)
{
    // sanity check's
    if (value > 100)
    {
        value = 100;
    }

    // light value hasn't changed
    if (_value == value)
    {
        return;
    }

    // last value should be always in the dimming range excluding ON and OFF states
    // it is used for switching it "back" to last known positive DIMMING state
    if (store && (_value > 0) && (_value < 100))
    {
        _lastValue = _value;
    }

    _value = value;

    // convert and send RAW dimming value to I2C slave dimmer (Attiny85)
    sendMessage_I2C(getValueRaw());

    if (store)
    {
        saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 0, _value);
        saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 1, _lastValue);

        //Serial << "Write sn:" << _sequenceNumber << " address: " << EEPROM_DATA_SIZE * _sequenceNumber + 0 << " _value = " << _value << endln;
        //Serial << "Write sn:" << _sequenceNumber << " address: " << EEPROM_DATA_SIZE * _sequenceNumber + 1 << " _lastValue = " << _lastValue << endln;
    }
};

byte ACDimmer_I2C::getValue() const
{
    return _value;
};

byte ACDimmer_I2C::getValueRaw() const
{
    // calculate RAW dimming value
    byte valueRaw = 0;

    if (_value == 0)
    {
        valueRaw = 128; // turn OFF
    }
    else if (_value == 100)
    {
        valueRaw = 0;  // turn ON
    }
    else
    {
        valueRaw = map(_value, 1, 100, _minimumValue, _maximumValue); // DIM message (inside allowed range)
    }

    return valueRaw;
}

void ACDimmer_I2C::setMinimumValue(byte value)
{
    _minimumValue = value;
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 2, _minimumValue);
};

byte ACDimmer_I2C::getMinimumValue() const
{
    return _minimumValue;
};

void ACDimmer_I2C::setMaximumValue(byte value)
{
    _maximumValue = value;
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 3, _maximumValue);
};

byte ACDimmer_I2C::getMaximumValue() const
{
    return _maximumValue;
};

void ACDimmer_I2C::setSlaveI2CAddress(byte value)
{
    _slaveI2CAddress = value;
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 4, _slaveI2CAddress);
};

byte ACDimmer_I2C::getSlaveI2CAddress() const
{
    return _slaveI2CAddress;
};

void ACDimmer_I2C::setMyMessageAccessor(MyMessage* myMessageAccessor)
{
    _myMessageAccessor = myMessageAccessor;
};

MyMessage* ACDimmer_I2C::getMyMessageAccessor() const
{
    return _myMessageAccessor;
};

void ACDimmer_I2C::setSequenceNumber(byte value)
{
    _sequenceNumber = value;
};

byte ACDimmer_I2C::getSequenceNumber() const
{
    return _sequenceNumber;
};

void ACDimmer_I2C::setEEPROMOffset(byte value)
{
    _eepromOffset = value;
};

byte ACDimmer_I2C::getEEPROMOffset() const
{
    return _eepromOffset;
};

void ACDimmer_I2C::setSensorsTypeOffset(byte value)
{
    _sensorsTypeOffset = value;
};

byte ACDimmer_I2C::getSensorsTypeOffset() const
{
    return _sensorsTypeOffset;
};

void ACDimmer_I2C::readEEPROM()
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

    // check how works receiving last state from the controller instead of EEPROM ( ... request(i + 1, V_STATUS); ...)

    // switch the light level to last known value
    sendMessage_I2C(getValueRaw());

    // send dimmer initial value to the controller
    sendMessage_Controller(V_PERCENTAGE, getValue());
}

void ACDimmer_I2C::writeEEPROM()
{
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 0, _value);
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 1, _lastValue);
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 2, _minimumValue);
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 3, _maximumValue);
    saveState(_eepromOffset + EEPROM_DATA_SIZE * _sequenceNumber + 4, _slaveI2CAddress);
}

bool ACDimmer_I2C::isSwitchedOn() const
{
    return _value > 0;
}

void ACDimmer_I2C::switchOn()
{
    stopFade();
    setValue(100, true);

    // send actual dimming level to controller
    sendMessage_Controller(V_PERCENTAGE, getValue());

    // send actual light status to controller
    // sendMessage_Controller(V_STATUS, isSwitchedOn());
};

void ACDimmer_I2C::switchLast()
{
    stopFade();
    setValue(_lastValue, true);

    // send actual dimming level to controller
    sendMessage_Controller(V_PERCENTAGE, getValue());

    // send actual light status to controller
    // sendMessage_Controller(V_STATUS, isSwitchedOn());
};

void ACDimmer_I2C::switchOff()
{
    stopFade();
    setValue(0, true);

    // send actual dimming level to controller
    sendMessage_Controller(V_PERCENTAGE, getValue());

    // send actual light status to controller
    // sendMessage_Controller(V_STATUS, isSwitchedOn());
};

void ACDimmer_I2C::switchToggle(bool threeStateMode /* = false */)
{
    if (_value == 100 || (_value > 0 && !threeStateMode))
        switchOff();
    else if (_value == 0 && threeStateMode)
        switchLast();
    else
        switchOn();
};

void ACDimmer_I2C::switchToggleOn()
{
    if (_value == 100)
        switchLast();
    else
        switchOn();
}

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

    // fade time is to short - e.g. assume some is turning the encoder knob ?
    if (duration <= MIN_FADE_INTERVAL)
    {
        // do nothing and just returning here will be better?

        setValue(fadeToValue, false); // hmmm mo¿e przekazywaæ to z czy ma iœæ do EEPROM z góry?
        // send actual dimming level to controller - ASSUMING THROTTLE !
        sendMessage_Controller(V_PERCENTAGE, getValue());

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
    // check if fading is in progress
    if (isFading())
        return map(_value, _fadeFromValue, _fadeToValue, 0, 100);
    else
        return 100; // just return 100 % (fading done) if it is not
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
        setValue(_fadeToValue, false); // hmmm mo¿e przekazywaæ to z czy ma iœæ do EEPROM z góry?

        // send actual dimming level to controller
        sendMessage_Controller(V_PERCENTAGE, getValue());

        return;
    }

    // change _value to where it should be
    float fadeDiff = _fadeToValue - _value;
    int increment = round(fadeDiff * percent);

    setValue(_value + increment, false); // false to save EEPROM write cycles not writing it here, during dimming

    // Update time and finish
    _fadeDuration -= timeDiff;
    _fadeLastStepTime = millis();

    return;
}

void ACDimmer_I2C::sendMessage_I2C(byte command)
{
    // send RAW dimming value through I2C to slave AC dimmer
    Wire.beginTransmission(_slaveI2CAddress);
    Wire.write(command);
    Wire.endTransmission();
}

void ACDimmer_I2C::sendMessage_Controller(byte type, byte command)
{
    // send actual light status to controller (if _myMessageAccessor was set)
    if (_myMessageAccessor != NULL)
        send(_myMessageAccessor->setSensor(_sensorsTypeOffset + _sequenceNumber + 1).setType(type).set(command));
}