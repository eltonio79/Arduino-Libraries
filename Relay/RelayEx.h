#ifndef RelayEx_H_
#define RelayEx_H_

#include "Arduino.h"
#include <Relay.h>

class MyMessage;

class RelayEx : public Relay
{
private:
    static byte MYSENSORS_OFFSET;           // this identifier is the sensors type offset in MySensors register
    static MyMessage* MYMESSAGE_ACCESSOR;   // reference to global message to controller, used to construct messages "on the fly"

public:
    RelayEx(unsigned long minToggleMillis = 0);
    RelayEx(const RelayEx& other);
    virtual ~RelayEx();
    RelayEx& operator=(const RelayEx& other);

    // Sequence number in the dimmers array (group ID)
    virtual void setSequenceNumber(byte value);
    virtual byte getSequenceNumber() const;

    // MySensors message interface
    static void setMyMessageAccessor(MyMessage* myMessageAccessor);
    static MyMessage* getMyMessageAccessor();

    // Dimmers offset (as a group) in the MySensors GateWay
    static void setMySensorsOffset(byte value);
    static byte getMySensorsOffset();

protected:
    virtual void On();
    virtual void Off();

private:
    void CopyFrom(const RelayEx& other);
    void sendMessage_Controller(byte type, byte command);

    // this identifier helps to hide implementation of manage MySensors auto acknowledgement
    byte _sequenceNumber;
};

#endif // RelayEx_H_