#ifndef ACDimmer_I2C_H_
#define ACDimmer_I2C_H_

#include <EEPROM.h>

// The minimum time (milliseconds) the program will wait between LED adjustments
// adjust this to modify performance.
#define MIN_FADE_INTERVAL 20

// Size of the data for ACDimmer_I2C class to store in the EEPROM
#define EEPROM_DATA_SIZE 5

class ACDimmer_I2C
{
public:
    // #param slaveI2CAddress adres sciemniacza I2C (Attiny85)
    // #param minimumValue (0 - 128); dobrac eksperymentalnie
    // #param maximumValue (0 - 128); dobrac eksperymentalnie
    ACDimmer_I2C(byte slaveI2CAddress = 0, byte minimumValue = 128, byte maximumValue = 0) :
        _value(minimumValue),
        _lastValue(maximumValue),
        _minimumValue(minimumValue),
        _maximumValue(maximumValue),
        _slaveI2CAddress(slaveI2CAddress),
        _fadeValue(0),
        _fadePercentDone(0),
        _fadeInterval(0),
        _fadeDuration(0),
        _fadeLastStepTime(0),
        _localSequenceNumber(0)
    {
    };

    ACDimmer_I2C(const ACDimmer_I2C& other) :
        _value(other._value),
        _lastValue(other._lastValue),
        _minimumValue(other._minimumValue),
        _maximumValue(other._maximumValue),
        _slaveI2CAddress(other._slaveI2CAddress),
        _fadeValue(other._fadeValue),
        _fadePercentDone(other._fadePercentDone),
        _fadeInterval(other._fadeInterval),
        _fadeDuration(other._fadeDuration),
        _fadeLastStepTime(other._fadeLastStepTime),
        _localSequenceNumber(other._localSequenceNumber)
    {
    };

    ~ACDimmer_I2C() {};

    byte getValue() const
    {
        return _value;
    };

    void setMinimumValue(byte value)
    {
        _minimumValue = value;
        EEPROM.write(_localSequenceNumber + 1, _minimumValue);
    };

    byte getMinimumValue() const
    {
        return _minimumValue;
    };

    void setMaximumValue(byte value)
    {
        _maximumValue = value;
        EEPROM.write(_localSequenceNumber + 2, _maximumValue);
    };

    byte getMaximumValue() const
    {
        return _maximumValue;
    };

    void setSlaveI2CAddress(byte value)
    {
        _slaveI2CAddress = value;
        EEPROM.write(_localSequenceNumber + 3, _slaveI2CAddress);
    };

    byte getSlaveI2CAddress() const
    {
        return _slaveI2CAddress;
    };

    // it HAS TO BE CALLED ONLY ONCE PER OBJECT in setup()
    void increaseSequenceNumber()
    {
        _localSequenceNumber = _globalSequenceNumber;
        _globalSequenceNumber = _globalSequenceNumber + EEPROM_DATA_SIZE;
    }

    void readEEPROM()
    {
        _value = EEPROM.read(_localSequenceNumber + 0);
        _lastValue = EEPROM.read(_localSequenceNumber + 1);
        _minimumValue = EEPROM.read(_localSequenceNumber + 2);
        _maximumValue = EEPROM.read(_localSequenceNumber + 3);
        _slaveI2CAddress = EEPROM.read(_localSequenceNumber + 4);
    }

    void writeEEPROM()
    {
        EEPROM.write(_localSequenceNumber + 0, _value);
        EEPROM.write(_localSequenceNumber + 1, _lastValue);
        EEPROM.write(_localSequenceNumber + 2, _minimumValue);
        EEPROM.write(_localSequenceNumber + 3, _maximumValue);
        EEPROM.write(_localSequenceNumber + 4, _slaveI2CAddress);
    }

    // #param value (0 - 100%)
    void setValue(byte value, bool store = false)
    {
        // sanity check
        if (value > 100)
        {
            value = 100;
        }

        _value = value;

        SendI2CCommand();

        if (store)
        {
            // last value should be greater than 0 always
            // it is used only for switching it "back" to last known dimming state 
            if (_value > 0)
            {
                _lastValue = _value;
                EEPROM.write(_localSequenceNumber + 1, _lastValue);
            }

            EEPROM.write(_localSequenceNumber + 0, _value);
        }
    };

    bool isSwitchedOn()
    {
        return _value > 0;
    }

    void switchOn()
    {
        stopFade();
        setValue(_lastValue); // not need to store to EEPROM here
    };

    void switchOff()
    {
        stopFade();
        setValue(0); // not need to store to EEPROM here
    };

    void stopFade()
    {
        _fadePercentDone = 100;
        _fadeDuration = 0;
    }

    bool isFading()
    {
        return _fadeDuration > 0;
    }

    byte get_progress()
    {
        return round(_fadePercentDone);
    }

    void setFadeTo(byte fadeValue, unsigned long duration)
    {
        // sanity check
        if (fadeValue > 100)
        {
            fadeValue = 100;
        }

        // light value hasn't changed
        if (_value == fadeValue)
        {
            return;
        }

        stopFade();
        _fadePercentDone = 0;

        // fade time is to short..
        if (duration <= MIN_FADE_INTERVAL)
        {
            setValue(fadeValue, true);
            return;
        }

        _fadeDuration = duration;
        _fadeValue = fadeValue;

        // Figure out what the interval should be so that we're chaning the color by at least 1 each cycle
        // Minimum interval is MIN_FADE_INTERVAL

        float fadeDiff = abs(_value - _fadeValue);
        _fadeInterval = round(static_cast<float>(duration) / fadeDiff);

        if (_fadeInterval < MIN_FADE_INTERVAL)
        {
            _fadeInterval = MIN_FADE_INTERVAL;
        }

        _fadeLastStepTime = millis();
    };

    // function has to be called in every processor tick
    // it is used to non-blocking lights up / down fade effect
    void update()
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
        _fadePercentDone += percent;

        // We've hit 100%, set LED to the final color
        if (percent >= 1)
        {
            stopFade();
            setValue(_fadeValue, true); // store fading values to EEPROM
            return;
        }

        // change _value to where it should be
        float fadeDiff = _fadeValue - _value;
        int increment = round(fadeDiff * percent);

        setValue(_value + increment); // save EEPROM write cycles not writing it here, during dimming

        // Update time and finish
        _fadeDuration -= timeDiff;

        _fadeLastStepTime = millis();

        return;
    }

private:
    void SendI2CCommand()
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

    // Properties stored inside EEPROM
    byte _value;
    byte _lastValue;
    byte _minimumValue;
    byte _maximumValue;
    byte _slaveI2CAddress;

    // Unblocking dimming properties
    byte _fadeValue;
    float _fadePercentDone;
    unsigned long  _fadeInterval;
    unsigned long _fadeDuration;
    unsigned long _fadeLastStepTime;

    // Other properties
    byte _localSequenceNumber;

    static byte _globalSequenceNumber;
};

byte ACDimmer_I2C::_globalSequenceNumber = 0;

#endif // ACDimmer_I2C_H_