#ifndef DoorRelay_H_
#define DoorRelay_H_

#include <Arduino.h>
#include <RelayEx.h>

class PCF8574;

class DoorRelay : public RelayEx
{
    PCF8574& _expander;
    byte _pinA;
    byte _pinB;

    unsigned long _maximumOnTime;

public:
    DoorRelay(PCF8574& expander, byte pinA, byte pinB);
    DoorRelay(const DoorRelay& other);
    DoorRelay& operator=(const DoorRelay& other);
    virtual ~DoorRelay();

    virtual bool switchOn(unsigned long seconds);
    virtual bool switchOff(unsigned long seconds);
    virtual bool switchOn();
    virtual bool switchOff();

    void setOpenPeriod(int openPeriod);
    int getOpenPeriod() const;

protected:
    // relay type dependant methods
    virtual void On();
    virtual void Off();
};

#endif // DoorRelay_H_