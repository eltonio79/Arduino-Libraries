#ifndef RelayEx_H_
#define RelayEx_H_

#include "Arduino.h"
#include <Relay.h>

class MyMessage;

class RelayEx : public Relay
{
private:
    static byte EEPROM_OFFSET;              // this identifier is the start address in the EEPROM to store the data
    static byte MYSENSORS_OFFSET;           // this identifier is the sensors type offset in MySensors register
    static MyMessage* MYMESSAGE_ACCESSOR;   // reference to global message to controller, used to construct messages "on the fly"

public:
    RelayEx(unsigned long minToggleMillis = 0);
    RelayEx(const RelayEx& other);
    virtual ~RelayEx();
    RelayEx& operator=(const RelayEx& other);

    // EEPROM access methods
    virtual void readEEPROM(bool notify);
    virtual void saveEEPROM(bool notify);

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

protected:
    virtual void On(bool store);
    virtual void Off(bool store);

private:
    void CopyFrom(const RelayEx& other);
    void sendMessage_Controller(byte type, byte command);

    // this identifier helps to hide implementation of re/storing an array of objects to/from EEPROM and manage MySensors auto acknowledgement
    byte _sequenceNumber;
};

#endif // RelayEx_H_