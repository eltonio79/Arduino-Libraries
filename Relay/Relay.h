#ifndef Relay_H_
#define Relay_H_

#include "Arduino.h"

class Relay
{
private:
    byte _pin;
    bool _isOn;
    bool _isTiming;
    unsigned long _triggerStartMillis;
    unsigned long _triggerDelayMillis;
    unsigned long _minToggleMillis;

public:
    Relay(byte pin);
    ~Relay();

    bool isOn();
    void init(bool bOn, unsigned long minToggleMillis = 0); // , byte eepromAddress
    bool switchOn(unsigned long seconds);
    bool switchOff(unsigned long seconds);
    bool switchOn();
    bool switchOff();
    bool toggle();
    void update();
};

#endif // Relay_H_