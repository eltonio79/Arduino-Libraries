#include "DimmerTriac.h"
#include <TimerOne.h>

int DimmerTriac::_frequencyStep = 78;

DimmerTriac::~DimmerTriac()
{
}

DimmerTriac::DimmerTriac(byte pinDimming) :
_pinDimming(pinDimming),
_zeroCrossDetected(false),
_dimmingCounter(0),
_dimmingLevel(128)
{
}

void DimmerTriac::init(TimerOne& timerOne, byte interruptZeroCross, void(*zeroCrossEventHandler)(), void(*dimmingLevelEventHandler)())
{
    attachInterrupt(interruptZeroCross, zeroCrossEventHandler, RISING); // attach an interupt event function for zero cross detection

    timerOne.initialize(_frequencyStep); // initialize TimerOne library for the specified frequency step
    timerOne.attachInterrupt(dimmingLevelEventHandler, _frequencyStep); // attach an interupt event function for the triac firing moments
}

void DimmerTriac::setDimmingLevel(byte dimmingLevel)
{
    _dimmingLevel = dimmingLevel;
}

int DimmerTriac::getDimmingLevel() const
{
    return _dimmingLevel;
}

volatile void DimmerTriac::zeroCrossing()
{
    _zeroCrossDetected = true; // set the true to tell our dimming() function that a zero cross has occured
    _dimmingCounter = 0;
    digitalWrite(_pinDimming, LOW);
}

volatile void DimmerTriac::dimming()
{
    if (_zeroCrossDetected == true)
    {
        if (_dimmingCounter > _dimmingLevel)
        {
            digitalWrite(_pinDimming, HIGH);
            _dimmingCounter = 0;
            _zeroCrossDetected = false;
        }
        else
        {
            ++_dimmingCounter;
        }
    }
}