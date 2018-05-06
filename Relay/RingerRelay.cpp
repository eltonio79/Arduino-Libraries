#include "RingerRelay.h"
#include "PCF8574.h"

RingerRelay::RingerRelay(PCF8574& expander, byte pinA, byte pinB) :
RelayEx(0), // better to call it instead of implicite call..
_expander(expander),
_pinA(pinA),
_pinB(pinB),
_previousDingDong(0),
_intervalDingDong(100)
{
    // nie dzia³a tu _expander.write(...) - wiesza MySensors / system !
}

RingerRelay::RingerRelay(const RingerRelay& other) :
RelayEx(other),
_expander(other._expander),
_pinA(other._pinA),
_pinB(other._pinB),
_previousDingDong(other._previousDingDong),
_intervalDingDong(other._intervalDingDong)
{
}

RingerRelay::~RingerRelay()
{
}

RingerRelay& RingerRelay::operator=(const RingerRelay& other)
{
    if (&other == this)
        return *this;

    RelayEx::operator=(other);

    _expander = other._expander;
    _pinA = other._pinA;
    _pinB = other._pinB;
    _previousDingDong = other._previousDingDong;
    _intervalDingDong = other._intervalDingDong;

    return *this;
}

void RingerRelay::update()
{
    static bool dingDong = true;

    if (this->isOn())
    {
        if (millis() - _previousDingDong > _intervalDingDong)
        {
            _previousDingDong = millis();

            if (dingDong)
            {
                _expander.write(_pinA, LOW);
                _expander.write(_pinB, HIGH);
            }
            else
            {
                _expander.write(_pinA, LOW);
                _expander.write(_pinB, LOW);
            }

            dingDong = !dingDong;
        }
    }

    Relay::update();
}

void RingerRelay::On()
{
    RelayEx::On();
}

void RingerRelay::Off()
{
    RelayEx::Off();
}