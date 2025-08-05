/* ---------------------------------------------------------------------------------------

  PEEPSY

  by Michael Egger [ a n y m a ]

  based on Continuity Tester by David Johnson-Davies - www.technoblogy.com - 18th November 2017
  ATTINY402 @ 1 MHz (internal oscillator)

  CC BY 4.0
  Licensed under a Creative Commons Attribution 4.0 International license:
  http://creativecommons.org/licenses/by/4.0/

--------------------------------------------------------------------------------------- */
/* #undef MILLIS_USE_TIMERA0
 */
#define MILLIS_USE_TIMERB0

#include <Arduino.h>
#include "Comparator.h"

// ==============================================================================
// - Defines
// ------------------------------------------------------------------------------
#include <avr/sleep.h>
#include <avr/power.h>

#ifndef MILLIS_USE_TIMERB0
#error "This sketch is written for use with TCB0 as the millis timing source"
#endif

const int Pin_Led = PIN_PA3;
const int Pin_Reference = PIN_PA7; // AINP0
const int Pin_Probe = PIN_PA6;     // AINN0
const int Pin_Speaker_A = PIN_PA2; //  WO1
const int Pin_Speaker_B = PIN_PA1; //  WO2
/*
const int beepFrequency = 4000; */

// ==============================================================================
// - Globals

const unsigned long Timeout = (unsigned long)20 * 1000; // twenty seconds
volatile unsigned long Time;

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

  //TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
  digitalWrite(Pin_Led, HIGH);
}

// ----------------------------------------------------------------------------------------
void NoBeep()
{
 // TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
  digitalWrite(Pin_Led, LOW);
}


// ==============================================================================
// - init
// ------------------------------------------------------------------------------

void setup()
{
  pinMode(Pin_Reference, INPUT_PULLUP);
  pinMode(Pin_Probe, INPUT_PULLUP);
  pinMode(Pin_Led, OUTPUT);
  pinMode(Pin_Speaker_A, OUTPUT);
  pinMode(Pin_Speaker_B, OUTPUT);
  digitalWrite(Pin_Speaker_B, LOW);
  digitalWrite(Pin_Speaker_A, LOW);

/*   takeOverTCA0();
  TCA0.SINGLE.CMP0 = 800;
  TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_FRQ_gc | TCA_SINGLE_CMP1EN_bm | TCA_SINGLE_CMP2EN_bm;
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc ;
  PORTA.PIN2CTRL |= PORT_INVEN_bm;
 */
  Comparator.init();

  attachInterrupt(digitalPinToInterrupt(Pin_Probe), wakeUp, FALLING);

  // Power saving
  /* ADCSRA &= ~(1 << ADEN);									// Disable ADC to save power
  PRR = 1 << PRUSI | 1 << PRADC;							// Turn off unused clocks */

  // Setup Analog Comparator
/*   AC0.MUXCTRLA = AC_INVERT_bm;
  AC0.CTRLA = AC_ENABLE_bm; */

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




  return;





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
