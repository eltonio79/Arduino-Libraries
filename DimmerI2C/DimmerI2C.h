#ifndef DIMMERI2C_H_
#define DIMMERI2C_H_

#include <DimmerEx.h>

class MyMessage;

class DimmerI2C : public DimmerEx
{
private:
    static byte RAW_VALUE_MIN;              // minimum RAW value (normally 128)
    static byte RAW_VALUE_MAX;              // minimum RAW value (normally 0)

    static MyMessage* MYMESSAGE_ACCESSOR;   // reference to global message to controller, used to construct messages "on the fly"

public:
    // #param slaveI2CAddress adres sciemniacza I2C (Attiny85 AC DIMMER MODULE or whatever..)
    // #param minimumValue (0 - 128); dobieraæ eksperymentalnie (per ¿arówka / triak)
    // #param maximumValue (0 - 128); dobieraæ eksperymentalnie (per ¿arówka / triak)
    DimmerI2C(byte slaveI2CAddress = 0, byte minimumValue = 128, byte maximumValue = 0);
    DimmerI2C(const DimmerI2C& other);
    virtual ~DimmerI2C();
    DimmerI2C& operator=(const DimmerI2C& other);

    // Member setters / getters
    virtual bool setValue(byte value);

    // Calculates RAW dimming value (acceptable by hardware, derived dimmer)
    virtual unsigned int getValueRaw() const;

    void setMinimumValue(byte value);
    byte getMinimumValue() const;

    void setMaximumValue(byte value);
    byte getMaximumValue() const;

    void setSlaveI2CAddress(byte value);
    byte getSlaveI2CAddress() const;

    // MySensors message interface
    static void setMyMessageAccessor(MyMessage* myMessageAccessor);
    static MyMessage* getMyMessageAccessor();

private:
    void CopyFrom(const DimmerI2C& other);

    void sendMessage_I2C(byte message);
    void sendMessage_Controller(byte type, byte command);

    byte _minimumValue;
    byte _maximumValue;
    byte _slaveI2CAddress;
};

#endif // DIMMERI2C_H_
