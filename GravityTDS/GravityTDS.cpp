#include <EEPROM.h>
#include <Statistics.h>
#include <GravityTDS.h>
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

#define SolutionTemperatureDefault     25.f   // solution default temperature
#define SolutionResistanceDefaultLow  20000.f // default low buffer solution resistance
#define SolutionResistanceDefaultHigh 82500.f // default high buffer solution resistance
#define SolutionEcDefaultLow          800.f   // default low buffer solution electro conductivity
#define SolutionEcDefaultHigh        1414.f   // default high buffer solution electro conductivity

#define SolutionResistanceTolerance     1.f // solution tolerance when calibrating / detecting low / high EC environments
#define SolutionTemperatureMin          0.f // solution minimum temperature
#define SolutionTemperatureMax        100.f // solution maximum temperature

using namespace Statistics;

GravityTDS::GravityTDS(byte pinSensor, byte pinPowerEc, byte pinPowerPh, byte samplesCount) :
    _pinSensor(pinSensor),
    _pinPowerEc(pinPowerEc),
    _pinPowerPh(pinPowerPh),
    _numberOfSamples(samplesCount),
    _samples(nullptr),
    _currentSampleIndex(0),
    _adcReferenceVoltage(5.f),
    _adcResolution(1024.f),
    _temperature(SolutionTemperatureDefault),
    _analogValueCalc(0.f),
    _resistance(0.f),
    _ecValue(0.f),
    _tdsValue(0.f),
    _calValueAddress(32),
    _calTemperature(SolutionTemperatureDefault),
    _calLowResistance(SolutionResistanceDefaultLow),
    _calHighResistance(SolutionResistanceDefaultHigh),
    _calLowEC(SolutionEcDefaultLow),
    _calHighEC(SolutionEcDefaultHigh),
    _calCurrentResistance(SolutionResistanceDefaultHigh),
    _cmdReceivedBuffer({ 0 }),
    _cmdReceivedBufferIndex(0)
{
    this->_samples = new uint16_t[_numberOfSamples];
}

GravityTDS::~GravityTDS()
{
    delete[] this->_samples;
}

void GravityTDS::setPin(int pin)
{
    this->_pinSensor = pin;
}

void GravityTDS::setTemperature(float temp)
{
    this->_temperature = temp;
}

void GravityTDS::setAnalogReferenceVoltage(float value)
{
    this->_adcReferenceVoltage = value;
}

void GravityTDS::setADCResolution(float range)
{
    this->_adcResolution = range;
}

void GravityTDS::setCalibrationValuesAddress(int address)
{
    this->_calValueAddress = address;
}

void GravityTDS::begin()
{
    pinMode(this->_pinSensor, INPUT);

    readCalibrationValues();

    for (int i = 0; i < this->_numberOfSamples; ++i)
    {
        analogRead(this->_pinSensor); // dummy call (solution taken from some Arduino forum thread..)
        this->_samples[i] = analogRead(this->_pinSensor);
    }
}

void GravityTDS::takeSample()
{
    // turn off PH module and turn on EC
    bool phPowerState = digitalRead(_pinPowerPh);
    if (phPowerState)
    {
        // turn off PH module
        digitalWrite(_pinPowerPh, LOW);
        delay(1);
    }

    // turn on EC module
    digitalWrite(_pinPowerEc, HIGH);
    delay(150);

    // dummy call to "wakeup" analog / digital converter
    analogRead(_pinSensor);

    // update readings table by fresh reading
    this->_analogValueRaw = analogRead(_pinSensor);
    _samples[_currentSampleIndex] = this->_analogValueRaw;

    // increase and roll over current sample index
    _currentSampleIndex = (_currentSampleIndex + 1) % _numberOfSamples;

    // calculate mediana reading
    this->_analogValueCalc = getMediana(_samples, _numberOfSamples);

    // turn off EC module when reading is done
    digitalWrite(_pinPowerEc, LOW);

    if (phPowerState)
    {
        // turn on PH module if it was before
        digitalWrite(_pinPowerPh, HIGH);
    }
}

/* void GravityTDS::TakeSampleOther()
{
    // Po testach wysz³o, ¿e zwyk³y analogRead w tym przypadku daje najlepsze wyniki.
    //analogRead(this->_pinSensor); // dummy call (solution taken from some Arduino forum thread..)

    this->_analogValueCalc = getMediana(_samples, _numberOfSamples);
    //this->_analogValueCalc = getStandardDeviation(this->_pinSensor, _samples, _numberOfSamples);
    this->_voltage = getVoltage(this->_analogValueCalc, this->_adcResolution, this->_adcReferenceVoltage);

    this->_ecValueRAW = (133.42f *
        this->_voltage *
        this->_voltage *-
        this->_voltage -
        255.86f *
        this->_voltage *
        this->_voltage +
        857.39f *
        this->_voltage) *
        this->_calValue;

    this->_ecValue = this->_ecValueRAW / (SolutionKValueDefault + 0.02f * (this->_temperature - SolutionTemperatureDefault)); // temperature compensation
    this->_tdsValue = _ecValue * EC_TDS_FACTOR;

    if (cmdSerialDataAvailable() > 0)
    {
        ecCalibration(cmdParse()); // if received serial cmd from the serial monitor, enter into the calibration mode
    }
}*/

void GravityTDS::update()
{
    // take a reading
    takeSample();

    // calculate Resistance
    this->_resistance = getResistance(this->_analogValueCalc);

    // calculate EC from Resistance
    this->_ecValue = resistanceToConductivity(_resistance, _temperature, _calLowEC, _calLowResistance, _calHighEC, _calHighResistance, _calTemperature) / EC_MILLIS_FACTOR;

    // calculate TDS from EC
    this->_tdsValue = _ecValue * EC_TDS_FACTOR;

    // if received serial cmd from the serial monitor, enter into the calibration mode
    if (cmdSerialDataAvailable() > 0)
    {
        ecCalibration(cmdParse());
    }
}

float GravityTDS::getAnalogRawValue()
{
    return _analogValueRaw;
}

float GravityTDS::getAnalogCalcValue()
{
    return _analogValueCalc;
}

float GravityTDS::getTdsValue()
{
    return _tdsValue;
}

float GravityTDS::getEcValue()
{
    return _ecValue;
}

void GravityTDS::readCalibrationValues()
{
    EEPROM_read(this->_calValueAddress + 0, this->_calLowResistance);

    Serial.print("EC Calibration low resistance: ");
    Serial.println(this->_calLowResistance);

    // if EEPROM is uninitialized, write default calibration value
    if (EEPROM.read(this->_calValueAddress + 0) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 1) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 2) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 3) == 0xFF)
    {
        this->_calLowResistance = SolutionResistanceDefaultLow;
        EEPROM_write(this->_calValueAddress + 0, this->_calLowResistance);
    }

    EEPROM_read(this->_calValueAddress + 4, this->_calHighResistance);

    Serial.print("EC Calibration high resistance: ");
    Serial.println(this->_calHighResistance);

    // if EEPROM is uninitialized, write default calibration value
    if (EEPROM.read(this->_calValueAddress + 4) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 5) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 6) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 7) == 0xFF)
    {
        this->_calHighResistance = SolutionResistanceDefaultHigh;
        EEPROM_write(this->_calValueAddress + 4, this->_calHighResistance);
    }

    EEPROM_read(this->_calValueAddress + 8, this->_calLowEC);

    Serial.print("EC Calibration low EC: ");
    Serial.println(this->_calLowEC);

    // if EEPROM is uninitialized, write default calibration value
    if (EEPROM.read(this->_calValueAddress +  8) == 0xFF &&
        EEPROM.read(this->_calValueAddress +  9) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 10) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 11) == 0xFF)
    {
        this->_calLowEC = SolutionEcDefaultLow;
        EEPROM_write(this->_calValueAddress + 8, this->_calLowEC);
    }

    EEPROM_read(this->_calValueAddress + 12, this->_calHighEC);

    Serial.print("EC Calibration high EC: ");
    Serial.println(this->_calHighEC);

    // if EEPROM is uninitialized, write default calibration value
    if (EEPROM.read(this->_calValueAddress + 12) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 13) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 14) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 15) == 0xFF)
    {
        this->_calHighEC = SolutionEcDefaultHigh;
        EEPROM_write(this->_calValueAddress + 12, this->_calHighEC);
    }

    EEPROM_read(this->_calValueAddress + 16, this->_calTemperature);

    Serial.print("EC Calibration temperature: ");
    Serial.println(this->_calTemperature);

    // if EEPROM is uninitialized, write default calibration value
    if (EEPROM.read(this->_calValueAddress + 16) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 17) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 18) == 0xFF &&
        EEPROM.read(this->_calValueAddress + 19) == 0xFF)
    {
        this->_calTemperature = SolutionTemperatureDefault; // new EEPROM, write typical voltage
        EEPROM_write(this->_calValueAddress + 16, this->_calTemperature);
    }
}

boolean GravityTDS::cmdSerialDataAvailable()
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

byte GravityTDS::cmdParse()
{
    byte modeIndex = 0;

    if (strstr(_cmdReceivedBuffer, "ENTERTDS") != NULL)
    {
        modeIndex = 1;
    }
    else if (strstr(_cmdReceivedBuffer, "CALTDS:") != NULL)
    {
        modeIndex = 2;
    }
    else if (strstr(_cmdReceivedBuffer, "EXITTDS") != NULL)
    {
        modeIndex = 3;
    }

    return modeIndex;
}

void GravityTDS::ecCalibration(byte mode)
{
    static bool phCalibrationFinish = false;
    static bool enterCalibrationFlag = false;

    switch (mode)
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
            Serial.println(F(">>> Enter EC Calibration Mode <<<"));
            Serial.println(F(">>> Please put the probe into the 400 or 707 ms/cm buffer solution <<<"));
            Serial.println();

            break;
        }
        case 2:
        {
            // automatically identify which of the two standard buffer solutions is used
            if (enterCalibrationFlag)
            {
                // auto recognize low buffer solution (400 ppm)
                if ((this->_calCurrentResistance > (SolutionResistanceDefaultLow - SolutionResistanceTolerance)) && (this->_calCurrentResistance < (SolutionResistanceDefaultLow + SolutionResistanceTolerance)))
                {
                    Serial.println();
                    Serial.print(F(">>> Buffer Solution: 400 ppm"));

                    this->_calLowResistance = this->_calCurrentResistance;

                    Serial.println(F(", send EXITPH to Save and Exit <<<"));
                    Serial.println();

                    phCalibrationFinish = true;
                }
                // auto recognize acid buffer solution (707 ppm)
                else if ((this->_calCurrentResistance > (SolutionResistanceDefaultHigh - SolutionResistanceTolerance)) && (this->_calCurrentResistance < (SolutionResistanceDefaultHigh + SolutionResistanceTolerance)))
                {
                    Serial.println();
                    Serial.print(F(">>> Buffer Solution: 707 ppm"));

                    this->_calHighResistance = this->_calCurrentResistance;

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
                    if ((this->_calCurrentResistance > (SolutionResistanceDefaultLow - SolutionResistanceTolerance)) && (this->_calCurrentResistance < (SolutionResistanceDefaultLow + SolutionResistanceTolerance)))
                    {
                        EEPROM_write(this->_calValueAddress + 0, this->_calLowResistance);
                    }
                    else if ((this->_calCurrentResistance > (SolutionResistanceDefaultHigh - SolutionResistanceTolerance)) && (this->_calCurrentResistance < (SolutionResistanceDefaultHigh + SolutionResistanceTolerance)))
                    {
                        EEPROM_write(this->_calValueAddress + 4, this->_calHighResistance);
                    }

                    if (this->_calTemperature > SolutionTemperatureMin && this->_calTemperature < SolutionTemperatureMax)
                    {
                        EEPROM_write(this->_calValueAddress + 8, this->_calTemperature);
                    }

                    Serial.print(F(">>> EC Calibration Successful"));
                }
                else
                {
                    Serial.print(F(">>> EC Calibration Failed"));
                }

                Serial.println(F(", exiting EC Calibration Mode <<<"));
                Serial.println();

                phCalibrationFinish = false;
                enterCalibrationFlag = false;
            }

            break;
        }
    }
}
