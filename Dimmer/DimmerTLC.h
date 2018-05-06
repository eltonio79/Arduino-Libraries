#ifndef DIMMERTLC_H_
#define DIMMERTLC_H_

#include <Arduino.h>
#include <DimmerEx.h>

class MyMessage;

class DimmerTLC : public DimmerEx
{
private:
    static unsigned int RAW_VALUE_MIN;      // minimum RAW value (normally 0)
    static unsigned int RAW_VALUE_MAX;      // minimum RAW value (normally 4095)

    static byte MYSENSORS_OFFSET;           // this identifier is the sensors type offset in MySensors register
    static MyMessage* MYMESSAGE_ACCESSOR;   // reference to global message to controller, used to construct messages "on the fly"

public:
    DimmerTLC(byte pin = 0);
    DimmerTLC(const DimmerTLC& other);
    virtual ~DimmerTLC();
    DimmerTLC& operator=(const DimmerTLC& other);

    // Member setters / getters
    virtual void setValue(byte value);

    // Calculates RAW dimming value (acceptable by hardware, derived dimmer)
    virtual unsigned int getValueRaw() const;

    void setPin(byte pin);
    byte getPin() const;

    // MySensors message interface
    static void setMyMessageAccessor(MyMessage* myMessageAccessor);
    static MyMessage* getMyMessageAccessor();

    // Dimmers offset (as a group) in the MySensors GateWay
    static void setMySensorsOffset(byte value);
    static byte getMySensorsOffset();

private:
    void CopyFrom(const DimmerTLC& other);
    void sendMessage_Controller(byte type, byte command);

    // pin number on TLC device
    byte _pin;
};

#endif // DIMMERTLC_H_