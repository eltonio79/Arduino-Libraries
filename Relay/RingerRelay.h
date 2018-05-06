#ifndef RingerRelay_H_
#define RingerRelay_H_

#include "Arduino.h"
#include "RelayEx.h"

class PCF8574;

class RingerRelay : public RelayEx
{
    PCF8574& _expander;
    byte _pinA;
    byte _pinB;

    unsigned long _previousDingDong;
    unsigned long _intervalDingDong;

public:
    RingerRelay(PCF8574& expander, byte pinA, byte pinB);
    RingerRelay(const RingerRelay& other);
    RingerRelay& operator=(const RingerRelay& other);
    virtual ~RingerRelay();

    virtual void update();
protected:
    // relay type dependant methods
    virtual void On();
    virtual void Off();
};

#endif // RingerRelay_H_