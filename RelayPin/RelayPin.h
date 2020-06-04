#ifndef PinRelay_H_
#define PinRelay_H_

#include <Arduino.h>
#include <RelayEx.h>

class PinRelay : public RelayEx
{
    byte _pin;

public:
    PinRelay(byte pin, unsigned long minToggleMillis = 0);
    PinRelay(const PinRelay& other);
    PinRelay& operator=(const PinRelay& other);
    virtual ~PinRelay();

protected:
    // relay type dependant methods
    virtual void On();
    virtual void Off();
};

#endif // PinRelay_H_
