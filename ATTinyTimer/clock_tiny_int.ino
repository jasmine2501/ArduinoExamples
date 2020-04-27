  /**********************************/
/* clock_tiny_int
    created: 2019-10-05
    by: jasmine2501

    Uses the timers to generate a fast clock signal for retro-computer
    Uses interrupts to handle standby (single step) mode
*/

#include "avr/interrupt.h"

#define CLK_PIN 0
#define MODE_PIN 1
#define STEP_PIN 2
#define ADJ_PIN 3

static volatile bool RUN_MODE = true; //note: stepping mode = false
static volatile long pot_value = 0;

void setup() {
  // set all the pinMode values
  pinMode (CLK_PIN, OUTPUT);
  pinMode (MODE_PIN, INPUT);
  pinMode (STEP_PIN, INPUT);
  pinMode (ADJ_PIN, INPUT);

  //always set up timer1
  setup_timer1();

  if (!digitalRead(MODE_PIN)) {
    start_timer0();
    detachStepButton();
  }
  else {
    stop_timer0();
    attachStepButton();
  }
}

void loop () {
}

void setup_timer1() {
  //timer1 is used to periodically read the analog input speed adjuster and check the mode
  //timer1 flags (TCCR1) -> CTC1 PWM1A COM1A1 COM1A0 CS13 CS12 CS11 CS10
  TCCR1 = 0b10001111; //16384 scaling, and CTC1 (compare timer interrupt vector)
  OCR1C = 255; //longest possible wait time

  //Timer mask register flags ->  --- OCIE1A OCIE1B OCIE0A OCIE0B TOIE1 TOIE0 ---
  TIMSK = 0b01000000;
}

void start_timer0() {
  //timer0 toggles the clock signal (auto, no interrupt)
  TCCR0A = 0b01000011; //timer0 flags (TCCR0A) -> COM0A1  COM0A0  COM0B1  COM0B0  --– --– WGM01 WGM00

  //note: scaling on CS02,CS01,CS00(000=STOPPED,001=none,010=8,011=64,100=256,101=1024,110/111=EXT)
  TCCR0B = 0b00001001; //timer0 flags (TCCR0B) -> FOC0A FOC0B –-- --– WGM02 CS02  CS01  CS00 
  
  pot_value = analogRead(ADJ_PIN);
  OCR0A = (pot_value / 5) + 1; // initial compare match value

  //Timer mask register flags ->  --- OCIE1A OCIE1B OCIE0A OCIE0B TOIE1 TOIE0 ---
  TIMSK = 0b01010000; 
}

void stop_timer0() {
  TCCR0A = 0b00000000; //timer0 flags (TCCR0A) -> COM0A1  COM0A0  COM0B1  COM0B0  --– --– WGM01 WGM00

  //note: scaling on CS02,CS01,CS00(000=STOPPED,001=none,010=8,011=64,100=256,101=1024,110/111=EXT)
  TCCR0B = 0b00000000; //timer0 flags (TCCR0B) -> FOC0A FOC0B –-- --– WGM02 CS02  CS01  CS00
 
  //just disable timer0 - leave timer1 running!
  TIMSK = 0b01000000;
  digitalWrite(CLK_PIN, LOW);
}

/*ISR(TIMER1_COMPA_vect)
{
  if (RUN_MODE) {
    //update the timer compare value
    pot_value = analogRead(ADJ_PIN);
    OCR0A = pot_value / 2;
  }
  
  //no need to debounce because this doesn't run often
  if (digitalRead(MODE_PIN) == RUN_MODE) {
    //need to switch modes
    RUN_MODE = !RUN_MODE;
    if (RUN_MODE) {
      start_timer0();
      detachStepButton();
    }
    else {
      stop_timer0();
      attachStepButton();
    }
  }
}
*/
void attachStepButton() {
  MCUCR = 0b00000001; // [1:0] = "01" - ANY logical change
  PCMSK = 0b00000100; //– – PCINT5 PCINT4 PCINT3 PCINT2 PCINT1 PCINT0
  GIMSK = 0b00100000; //enable pin change interrupt
}

void detachStepButton() {
  GIMSK = 0b00000000;
}

ISR(PCINT0_vect) {
  digitalWrite(CLK_PIN, !digitalRead(STEP_PIN));
}
