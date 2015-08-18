#include "ACDimmer.h"
#include <TimerOne/TimerOne.h>

int ACDimmer::_frequencyStep = 78;

ACDimmer::~ACDimmer()
{
}

ACDimmer::ACDimmer(byte pinDimming) :
_pinDimming(pinDimming),
_zeroCrossDetected(false),
_dimmingCounter(0),
_dimmingLevel(128)
{
}

void ACDimmer::init(TimerOne& timerOne, byte interruptZeroCross, void(*zeroCrossEventHandler)(), void(*dimmingLevelEventHandler)())
{
    attachInterrupt(interruptZeroCross, zeroCrossEventHandler, RISING); // attach an interupt event function for zero cross detection

    timerOne.initialize(_frequencyStep); // initialize TimerOne library for the specified frequency step
    timerOne.attachInterrupt(dimmingLevelEventHandler, _frequencyStep); // attach an interupt event function for the triac firing moments
}

void ACDimmer::setDimmingLevel(byte dimmingLevel)
{
    _dimmingLevel = dimmingLevel;
}

volatile void ACDimmer::zeroCrossing()
{
    _zeroCrossDetected = true; // set the true to tell our dimming() function that a zero cross has occured
    _dimmingCounter = 0;
    digitalWrite(_pinDimming, LOW);
}

volatile void ACDimmer::dimming()
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