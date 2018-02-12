#ifndef Relay_H_
#define Relay_H_

#include "Arduino.h"

class Relay
{
protected:
    // Properties stored inside EEPROM
    bool _isOn;

private:
    // internal members
    bool _isTiming;
    unsigned long _triggerStartMillis;
    unsigned long _triggerDelayMillis;
    unsigned long _minToggleMillis;

    void CopyFrom(const Relay& other);

public:
    Relay();
    Relay(const Relay& other);
    virtual ~Relay();

    Relay& operator=(const Relay& other);

    bool isOn();
    void init(bool bOn, unsigned long minToggleMillis = 0);
    bool switchOn(unsigned long seconds);
    bool switchOff(unsigned long seconds);
    bool switchOn();
    bool switchOff();
    bool toggle();

    // call it in the main loop
    void update();

protected:
    // relay type dependant methods
    virtual void On() = 0;
    virtual void Off() = 0;
};

#endif // Relay_H_