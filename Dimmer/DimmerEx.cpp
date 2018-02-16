#include "DimmerEx.h"
#include <Wire.h>
#include <Streaming.h>

#define UNUSED(argument) (void) (argument)

// Implementation of DimmerEx class

byte DimmerEx::VALUE_MIN = 0;
byte DimmerEx::VALUE_MAX = 100;
unsigned long DimmerEx::FADE_DURATION_OFF = 0;
unsigned long DimmerEx::FADE_DURATION_MIN = 20;
unsigned long DimmerEx::FADE_DURATION_MAX = 3600000; // 1 hour (for sanity checks)

DimmerEx::DimmerEx() :
    _value(DimmerEx::VALUE_MIN),
    _lastValue(DimmerEx::VALUE_MAX / 2), // last value has to be from 1 to 99 %
    _fadeFromValue(_value),
    _fadeToValue(_value),
    _fadeDuration(DimmerEx::FADE_DURATION_OFF),
    _fadeInterval(0),
    _fadeLastStepTime(0),
    _store(false),
    _sequenceNumber(0)
{
}

DimmerEx::DimmerEx(const DimmerEx& other)
{
    CopyFrom(other);
}

DimmerEx::~DimmerEx()
{
}

DimmerEx& DimmerEx::operator=(const DimmerEx& other)
{
    if (&other == this)
        return *this;

    CopyFrom(other);

    return *this;
}

void DimmerEx::CopyFrom(const DimmerEx& other)
{
    _value = other._value;
    _lastValue = other._lastValue;
    _fadeFromValue = other._fadeFromValue;
    _fadeToValue = other._fadeToValue;
    _fadeDuration = other._fadeDuration;
    _fadeInterval = other._fadeInterval;
    _fadeLastStepTime = other._fadeLastStepTime;
    _store = other._store;
    _sequenceNumber = other._sequenceNumber;
}

void DimmerEx::setValue(byte value, bool store)
{
    UNUSED(store);

    // sanity checks
    if (value > DimmerEx::VALUE_MAX)
        value = DimmerEx::VALUE_MAX;

    // light value hasn't changed
    if (_value == value)
        return;

    // last value should be always in the dimming range excluding ON and OFF states
    // it is used for switching it "back" to last known positive DIMMING state
    if (!isFading() && (_value > DimmerEx::VALUE_MIN) && (_value < DimmerEx::VALUE_MAX))
        _lastValue = _value;

    _value = value;
}

byte DimmerEx::getValue() const
{
    return _value;
}

unsigned int DimmerEx::getValueRaw() const
{
    return static_cast<unsigned int>(getValue());
}

bool DimmerEx::isSwitchedOn() const
{
    return _value > DimmerEx::VALUE_MIN;
}

void DimmerEx::switchOn(bool store)
{
    stopFade();
    setValue(DimmerEx::VALUE_MAX, store);
};

void DimmerEx::switchLast(bool store)
{
    stopFade();
    setValue(_lastValue, store);
};

void DimmerEx::switchOff(bool store)
{
    stopFade();
    setValue(DimmerEx::VALUE_MIN, store);
};

void DimmerEx::switchToggle(bool threeStateMode, bool store)
{
    if (_value == DimmerEx::VALUE_MAX || (_value > DimmerEx::VALUE_MIN && !threeStateMode))
        switchOff(store);
    else if (_value == DimmerEx::VALUE_MIN && threeStateMode)
        switchLast(store);
    else
        switchOn(store);
};

void DimmerEx::switchToggleOn(bool store)
{
    if (_value == DimmerEx::VALUE_MAX)
        switchLast(store);
    else
        switchOn(store);
}

bool DimmerEx::isFading() const
{
    return _fadeDuration > DimmerEx::FADE_DURATION_OFF;
}

void DimmerEx::startFade(byte fadeToValue, unsigned long fadeDuration, bool store)
{
    // sanity checks
    if (fadeToValue > DimmerEx::VALUE_MAX)
        fadeToValue = DimmerEx::VALUE_MAX;

    if (_value == fadeToValue) // light value hasn't changed
        return;

    if (fadeDuration > DimmerEx::FADE_DURATION_MAX)
        fadeDuration = DimmerEx::FADE_DURATION_MAX;

    if (fadeDuration <= DimmerEx::FADE_DURATION_MIN)
        return;

    stopFade();

    _fadeDuration = fadeDuration;
    _fadeFromValue = _value;
    _fadeToValue = fadeToValue;
    _store = store;

    // Figure out what the interval should be so that we're chaning the color by at least 1 each cycle
    // Minimum fade interval is FADE_DURATION_MIN
    float fadeDiff = abs(_value - _fadeToValue);
    _fadeInterval = round(static_cast<float>(fadeDuration) / fadeDiff);

    if (_fadeInterval < FADE_DURATION_MIN)
        _fadeInterval = FADE_DURATION_MIN;

    _fadeLastStepTime = millis();
};

void DimmerEx::stopFade()
{
    _fadeDuration = DimmerEx::FADE_DURATION_OFF;
}

byte DimmerEx::getFadeProgress() const
{
    // check if fading is in progress
    if (isFading())
        return map(_value, _fadeFromValue, _fadeToValue, DimmerEx::VALUE_MIN, DimmerEx::VALUE_MAX);
    else
        return DimmerEx::VALUE_MAX; // just return 100 % (fading done)
}

// function has to be called in every processor tick
// it is used to non-blocking lights up / down fade effect
void DimmerEx::update()
{
    // no fade - so here is nothing to do
    if (!isFading())
        return;

    unsigned long now = millis();
    unsigned long timeDiff = now - _fadeLastStepTime;

    // Interval hasn't passed yet
    if (timeDiff < _fadeInterval)
        return;

    // How far along have we gone since last update
    float percent = static_cast<float>(timeDiff) / static_cast<float>(_fadeDuration);

    // We've hit 100 % (percent == 1), set lights level to the final value
    if (percent >= 1)
    {
        stopFade();
        setValue(_fadeToValue, _store);
        _store = false;
        return;
    }

    // change _value to where it should be
    float fadeDiff = _fadeToValue - _value;
    int increment = round(fadeDiff * percent);

    setValue(_value + increment, false);

    // Update time and finish
    _fadeDuration -= timeDiff;
    _fadeLastStepTime = millis();

    return;
}

void DimmerEx::setSequenceNumber(byte value)
{
    _sequenceNumber = value;
};

byte DimmerEx::getSequenceNumber() const
{
    return _sequenceNumber;
};

void DimmerEx::readEEPROM(bool notify)
{
    UNUSED(notify);
}

void DimmerEx::saveEEPROM(bool notify)
{
    UNUSED(notify);
}