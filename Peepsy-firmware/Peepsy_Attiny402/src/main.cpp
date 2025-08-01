/* ---------------------------------------------------------------------------------------

  PEEPSY

  by Michael Egger [ a n y m a ]

  based on Continuity Tester by David Johnson-Davies - www.technoblogy.com - 18th November 2017
  ATTINY402 @ 1 MHz (internal oscillator)

  CC BY 4.0
  Licensed under a Creative Commons Attribution 4.0 International license:
  http://creativecommons.org/licenses/by/4.0/

--------------------------------------------------------------------------------------- */

#include <Arduino.h>
#include "Comparator.h"

// ==============================================================================
// - Defines
// ------------------------------------------------------------------------------
#include <avr/sleep.h>
#include <avr/power.h>

const int Pin_Led = PIN_PA3;
const int Pin_Reference = PIN_PA7; // AINP0
const int Pin_Probe = PIN_PA6;     // AINN0
const int Pin_Speaker_A = PIN_PA2;
const int Pin_Speaker_B = PIN_PA1;

// ==============================================================================
// - Globals

const unsigned long Timeout = (unsigned long)20 * 1000; // twenty seconds
volatile unsigned long Time;
long lastblink;
bool Sense;

// ----------------------------------------------------------------------------------------
// Pin change interrupt service routine - resets sleep timer
void wakeUp()
{
  Time = millis();
}

// ----------------------------------------------------------------------------------------
void Beep()
{
  // TCCR1 = TCCR1 | 3;											// Counter = clock/4
  // tone(Pin_Speaker_A, 2000);
  analogWrite(Pin_Speaker_A, 127);
  digitalWrite(Pin_Led, HIGH);
}

// ----------------------------------------------------------------------------------------
void NoBeep()
{
  // TCCR1 = TCCR1 & ~3;											// Counter stopped
  // noTone(Pin_Speaker_A);
  analogWrite(Pin_Speaker_A, 0);

  digitalWrite(Pin_Led, LOW);
}

// ==============================================================================
// - init
// ------------------------------------------------------------------------------

void setup()
{
  Comparator.init();

  pinMode(Pin_Reference, INPUT_PULLUP);
  pinMode(Pin_Probe, INPUT_PULLUP);
  pinMode(Pin_Led, OUTPUT);
  pinMode(Pin_Speaker_A, OUTPUT);
  pinMode(Pin_Speaker_B, OUTPUT);
  digitalWrite(Pin_Speaker_B, LOW);
  // Pin-change interrupt
  attachInterrupt(digitalPinToInterrupt(Pin_Probe), wakeUp, FALLING);

  // Power saving
  /* ADCSRA &= ~(1 << ADEN);									// Disable ADC to save power
  PRR = 1 << PRUSI | 1 << PRADC;							// Turn off unused clocks */

  // Setup Analog Comparator
  AC0.MUXCTRLA = AC_INVERT_bm;
  AC0.CTRLA = AC_ENABLE_bm;

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Start running
  Time = millis();
  Comparator.start();
}

// ==============================================================================
// - LOOP
// ------------------------------------------------------------------------------
void loop()
{
  Sense = !Comparator.read();

  if (Sense)
  {
    Beep();
  }
  else

  {
    NoBeep();
  }

  // Go to sleep?
  if (millis() - Time > Timeout)
  {
    digitalWrite(Pin_Led, LOW);
    pinMode(Pin_Reference, INPUT); // Turn off pullup to save power
    sleep_enable();
    sleep_cpu();
    // Carry on here when we wake up
    pinMode(Pin_Reference, INPUT_PULLUP);
  }
}
