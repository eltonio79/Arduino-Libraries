#include <GravityPH.h>
#include <EEPROM.h>
#include <Statistics.h>
#include <Streaming.h>

#define EEPROM_write(address, p)\
{\
    unsigned int i = 0;\
    byte *pp = reinterpret_cast<byte*>(&p);\
    for(; i < sizeof(p); ++i)\
    {\
        EEPROM.write(address  +i, pp[i]);\
    }\
}

#define EEPROM_read(address, p)\
{\
    unsigned int i = 0;\
    byte *pp = reinterpret_cast<byte*>(&p);\
    for(; i < sizeof(p); ++i)\
    {\
        pp[i] = EEPROM.read(address + i);\
    }\
}

#define SolutionTemperatureDefault      25.f // solution default temperature
#define SolutionCalibrationPhAcid      4.01f // solution acid calibration pH
#define SolutionCalibrationPhNeutral   6.86f // solution neutral calibration pH

#define SolutionVoltageAcidDefault    3.125f // default acid buffer solution voltage
#define SolutionVoltageNeutralDefault 2.608f // default neutral buffer solution voltage

#define SolutionVoltageTolerance       0.15f // solution tolerance when calibrating / detecting acid / neutral environments
#define SolutionTemperatureMin           0.f // solution minimum temperature
#define SolutionTemperatureMax         100.f // solution maximum temperature

using namespace Statistics;

GravityPH::GravityPH(byte pinSensor,
                     byte pinPowerEc,
                     byte pinPowerPh,
                     byte samplesCount) :
    _pinSensor(pinSensor),
    _pinPowerEc(pinPowerEc),
    _pinPowerPh(pinPowerPh),
    _numberOfSamples(samplesCount),
    _samples(nullptr),
    _currentSampleIndex(0),
    _adcReferenceVoltage(5.f),
    _adcResolution(1024.f),
    _temperature(0.f),
    _analogValueCalc(0.f),
    _voltage(0.f),
    _phValue(0.f),
    _calValueAddress(0),
    _calTemperature(SolutionTemperatureDefault),
    _calAcidVoltage(SolutionVoltageAcidDefault),
    _calNeutralVoltage(SolutionVoltageNeutralDefault),
    _calCurrentVoltage(SolutionVoltageNeutralDefault),
    _cmdReceivedBuffer({ 0 }),
    _cmdReceivedBufferIndex(0)
{
    this->_samples = new uint16_t[_numberOfSamples];
}

GravityPH::~GravityPH()
{
    delete[] this->_samples;
}

void GravityPH::begin()
{
    readCalibrationValues();

    pinMode(this->_pinSensor, INPUT);

    for (int i = 0; i < this->_numberOfSamples; ++i)
    {
        analogRead(this->_pinSensor); // dummy call (solution taken from some Arduino forum thread..)
        this->_samples[i] = analogRead(this->_pinSensor);
    }
}

void GravityPH::takeSample()
{
    // turn off EC module and turn on PH
    bool ecPowerState = digitalRead(_pinPowerEc);
    if (ecPowerState)
    {
        // turn off EC module
        digitalWrite(_pinPowerEc, LOW);
        delay(1);
    }

    // turn on PH module
    digitalWrite(_pinPowerPh, HIGH);
    delay(150);

    // update readings table by fresh reading
    analogRead(_pinSensor);
    this->_analogValueRaw = analogRead(_pinSensor);
    _samples[_currentSampleIndex] = this->_analogValueRaw;

    // increase and roll over current sample index
    _currentSampleIndex = (_currentSampleIndex + 1) % _numberOfSamples;

    // this->_analogValueCalc = getStandardDeviation(_samples, _numberOfSamples);
    this->_analogValueCalc = getMediana(_samples, _numberOfSamples);

    // turn off PH module when reading is done
    digitalWrite(_pinPowerPh, LOW);

    if (ecPowerState)
    {
        // turn on EC module if it was before
        digitalWrite(_pinPowerEc, HIGH);
    }
}

void GravityPH::update()
{
    // take a reading
    takeSample();

    // calculate voltage
    this->_voltage = getVoltage(this->_analogValueCalc, this->_adcResolution, this->_adcReferenceVoltage);

    // convert voltage to pH with temperature compensation
    this->_phValue = this->calculatePH();

    // if received serial cmd from the serial monitor, enter into the calibration mode
    if (cmdSerialDataAvailable() > 0)
    {
        phCalibration(cmdParse());
    }
}

void GravityPH::setPin(int pin)
{
    this->_pinSensor = pin;
}

void GravityPH::setTemperature(float temp)
{
    this->_temperature = temp;
}

void GravityPH::setCalibrationValuesAddress(int address)
{
    this->_calValueAddress = address;
}

void GravityPH::setAnalogReferenceVoltage(float voltage)
{
    this->_adcReferenceVoltage = voltage;
}

void GravityPH::setADCResolution(float resolution)
{
    _adcResolution = resolution;
}

float GravityPH::getAnalogRawValue()
{
    return _analogValueRaw;
}

float GravityPH::getAnalogCalcValue()
{
    return _analogValueCalc;
}

float GravityPH::getPhValue()
{
    return _phValue;
}

float GravityPH::calculatePH()
{
    if ((_temperature < SolutionTemperatureMin) ||
        (_temperature > SolutionTemperatureMax))
    {
        return -1.f;
    }

    // The value at pH 6.86 is taken as reference
    float voltageCalibrationZero = _calNeutralVoltage;

    // The sensitivity is calculated using the other two calibration values
    // Asumme that the pH sensor is lineal in the range.
    // sensitivity = pHVariation / volts
    // divided by 'real' PH sollution difference = (pH) 6.86 - (pH) 4.01 = 2.85
    float sensitivity = (_calAcidVoltage - _calNeutralVoltage) / (SolutionCalibrationPhNeutral - SolutionCalibrationPhAcid);

    // Add the change in the conductivity according to the change in temperature (magic number..)
    sensitivity = sensitivity + (_temperature - _calTemperature) * 0.0001984f;

    // pH value calculated from the calibration values
    return SolutionCalibrationPhNeutral + (voltageCalibrationZero - this->_voltage) / sensitivity;
}

void GravityPH::calibration(float voltage, float temperature)
{
    this->_calCurrentVoltage = voltage;
    this->_calTemperature = temperature;

    if(cmdSerialDataAvailable() > 0)
    {
        phCalibration(cmdParse()); // if received Serial CMD from the serial monitor, enter into the calibration mode
    }
}

void GravityPH::readCalibrationValues()
{
    // load the neutral calibration (pH = 6.86) voltage from the EEPROM
    EEPROM_read(this->_calValueAddress + 0, this->_calNeutralVoltage);

    Serial.print("PH Calibration neutral voltage: ");
    Serial.println(this->_calNeutralVoltage);

    // if EEPROM is uninitialized, write default calibration value
    if (EEPROM.read(this->_calValueAddress + 0) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 1) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 2) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 3) == 0xFF)
    {
        this->_calNeutralVoltage = SolutionVoltageNeutralDefault; // new EEPROM, write typical voltage
        EEPROM_write(this->_calValueAddress + 0, this->_calNeutralVoltage);
    }

    // load the acid calibration (pH = 4.01) voltage from the EEPROM
    EEPROM_read(this->_calValueAddress + 4, this->_calAcidVoltage);

    Serial.print("PH Calibration acid voltage: ");
    Serial.println(this->_calAcidVoltage);

    // if EEPROM is uninitialized, write default calibration value
    if (EEPROM.read(this->_calValueAddress + 4) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 5) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 6) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 7) == 0xFF)
    {
        this->_calAcidVoltage = SolutionVoltageAcidDefault; // new EEPROM, write typical voltage
        EEPROM_write(this->_calValueAddress + 4, this->_calAcidVoltage);
    }

    // load the calibration temperature from the EEPROM
    EEPROM_read(this->_calValueAddress + 8, this->_calTemperature);

    Serial.print("PH Calibration temperature: ");
    Serial.println(this->_calTemperature);

    // if EEPROM is uninitialized, write default calibration value
    if (EEPROM.read(this->_calValueAddress +  8) == 0xFF &&
        EEPROM.read(this->_calValueAddress +  9) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 10) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 11) == 0xFF)
    {
        this->_calTemperature = SolutionTemperatureDefault; // new EEPROM, write typical voltage
        EEPROM_write(this->_calValueAddress + 8, this->_calTemperature);
    }
}

boolean GravityPH::cmdSerialDataAvailable()
{
    static unsigned long cmdReceivedTimeOut = millis();
    char cmdReceivedChar = '\0';

    while (Serial.available() > 0)
    {
        if (millis() - cmdReceivedTimeOut > 500U)
        {
            _cmdReceivedBufferIndex = 0;
            memset(_cmdReceivedBuffer, 0, (SERIAL_BUFFER_LENGTH + 1));
        }

        cmdReceivedTimeOut = millis();
        cmdReceivedChar = Serial.read();

        if (cmdReceivedChar == '\n' || _cmdReceivedBufferIndex == SERIAL_BUFFER_LENGTH)
        {
            _cmdReceivedBufferIndex = 0;
            strupr(_cmdReceivedBuffer);

            return true;
        }
        else
        {
            _cmdReceivedBuffer[_cmdReceivedBufferIndex] = cmdReceivedChar;
            _cmdReceivedBufferIndex++;
        }
    }

    return false;
}

byte GravityPH::cmdParse()
{
    byte modeIndex = 0;

    if (strstr(_cmdReceivedBuffer, "ENTERPH") != NULL)
    {
        modeIndex = 1;
    }
    else if (strstr(_cmdReceivedBuffer, "CALPH") != NULL)
    {
        modeIndex = 2;
    }
    else if (strstr(_cmdReceivedBuffer, "EXITPH") != NULL)
    {
        modeIndex = 3;
    }

    return modeIndex;
}

void GravityPH::phCalibration(byte mode)
{
    static bool phCalibrationFinish = false;
    static bool enterCalibrationFlag = false;

    switch(mode)
    {
        case 0:
        {
            if (enterCalibrationFlag)
            {
                Serial.println(F(">>> Command Error <<<"));
            }

            break;
        }
        case 1:
        {
            enterCalibrationFlag = true;
            phCalibrationFinish = false;

            Serial.println();
            Serial.println(F(">>> Enter PH Calibration Mode <<<"));
            Serial.println(F(">>> Please put the probe into the 4.01 or 6.86 standard buffer solution <<<"));
            Serial.println();

            break;
        }
        case 2:
        {
            // automatically identify which of the two standard buffer solutions is used (in this case acid & neutral)
            if (enterCalibrationFlag)
            {
                // auto recognize neutral buffer solution (pH 6.86)
                if ((this->_calCurrentVoltage > (SolutionVoltageNeutralDefault - SolutionVoltageTolerance)) && (this->_calCurrentVoltage < (SolutionVoltageNeutralDefault + SolutionVoltageTolerance)))
                {
                    Serial.println();
                    Serial.print(F(">>> Buffer Solution: 6.86"));

                    this->_calNeutralVoltage = this->_calCurrentVoltage;

                    Serial.println(F(", send EXITPH to Save and Exit <<<"));
                    Serial.println();

                    phCalibrationFinish = true;
                }
                // auto recognize acid buffer solution (pH 4.01)
                else if ((this->_calCurrentVoltage > (SolutionVoltageAcidDefault - SolutionVoltageTolerance)) && (this->_calCurrentVoltage < (SolutionVoltageAcidDefault + SolutionVoltageTolerance)))
                {
                    Serial.println();
                    Serial.print(F(">>> Buffer Solution: 4.01"));

                    this->_calAcidVoltage = this->_calCurrentVoltage;

                    Serial.println(F(", send EXITPH to Save and Exit <<<"));
                    Serial.println();

                    phCalibrationFinish = true;
                }
                // not proper buffer solution or faulty operation
                else
                {
                    Serial.println();
                    Serial.print(F(">>> Buffer Solution Error <<<"));
                    Serial.println();

                    phCalibrationFinish = false;
                }
            }

            break;
        }
        case 3:
        {
            if (enterCalibrationFlag)
            {
                Serial.println();

                if (phCalibrationFinish)
                {
                    if ((this->_calCurrentVoltage > (SolutionVoltageNeutralDefault - SolutionVoltageTolerance)) && (this->_calCurrentVoltage < (SolutionVoltageNeutralDefault + SolutionVoltageTolerance)))
                    {
                        EEPROM_write(this->_calValueAddress + 0, this->_calNeutralVoltage);
                    }
                    else if ((this->_calCurrentVoltage > (SolutionVoltageAcidDefault - SolutionVoltageTolerance)) && (this->_calCurrentVoltage < (SolutionVoltageAcidDefault + SolutionVoltageTolerance)))
                    {
                        EEPROM_write(this->_calValueAddress + 4, this->_calAcidVoltage);
                    }

                    if (this->_calTemperature > SolutionTemperatureMin && this->_calTemperature < SolutionTemperatureMax)
                    {
                        EEPROM_write(this->_calValueAddress + 8, this->_calTemperature);
                    }

                    Serial.print(F(">>> PH Calibration Successful"));
                }
                else
                {
                    Serial.print(F(">>> PH Calibration Failed"));
                }

                Serial.println(F(", exiting PH Calibration Mode <<<"));
                Serial.println();

                phCalibrationFinish = false;
                enterCalibrationFlag = false;
            }

            break;
        }
    }
}
