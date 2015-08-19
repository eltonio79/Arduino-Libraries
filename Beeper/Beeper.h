#ifndef Beeper_H_
#define Beeper_H_

#include "Arduino.h"

namespace
{
    void Beep(byte pin, unsigned int beepDelay)
    {
        // the only "delay()" call in this code (use rarely..)
        analogWrite(pin, 5); // 255 = very loud
        delay(beepDelay);
        analogWrite(pin, 0);
        delay(beepDelay);
    }
}

class Beeper
{
private:
    byte _pin;
    bool _isOn;
    bool _bTone;
    unsigned long _previousBeeperMillis;
    unsigned long _beepDelayOn;
    unsigned long _beepDelayOff;

public:
    Beeper(byte pin);
    ~Beeper();

    bool isOn();
    void init(bool bOn, unsigned long beepDelayOn, unsigned long beepDelayOff); // , byte eepromAddress
    void switchOn();
    void switchOff();
    void update();
};

#endif // Beeper_H_