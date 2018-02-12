/* THREE STATE, UNIVERSAL, NOT-BLOCKING, DIMMER BASE CLASS */

#ifndef DIMMEREX_H_
#define DIMMEREX_H_

#include <Arduino.h>

// The minimum time (milliseconds) the program will wait between LED adjustments
// Adjust this define to modify performance

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

    // #param acceted values from 0 to 100 (percent of the light)
    virtual void setValue(byte value, bool store);
    byte getValue() const;

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
};

#endif // DIMMEREX_H_