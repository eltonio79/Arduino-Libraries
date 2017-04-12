#ifndef ACDimmer_I2C_H_
#define ACDimmer_I2C_H_

class ACDimmer_I2C
{
public:
    // #param minimumValue (0 - 128); dobrac eksperymentalnie
    // #param maximumValue (0 - 128); dobrac eksperymentalnie
    // #param slaveI2CAddress adres sciemniacza I2C (Attiny85)
    ACDimmer_I2C(byte minimumValue, byte maximumValue, byte slaveI2CAddress) :
        _value(0),
        _minimumValue(minimumValue),
        _maximumValue(maximumValue),
        _slaveI2CAddress(slaveI2CAddress)
        {
        };
    ~ACDimmer_I2C() {};

    // #param value (0 - 100%)
    void setValue(byte value)
    {
        if (value > 100) // sanity check
            value = 100;

        _value = value;

        SendI2CCommand();
    };

    byte getValue() const { return _value; };

    void setMinimumValue(byte value) { _minimumValue = value; };
    byte getMinimumValue() const { return _minimumValue; };

    void setMaximumValue(byte value) { _maximumValue = value; };
    byte getMaximumValue() const { return _maximumValue; };

private:
    void SendI2CCommand()
    {
        byte valueRaw = 128;

        if (_value > 0)
            valueRaw = map(_value, 1, 100, _minimumValue, _maximumValue);

        Wire.beginTransmission(_slaveI2CAddress);
        Wire.write(valueRaw);
        Wire.endTransmission();
    }

    byte _value;
    byte _minimumValue;
    byte _maximumValue;
    byte _slaveI2CAddress;
};

#endif // ACDimmer_I2C_H_