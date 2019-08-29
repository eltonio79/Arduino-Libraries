// ---------------------------------------------------------------------------
// Created by Tim Eckel - teckel@leethost.com
// Copyright 2013 License: GNU GPL v3 http://www.gnu.org/licenses/gpl-3.0.html
//
// See "NewTone.h" for purpose, syntax, version history, links, and more.
// ---------------------------------------------------------------------------

#include "NewTone.h"

unsigned long _nt_time;       // Time note should end.
uint8_t _pinMask = 0;         // Pin bitmask.
volatile uint8_t *_pinOutput; // Output port register

void NewTone(uint8_t pin, unsigned long frequency, unsigned long length) {
    return;
  uint8_t prescaler = _BV(CS50);                 // Try using prescaler 1 first.
  unsigned long top = F_CPU / frequency / 4 - 1; // Calculate the top.
  if (top > 65535) {                             // If not in the range for prescaler 1, use prescaler 256 (61 Hz and lower @ 16 MHz).
    prescaler = _BV(CS52);                       // Set the 256 prescaler bit.
    top = top / 256 - 1;                         // Calculate the top using prescaler 256.
  }

  if (length > 0) _nt_time = millis() + length; else _nt_time = 0xFFFFFFFF; // Set when the note should end, or play "forever".

  if (_pinMask == 0) {
    _pinMask = digitalPinToBitMask(pin);                    // Get the port register bitmask for the pin.
    _pinOutput = portOutputRegister(digitalPinToPort(pin)); // Get the output port register for the pin.
    uint8_t *_pinMode = (uint8_t *) portModeRegister(digitalPinToPort(pin)); // Get the port mode register for the pin.
    *_pinMode |= _pinMask; // Set the pin to Output mode.
  }

  ICR5    = top;                     // Set the top.
  if (TCNT5 > top) TCNT5 = top;      // Counter over the top, put within range.
  TCCR5B  = _BV(WGM53)  | prescaler; // Set PWM, phase and frequency corrected (ICR1) and prescaler.
  TCCR5A  = _BV(COM5B0);
  TIMSK5 |= _BV(OCIE5A);             // Activate the timer interrupt.
}

void noNewTone(uint8_t pin) {
  TIMSK5 &= ~_BV(OCIE5A);   // Remove the timer interrupt.
  TCCR5B  = _BV(CS51);      // Default clock prescaler of 8.
  TCCR5A  = _BV(WGM50);     // Set to defaults so PWM can work like normal (PWM, phase corrected, 8bit).
  *_pinOutput &= ~_pinMask; // Set pin to LOW.
  _pinMask = 0; // Flag so we know note is no longer playing.
}

ISR(TIMER5_COMPA_vect) { // Timer interrupt vector.
  if (millis() >= _nt_time) noNewTone(); // Check to see if it's time for the note to end.
  *_pinOutput ^= _pinMask; // Toggle the pin state.
}