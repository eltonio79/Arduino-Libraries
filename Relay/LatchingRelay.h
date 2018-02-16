#ifndef LatchingRelay_H_
#define LatchingRelay_H_

#include "Arduino.h"
#include "RelayEx.h"

class PCF8574;

class LatchingRelay : public RelayEx
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
    virtual void On(bool store);
    virtual void Off(bool store);
};

#endif // LatchingRelay_H_