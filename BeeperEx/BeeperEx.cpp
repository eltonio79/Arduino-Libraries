#include "BeeperEx.h"
#include <NewTone.h>

BeeperEx::~BeeperEx()
{
}

BeeperEx::BeeperEx(byte pin, unsigned long beepFrequency, unsigned long beepLength, unsigned long beepTone, bool isEnabled) :
_pin(pin),
_beepFrequency(beepFrequency),
_beepLength(beepLength),
_beepTone(beepTone),
_isEnabled(isEnabled),
_previousBeeperMillis(0)
{
}

void BeeperEx::reInit(unsigned long beepFrequency, unsigned long beepLength, unsigned long beepTone)
{
    _beepFrequency = beepFrequency;
    _beepLength = beepLength;
    _beepTone = beepTone;
}

bool BeeperEx::isEnabled()
{
    return _isEnabled;
}

void BeeperEx::enable()
{
    _isEnabled = true;
}

void BeeperEx::disable()
{
    _isEnabled = false;
}

void BeeperEx::update()
{
    if (_isEnabled)
    {
        if (millis() - _previousBeeperMillis > _beepFrequency)
        {
            _previousBeeperMillis = millis();
            
             NewTone(_pin, _beepTone, _beepLength);
        }
    }
}
