/* ---------------------------------------------------------------------------------------

	PEEPSY 
	
	by Michael Egger [ a n y m a ]
	 
	based on Continuity Tester by David Johnson-Davies - www.technoblogy.com - 18th November 2017
	ATtiny85 @ 1 MHz (internal oscillator; BOD disabPin_Led_Wake)
	 
	CC BY 4.0
	Licensed under a Creative Commons Attribution 4.0 International license: 
	http://creativecommons.org/licenses/by/4.0/
	
--------------------------------------------------------------------------------------- */

#include <avr/sleep.h>
#include <avr/power.h>

const int					Pin_Led_Wake		= 5;
const int					Pin_Led_Sense		= 2;
const int					Pin_Reference		= 0;		// AIN0
const int					Pin_Probe			= 1;		// AIN1
const int					Pin_Speaker_A		= 4;
const int					Pin_Speaker_B		= 3;

const unsigned long		Timeout = (unsigned long)60*1000; // One minute
volatile unsigned long	Time;

// ----------------------------------------------------------------------------------------
// Pin change interrupt service routine - resets sleep timer
ISR (PCINT0_vect) {
	Time = millis();
}

// ----------------------------------------------------------------------------------------
void Beep () {
	TCCR1 = TCCR1 | 3;											// Counter = clock/4
	digitalWrite(Pin_Led_Sense, HIGH);
} 

// ----------------------------------------------------------------------------------------
void NoBeep () {
	TCCR1 = TCCR1 & ~3;											// Counter stopped
	digitalWrite(Pin_Led_Sense, LOW);
}


// ----------------------------------------------------------------------------------------
void setup () {
	pinMode(Pin_Reference,	INPUT_PULLUP);
	pinMode(Pin_Probe,		INPUT_PULLUP);
	pinMode(Pin_Led_Sense,	OUTPUT);
	pinMode(Pin_Led_Wake,	OUTPUT);
	pinMode(Pin_Speaker_A,	OUTPUT);
	pinMode(Pin_Speaker_B,	OUTPUT);
	
 	 // Setup beep
 	TCCR1 = 1 << CTC1 | 0 << COM1A0 | 0 << CS10;  			// CTC mode, counter stopped
	GTCCR = 1 << COM1B0;									// Toggle OC1B (PB4 and PB3 in PM Mode )
	GTCCR |= 1 << PWM1B;									// PWM Mode
	OCR1C = 238;											// Plays 1042Hz (C6)
	OCR1B = 119;											// at duty cycle 50

	
	//Pin-change interrupt
	PCMSK = 1 << Pin_Probe;									// Pin change interrupt on Pin_Probe
	GIMSK = GIMSK | 1 << PCIE;								// Enable pin-change interrupt
	
	// Power saving
	ADCSRA &= ~(1 << ADEN);									// Disable ADC to save power
	PRR = 1 << PRUSI | 1 << PRADC;							// Turn off unused clocks
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	
	// Start running
	//digitalWrite(Pin_Led_Wake, HIGH);
	Time = millis();
}

// ----------------------------------------------------------------------------------------
void loop() {
	bool Sense = ACSR>>ACO & 1;
	if (Sense) Beep(); else NoBeep();
	
	// Go to sleep?
	if (millis() - Time > Timeout) {
		digitalWrite(Pin_Led_Sense, LOW);
		digitalWrite(Pin_Led_Wake, 	LOW);
		pinMode(Pin_Reference, 		INPUT);								// Turn off pullup to save power
		sleep_enable();
		sleep_cpu();
		// Carry on here when we wake up
		pinMode(Pin_Reference, INPUT_PULLUP);
		digitalWrite(Pin_Led_Wake, 	HIGH);
	}
}


