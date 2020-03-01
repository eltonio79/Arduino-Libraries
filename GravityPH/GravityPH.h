// TODO - rozszerz o kalibracjê ZASAD.. jak tu:
// https://github.com/Libelium/waspmoteapi/blob/master/libraries/SensorSW/WaspSensorSW.cpp#L280

#ifndef _GRAVITYPH_H_
#define _GRAVITYPH_H_

#include "Arduino.h"

#define SERIAL_BUFFER_LENGTH 15

class GravityPH
{
public:
    GravityPH(byte pinSensor, byte pinPowerEc, byte pinPowerPh, byte samplesCount);
    ~GravityPH();

    void begin();
    void update();

    void calibration(float voltage, float temperature);

    void setPin(int pin); // set analog pin for PH readings
    void setTemperature(float temp); // set the temperature and execute temperature compensation
    void setCalibrationValuesAddress(int address); // set the EEPROM address to store the pH calibration values, default address: 0x00
    void setAnalogReferenceVoltage(float voltage); // reference voltage on ADC, default 5.0V on Arduino UNO
    void setADCResolution(float resolution); // 1024 for 10bit ADC; 4096 for 12bit ADC

    float getAnalogRawValue();
    float getAnalogCalcValue();
    float getPhValue();

private:
    byte _pinSensor;
    byte _pinPowerEc;
    byte _pinPowerPh;

    byte _numberOfSamples;
    uint16_t* _samples;
    byte _currentSampleIndex;

    float _adcReferenceVoltage; // reference voltage on ADC, default 5.0V on Arduino UNO
    float _adcResolution; // 1024 for 10bit ADC; 4096 for 12bit ADC

    float _temperature;
    float _analogValueRaw;
    float _analogValueCalc;
    float _voltage;
    float _phValue;

    int _calValueAddress; // the address of the calibration value stored in the EEPROM
    float _calTemperature;
    float _calAcidVoltage;
    float _calNeutralVoltage;
    float _calCurrentVoltage;

    char _cmdReceivedBuffer[SERIAL_BUFFER_LENGTH + 1];
    byte _cmdReceivedBufferIndex;

    void takeSample();
    float calculatePH();

    void readCalibrationValues();
    bool cmdSerialDataAvailable();
    void phCalibration(byte mode);

    byte cmdParse();
};

#endif
