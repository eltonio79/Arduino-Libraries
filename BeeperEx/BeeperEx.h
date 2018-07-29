#ifndef BeeperEx_H_
#define BeeperEx_H_

#include "Arduino.h"

class BeeperEx
{
private:
    byte _pin;
    unsigned long _previousBeeperMillis;
    unsigned long _beepFrequency;
    unsigned long _beepLength;
    unsigned long _beepTone;
    bool _isEnabled;

public:
    BeeperEx(byte pin, unsigned long beepFrequency, unsigned long beepLength, unsigned long beepTone, bool isEnabled);
    ~BeeperEx();

    void reInit(unsigned long beepFrequency, unsigned long beepLength, unsigned long beepTone);

    bool isEnabled();
    void enable();
    void disable();
    void update();
};

#endif // BeeperEx_H_