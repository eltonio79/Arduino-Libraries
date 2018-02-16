#ifndef Relay_H_
#define Relay_H_

#include "Arduino.h"

class Relay
{
protected:
    // Properties stored inside EEPROM
    bool _isOn;

private:
    void CopyFrom(const Relay& other);

    // internal members
    bool _store;
    bool _isTiming;
    unsigned long _triggerStartMillis;
    unsigned long _triggerDelayMillis;
    unsigned long _minToggleMillis;

public:
    Relay(unsigned long minToggleMillis = 0);
    Relay(const Relay& other);
    virtual ~Relay();
    Relay& operator=(const Relay& other);

    // Member setters / getters
    bool isOn();
    bool switchOn(unsigned long seconds, bool store);
    bool switchOff(unsigned long seconds, bool store);
    bool switchOn(bool store);
    bool switchOff(bool store);
    bool switchToggle(bool store);

    // call it in the main loop
    void update();

protected:
    // relay type dependant methods
    virtual void On(bool store) = 0;
    virtual void Off(bool store) = 0;
};

#endif // Relay_H_