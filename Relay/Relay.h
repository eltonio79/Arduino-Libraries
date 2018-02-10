#ifndef Relay_H_
#define Relay_H_

#include "Arduino.h"

class MyMessage;

class Relay
{
private:
    // Properties stored inside EEPROM
    bool _isOn;

    // internal members
    byte _pin;
    bool _isTiming;
    unsigned long _triggerStartMillis;
    unsigned long _triggerDelayMillis;
    unsigned long _minToggleMillis;

    // this identifier helps to hide implementation of re/storing an array of objects to/from EEPROM
    byte _sequenceNumber;

    // this identifier is the start address in the EEPROM to store the data
    byte _eepromOffset;

    // this identifier is the sensors type offset in MySensors register
    byte _sensorsTypeOffset;

    // reference to global message to controller, used to construct messages "on the fly"
    MyMessage* _myMessageAccessor;

    void CopyFrom(const Relay& other);
    void sendMessage_Controller(byte type, byte command);

public:
    Relay(byte pin);
    Relay(const Relay& other);
    virtual ~Relay();

    Relay& operator=(const Relay& other);

    bool isOn();
    void init(bool bOn, unsigned long minToggleMillis = 0); // , byte eepromAddress
    bool switchOn(unsigned long seconds);
    bool switchOff(unsigned long seconds);
    bool switchOn();
    bool switchOff();
    bool toggle();

    // call it in the main loop to have all functionalities
    void update();

    void setSequenceNumber(byte value);
    byte getSequenceNumber() const;

    void setEEPROMOffset(byte value);
    byte getEEPROMOffset() const;

    void setSensorsTypeOffset(byte value);
    byte getSensorsTypeOffset() const;

    void readEEPROM();
    void writeEEPROM();

    void setMyMessageAccessor(MyMessage* myMessageAccessor);
    MyMessage* getMyMessageAccessor() const;

protected:
    // relay type dependant methods
    virtual void On();
    virtual void Off();
};

#endif // Relay_H_