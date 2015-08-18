#ifndef ACDimmer_H_
#define ACDimmer_H_

#include "Arduino.h"

class TimerOne;

class ACDimmer
{
private:
    byte _pinDimming;
    volatile bool _zeroCrossDetected; // flag to tell if we have crossed zero
    volatile int _dimmingCounter;     // dimming counter
    int _dimmingLevel;                // dimming level (0-128)  0 = on, 128 = 0ff

    static int _frequencyStep;        // 1000000 us / 100 Hz / 128

public:
    ACDimmer(byte pinDimming);
    ~ACDimmer();

    void setDimmingLevel(byte dimmingLevel);                // set the dimming level
    volatile void zeroCrossing();                           // has to be called when AC Zero Cross is detected
    volatile void dimming();                                // has to be called during each frequency step

    static void init(TimerOne& timerOne,
                     byte interruptZeroCross,
                     void(*zeroCrossEventHandler)(),
                     void(*dimmingLevelEventHandler)());    // initializes external timer callback functions
};

#endif // ACDimmer_H_