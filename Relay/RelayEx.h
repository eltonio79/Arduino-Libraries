#ifndef RelayEx_H_
#define RelayEx_H_

#include "Arduino.h"
#include <Relay.h>

class MyMessage;

class RelayEx : public Relay
{
private:
    // this identifier helps to hide implementation of re/storing an array of objects to/from EEPROM and manage MySensors auto acknowledgement
    byte _sequenceNumber;
    // this identifier is the start address in the EEPROM to store the data
    byte _eepromOffset;
    // this identifier is the sensors type offset in MySensors register
    byte _mySensorsOffset;
    // reference to global message to controller, used to construct messages "on the fly"
    MyMessage* _myMessageAccessor;

    void CopyFrom(const RelayEx& other);
    void sendMessage_Controller(byte type, byte command);

public:
    RelayEx();
    RelayEx(const RelayEx& other);
    virtual ~RelayEx();

    RelayEx& operator=(const RelayEx& other);

    void setSequenceNumber(byte value);
    byte getSequenceNumber() const;

    void setEEPROMOffset(byte value);
    byte getEEPROMOffset() const;

    void setMySensorsOffset(byte value);
    byte getMySensorsOffset() const;

    void readEEPROM(bool notify);
    void saveEEPROM(bool notify);

    void setMyMessageAccessor(MyMessage* myMessageAccessor);
    MyMessage* getMyMessageAccessor() const;

protected:
    virtual void On();
    virtual void Off();
};

#endif // RelayEx_H_