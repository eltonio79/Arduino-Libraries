#ifndef DIMMERTLC_H_
#define DIMMERTLC_H_

#include <Arduino.h>
#include <DimmerEx.h>

// Size of the data for DimmerTLC class to store in the EEPROM
#define EEPROM_DATA_SIZE 5

class MyMessage;

class DimmerTLC : public DimmerEx
{
public:
    DimmerTLC(byte pin = 0);
    DimmerTLC(const DimmerTLC& other);
    virtual ~DimmerTLC();
    DimmerTLC& operator=(const DimmerTLC& other);

    // EEPROM access methods
    void readEEPROM(bool notify);
    void saveEEPROM(bool notify);

    // Other methods
    virtual void setValue(byte value, bool store);

    void setPin(byte pin);
    byte getPin() const;

    void setMyMessageAccessor(MyMessage* myMessageAccessor);
    MyMessage* getMyMessageAccessor() const;

    void setSequenceNumber(byte value);
    byte getSequenceNumber() const;

    void setEEPROMOffset(byte value);
    byte getEEPROMOffset() const;

    void setMySensorsOffset(byte value);
    byte getMySensorsOffset() const;

    byte getValueRaw() const;

private:
    void CopyFrom(const DimmerTLC& other);

    void sendMessage_I2C(byte message);
    void sendMessage_Controller(byte type, byte command);

    // pin number on TLC device
    byte _pin;
    // this identifier helps to hide implementation of re/storing an array of objects to/from EEPROM
    byte _sequenceNumber;
    // this identifier is the start address in the EEPROM to store the data
    byte _eepromOffset;
    // this identifier is the sensors type offset in MySensors register
    byte _mySensorsOffset;
    // reference to global message to controller, used to construct messages "on the fly"
    MyMessage* _myMessageAccessor;
};

#endif // DIMMERTLC_H_