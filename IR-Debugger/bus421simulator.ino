/* 
   bus421simulator for Arduino Uno (Atmega328=)
   
   this sketch sends a few bits simulating an omnibus every .5 seconds
   it needs timer0 8 bit and timer1 16 bit
   take note: delay() from the arduino libraries won't work, since it 
   also uses timer0.
   
   based on https://github.com/Taxilof/atmega_ortsbaken_simulator by Simon Fuchs
*/


#include <util/delay.h>

// Output: PB5 or "Digital pin 11 (PWM)"
#define OUTPUT_ENABLE (DDRB = (1<<PB1))
#define OUTPUT_DISABLE (DDRB = 0)
#define OUTPUT_TOGGLE (DDRB ^= (1<<PB1));PINB=0

// Enable/Disable the connection between the output PB5 and the internal PWM output
#define OUTPUT_PWM_TOGGLE  (TCCR1A ^=  (1<<COM1A1))
#define OUTPUT_PWM_DISABLE (TCCR1A &= ~(1<<COM1A1))
#define OUTPUT_PWM_ENABLE  (TCCR1A |=  (1<<COM1A1))

volatile boolean running;

// vars for handling bit-modulation
uint8_t first_half = 1;
uint8_t last_state = 0;
uint8_t buff_pointer = 0;
// teh data
uint8_t buff[255] = {
	1,1,1,1,1,1,        // 1-6: bitsync
	0,0,0,0,0,0,0,0,0,  // 7-15: rahmensync
	1,                  // 16: startbit
	1,0,1,0, 0,1,1,1,   // 24-17: byte 1
	1,                  // 25: sperrbit
	1,0,1,0, 0,1,1,1,   // 33-26: byte 2
	1,                  // 34: sperrbit
	1,0,1,0, 0,1,1,1,   // 42-35: byte 3
	1,                  // 43: sperrbit
	1,0,1,0, 0,1,1,1,   // 51-44: byte 4
	1,                  // 52: sperrbit   
	1,0,1,0, 0,1,1,1	// 53-60: CRC (this one is wrong!)
	};  


void setup() {
  // put your setup code here, to run once:
  // timer calc:   https://et06.dunweber.com/atmega_timers/
  
  // Timer 1 16 bit fuer grundmodulation mittels fast PWM 
  // ICR1 sets the TOP of the counter, OCR1A the position of high->low switch
  // 28.8khz => 556 ticks
  // 48khz   => 333 ticks
  ICR1 = 556; // TOP 
  OCR1A = 556 >> 2; // ein viertel fuer 25% duty cycle

  // Fast PWM mit TOP ICR1:  
  TCCR1A = (1<<WGM11) | (0<<WGM10);
  TCCR1B = (1<<WGM12) | (1<<WGM13);
  // Clear OC1A on compare match, set at BOTTOM => 25% high, 75% low
  TCCR1A |= (1<<COM1A1) | (0<<COM1A0);
  // Prescaler: 1 (works with 16mhz)
  TCCR1B |= (1<<CS10);



  // Timer 0 8 bit für bit-modulation
  // 4800bit/s => 9600Hz ISR Frequenz da immer am anfang bzw mitte eines bits umgetastet werden muss
  // CTC mit TOP OCRA und prescaler 8:
  TCCR0A = (1<<WGM01) | (0<<WGM00);
  TCCR0B = (1<<WGM02) | (0<<CS02) | (1<<CS01) | (0<<CS00);
  // we want 9600 Hz: 208 (makes 9615 Hz, close enough)
  OCR0A = 208;
  // enable compare match interrupt
  TIMSK0 |= (1<<OCIE0A);
  
  

  
  // aaaand start
  OUTPUT_ENABLE; // set PB5 as outout
  OUTPUT_PWM_DISABLE;  // we start with PWM disabled, as the ISR starts with a PWM_TOGGLE -> pwm enabled
  running = true;
  // enable global interrupts
  sei();  

}

// this modulates the bits
ISR(TIMER0_COMPA_vect) {
  if (first_half) {     	// we are in first half of one bit 
    OUTPUT_PWM_TOGGLE;		// first halfs always start with toggle
    first_half = 0; 	    // not in first half anymore
  } else { 					// we are in second half!! 
    if (buff[buff_pointer] == 0) {
      OUTPUT_PWM_TOGGLE;     // we got a zero => toggle in the middle of the bit
    }  // no else-case, b/c if we got a one, no transisiton is needed
    buff_pointer++;    // next bit is next :D
    first_half = 1;    // we are now in first half
  }
  
  
  // if we reached the end, start again
  if (buff_pointer == 61) {
    OUTPUT_PWM_DISABLE;
    OUTPUT_DISABLE;
    cli();
    buff_pointer = 0;

    _delay_ms(500);
    OUTPUT_ENABLE;
    sei();
  }
}


void loop() {

}
