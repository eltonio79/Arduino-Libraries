#ifndef DIMMERLED_H_
#define DIMMERLED_H_

#include <Arduino.h>
#include <DimmerEx.h>

class MyMessage;

class DimmerLED : public DimmerEx
{
private:
    static byte RAW_VALUE_MIN;              // minimum RAW value (normally 0)
    static byte RAW_VALUE_MAX;              // minimum RAW value (normally 255)

    static MyMessage* MYMESSAGE_ACCESSOR;   // reference to global message to controller, used to construct messages "on the fly"

public:
    typedef uint8_t(*curveFunction)(uint8_t);

    // #param
    // #param minimumValue (0 - 255); dobieraæ eksperymentalnie
    // #param maximumValue (0 - 255); dobieraæ eksperymentalnie
    DimmerLED(const uint8_t* pins, uint8_t pinsCount, byte minimumValue = RAW_VALUE_MIN, byte maximumValue = RAW_VALUE_MAX);
    DimmerLED(const DimmerLED& other);
    virtual ~DimmerLED();
    DimmerLED& operator=(const DimmerLED& other);

    // Member setters / getters
    virtual bool setValue(byte value);

    // Calculates RAW dimming value (acceptable by hardware, derived dimmer)
    virtual unsigned int getValueRaw() const;

    void setMinimumValue(byte value);
    byte getMinimumValue() const;

    void setMaximumValue(byte value);
    byte getMaximumValue() const;

    void setPinsActive(uint8_t pinsActive);
    uint8_t getPinsActive() const;

    void setCurve(curveFunction curve);
    curveFunction getCurve();

    // MySensors message interface
    static void setMyMessageAccessor(MyMessage* myMessageAccessor);
    static MyMessage* getMyMessageAccessor();

private:
    void CopyFrom(const DimmerLED& other);

    void sendMessage_Controller(byte type, byte command);

    byte _minimumValue;
    byte _maximumValue;

    uint8_t* _pins;
    uint8_t _pinsCount;
    uint8_t _pinsActive;

    curveFunction _curve;
};

#endif // DIMMERLED_H_