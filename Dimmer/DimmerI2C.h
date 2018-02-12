#ifndef DIMMERI2C_H_
#define DIMMERI2C_H_

#include <Arduino.h>
#include <DimmerEx.h>

// Size of the data for DimmerI2C class to store in the EEPROM
#define EEPROM_DATA_SIZE 5

class MyMessage;

class DimmerI2C : public DimmerEx
{
private:
    static byte RAW_VALUE_MIN;
    static byte RAW_VALUE_MAX;
public:
    // #param slaveI2CAddress adres sciemniacza I2C (Attiny85 AC DIMMER MODULE or whatever..)
    // #param minimumValue (0 - 128); dobieraæ eksperymentalnie (per ¿arówka / triak)
    // #param maximumValue (0 - 128); dobieraæ eksperymentalnie (per ¿arówka / triak)
    DimmerI2C(byte slaveI2CAddress = 0, byte minimumValue = 128, byte maximumValue = 0);
    DimmerI2C(const DimmerI2C& other);
    virtual ~DimmerI2C();
    DimmerI2C& operator=(const DimmerI2C& other);

    // EEPROM access methods
    void readEEPROM(bool notify);
    void saveEEPROM(bool notify);

    // Other methods
    virtual void setValue(byte value, bool store);

    void setMinimumValue(byte value, bool store);
    byte getMinimumValue() const;

    void setMaximumValue(byte value, bool store);
    byte getMaximumValue() const;

    void setSlaveI2CAddress(byte value, bool store);
    byte getSlaveI2CAddress() const;

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
    void CopyFrom(const DimmerI2C& other);

    void sendMessage_I2C(byte message);
    void sendMessage_Controller(byte type, byte command);

    // Properties stored inside EEPROM
    byte _minimumValue;
    byte _maximumValue;
    byte _slaveI2CAddress;

    // this identifier helps to hide implementation of re/storing an array of objects to/from EEPROM
    byte _sequenceNumber;
    // this identifier is the start address in the EEPROM to store the data
    byte _eepromOffset;
    // this identifier is the sensors type offset in MySensors register
    byte _mySensorsOffset;
    // reference to global message to controller, used to construct messages "on the fly"
    MyMessage* _myMessageAccessor;
};

#endif // DIMMERI2C_H_