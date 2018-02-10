#ifndef LatchingRelay_H_
#define LatchingRelay_H_

#include "Arduino.h"
#include "Relay.h"

class PCF8574;

class LatchingRelay : public Relay
{
    static const byte _switchingOffsetTimeMillis;

    PCF8574& _expander;
    byte _pinA;
    byte _pinB;

public:
    LatchingRelay(PCF8574& expander, byte pinA, byte pinB);
    LatchingRelay(const LatchingRelay& other);
    LatchingRelay& operator=(const LatchingRelay& other);
    virtual ~LatchingRelay();

protected:
    // relay type dependant methods
    virtual void On();
    virtual void Off();
};

#endif // LatchingRelay_H_