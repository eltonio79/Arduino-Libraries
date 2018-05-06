/* THREE STATE, UNIVERSAL, NOT-BLOCKING, DIMMER BASE CLASS */

#ifndef DIMMEREX_H_
#define DIMMEREX_H_

#include <Arduino.h>

// The minimum time (milliseconds) the program will wait between LED adjustments
// Adjust this define to modify performance

//dodaæ dimming curve - ze wzoru i z tabeli sterowane static memberem w ca³ej klasie lub wy³¹czane (enum raw calc led8 led10 led12 led16 mains)

// przyklad uzycia "dimming curve ze wzoru" // https://diarmuid.ie/blog/pwm-exponential-led-fading-on-arduino-or-other-platforms/
// The number of Steps between the output being on and off
//const int dimIntervals = 100; // DimmerEx::VALUE_MAX - DimmerEx::VALUE_MIN
// The R value in the graph equation
// float R = 0.0f;
// Calculate the R variable (only needs to be done once - at setup())
//R = (dimIntervals * log10(2)) / (log10(255));
//for (int interval = 0; interval <= dimIntervals; interval++)
// {
//    // Calculate the required PWM value for this interval step
//    brightness = pow(2, (interval / R)) - 1;
//    // Set the LED output to the calculated brightness
//    analogWrite(transistorPin, brightness);
//    delay(7);                                   // delay (n) where n is time to full brightness in milliseconds
// }

class DimmerEx
{
protected:
    static byte          VALUE_MIN;
    static byte          VALUE_MAX;
    static unsigned long FADE_DURATION_OFF;
    static unsigned long FADE_DURATION_MIN;
    static unsigned long FADE_DURATION_MAX;

public:
    DimmerEx();
    DimmerEx(const DimmerEx& other);
    virtual ~DimmerEx();
    DimmerEx& operator=(const DimmerEx& other);

    // Member setters / getters
    virtual void setValue(byte value); // acceted values from 0 to 100 (percent of the light)
    byte getValue() const;

    // Calculates RAW dimming value (acceptable by hardware, derived dimmer)
    virtual unsigned int getValueRaw() const;

    // Three-state functionality
    bool isSwitchedOn() const;

    void switchOn();
    void switchToggleOn();
    void switchLast();
    void switchOff();
    void switchToggle(bool threeStateMode);

    // Dimming functionality
    bool isFading() const;
    byte getFadeProgress() const; // returns current dimming progress in percentage ( 0 - 100 % )

    void startFade(byte fadeValue, unsigned long duration);
    void stopFade();

    // This function has to be called in every processor tick / in the loop(s).
    // It is used to deliver non-code-blocking fade effect.
    void update();

    // Sequence number in the dimmers array (group ID)
    void setSequenceNumber(byte value);
    byte getSequenceNumber() const;

private:
    void CopyFrom(const DimmerEx& other);

protected:
    // STATE MEMBERS
    byte _value;
    byte _lastValue;
    // NO-DELAY FADING MEMBERS
    byte _fadeFromValue;
    byte _fadeToValue;
    unsigned long _fadeDuration;
    unsigned long _fadeInterval;
    unsigned long _fadeLastStepTime;
    // this identifier helps to hide implementation of comunicate with external things (MySensors)
    byte _sequenceNumber;
};

#endif // DIMMEREX_H_