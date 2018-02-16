#ifndef DIMMERI2C_H_
#define DIMMERI2C_H_

#include <Arduino.h>
#include <DimmerEx.h>

class MyMessage;

class DimmerI2C : public DimmerEx
{
private:
    static byte EEPROM_DATA_SIZE;           // Size of the data for DimmerI2C class to store in the EEPROM
    static byte RAW_VALUE_MIN;              // minimum RAW value (normally 128)
    static byte RAW_VALUE_MAX;              // minimum RAW value (normally 0)

    static byte EEPROM_OFFSET;              // this identifier is the start address in the EEPROM to store the data
    static byte MYSENSORS_OFFSET;           // this identifier is the sensors type offset in MySensors register
    static MyMessage* MYMESSAGE_ACCESSOR;   // reference to global message to controller, used to construct messages "on the fly"

public:
    // #param slaveI2CAddress adres sciemniacza I2C (Attiny85 AC DIMMER MODULE or whatever..)
    // #param minimumValue (0 - 128); dobiera� eksperymentalnie (per �ar�wka / triak)
    // #param maximumValue (0 - 128); dobiera� eksperymentalnie (per �ar�wka / triak)
    DimmerI2C(byte slaveI2CAddress = 0, byte minimumValue = 128, byte maximumValue = 0);
    DimmerI2C(const DimmerI2C& other);
    virtual ~DimmerI2C();
    DimmerI2C& operator=(const DimmerI2C& other);

    // EEPROM access methods
    virtual void readEEPROM(bool notify);
    virtual void saveEEPROM(bool notify);

    // Member setters / getters
    virtual void setValue(byte value, bool store);

    // Calculates RAW dimming value (acceptable by hardware, derived dimmer)
    virtual unsigned int getValueRaw() const;

    void setMinimumValue(byte value, bool store);
    byte getMinimumValue() const;

    void setMaximumValue(byte value, bool store);
    byte getMaximumValue() const;

    void setSlaveI2CAddress(byte value, bool store);
    byte getSlaveI2CAddress() const;

    // Sequence number in the dimmers array (group ID)
    void setSequenceNumber(byte value);
    byte getSequenceNumber() const;

    // MySensors message interface
    static void setMyMessageAccessor(MyMessage* myMessageAccessor);
    static MyMessage* getMyMessageAccessor();

    // Dimmers offset (as a group) in the EEPROM
    static void setEEPROMOffset(byte value);
    static byte getEEPROMOffset();

    // Dimmers offset (as a group) in the MySensors GateWay
    static void setMySensorsOffset(byte value);
    static byte getMySensorsOffset();

private:
    void CopyFrom(const DimmerI2C& other);

    void sendMessage_I2C(byte message);
    void sendMessage_Controller(byte type, byte command);

    // Properties stored inside EEPROM
    byte _minimumValue;
    byte _maximumValue;
    byte _slaveI2CAddress;

    // this identifier helps to hide implementation of re/storing an array of objects to/from EEPROM
    // and comunicate with MySensors controller
    byte _sequenceNumber;
};

#endif // DIMMERI2C_H_