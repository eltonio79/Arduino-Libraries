#ifndef GRAVITY_TDS_H
#define GRAVITY_TDS_H

#include "Arduino.h"

#define SERIAL_BUFFER_LENGTH 15

#define EC_TDS_FACTOR 0.5f
#define EC_MILLIS_FACTOR 1000.f

class GravityTDS
{
public:
    GravityTDS(byte pinSensor, byte pinPowerEc, byte pinPowerPh, byte samplesCount);
    ~GravityTDS();

    void begin();
    void update();

    void setPin(int pin); 
    void setTemperature(float temp); // set the temperature and execute temperature compensation
    void setAnalogReferenceVoltage(float value); // reference voltage on ADC, default 5.0V on Arduino UNO
    void setADCResolution(float range); // 1024 for 10bit ADC; 4096 for 12bit ADC
    void setCalibrationValuesAddress(int address); // set the EEPROM address to store the k value,default address: 0x08

    float getAnalogRawValue();
    float getAnalogCalcValue();
    float getTdsValue();
    float getEcValue();

private:
    byte _pinSensor;
    byte _pinPowerEc;
    byte _pinPowerPh;

    byte _numberOfSamples;
    uint16_t* _samples;
    byte _currentSampleIndex;

    float _adcReferenceVoltage;
    float _adcResolution;

    float _temperature; // temperature
    float _analogValueRaw; // analog pin direct reading
    float _analogValueCalc; // analog pin calcuated (prognosed) value
    float _resistance; // analog pin input voltage (from 0 to _adcResolution value)
    float _ecValue; // ec value after temperature compensation
    float _tdsValue; // tds value after temperature compensation

    int   _calValueAddress;
    float _calTemperature;
    float _calLowResistance;
    float _calHighResistance;
    float _calLowEC;
    float _calHighEC;
    float _calCurrentResistance;

    char _cmdReceivedBuffer[SERIAL_BUFFER_LENGTH + 1];
    byte _cmdReceivedBufferIndex;

    void takeSample();

    void readCalibrationValues();
    bool cmdSerialDataAvailable();
    void ecCalibration(byte mode);

    byte cmdParse();
};

#endif
