#ifndef ACDimmer_I2C_H_
#define ACDimmer_I2C_H_

#include <Arduino.h>

// The minimum time (milliseconds) the program will wait between LED adjustments
// Adjust this define to modify performance
#define MIN_FADE_INTERVAL 20

// Size of the data for ACDimmer_I2C class to store in the EEPROM
#define EEPROM_DATA_OFFSET 5
#define EEPROM_DATA_SIZE 5

class MyMessage;

class ACDimmer_I2C
{
public:
    // #param slaveI2CAddress adres sciemniacza I2C (Attiny85)
    // #param minimumValue (0 - 128); dobieraæ eksperymentalnie (per ¿arówka / triak)
    // #param maximumValue (0 - 128); dobieraæ eksperymentalnie (per ¿arówka / triak)
    ACDimmer_I2C(byte slaveI2CAddress = 0, byte minimumValue = 128, byte maximumValue = 0);

    ACDimmer_I2C(const ACDimmer_I2C& other);
    ~ACDimmer_I2C();

    // #param value ( 0 - 100 % )
    void setValue(byte value, bool store = false);
    byte getValue() const;
    byte getValueRaw() const;

    void setMinimumValue(byte value);
    byte getMinimumValue() const;

    void setMaximumValue(byte value);
    byte getMaximumValue() const;

    void setSlaveI2CAddress(byte value);
    byte getSlaveI2CAddress() const;

    void setMyMessageAccessor(MyMessage* myMessageAccessor);
    MyMessage* getMyMessageAccessor() const;

    void setSequenceNumber(byte value);
    byte getSequenceNumber() const;

    void readEEPROM();
    void writeEEPROM();

    bool isSwitchedOn() const;
    void switchOn();
    void switchToggleOn();
    void switchLast();
    void switchOff();
    void switchToggle(bool threeStateMode = false);

    bool isFading() const;
    void startFade(byte fadeValue, unsigned long duration);
    void stopFade();

    // returns current dimming progress in percentage ( 0 - 100 % )
    byte getFadeProgress() const;

    // This function has to be called in every processor tick / in the loop(s).
    // It is used to deliver non-code-blocking fade effect.
    void update();

private:
    void sendMessage_I2C(byte message);
    void sendMessage_Controller(byte type, byte command);

    // Properties stored inside EEPROM
    byte _value;
    byte _lastValue;
    byte _minimumValue;
    byte _maximumValue;
    byte _slaveI2CAddress;

    // Unblocking dimming properties
    byte _fadeFromValue;
    byte _fadeToValue;
    unsigned long _fadeInterval;
    unsigned long _fadeDuration;
    unsigned long _fadeLastStepTime;

    // this identifier helps to hide implementation of re/storing an array of objects to/from EEPROM
    byte _sequenceNumber;

    // reference to global message to controller, used to construct messages "on the fly"
    MyMessage* _myMessageAccessor;
};

#endif // ACDimmer_I2C_H_