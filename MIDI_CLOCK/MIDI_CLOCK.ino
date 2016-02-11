#include <MIDI.h>

#define START_PIN 10
#define CLOCK_PIN 9
#define CONTINUE_PIN 11
#define STOP_PIN 12


#define DIVISOR_PIN A0
int divisor = 1;
int clockCount = 0;

int startTriggerRemaining = 0, clockTriggerRemaining = 0, continueTriggerRemaining = 0, stopTriggerRemaining = 0;
#define TRIGGER_PULSE_MS 10

MIDI_CREATE_DEFAULT_INSTANCE();


void handleStart() {
  digitalWrite(START_PIN, HIGH);
  startTriggerRemaining = TRIGGER_PULSE_MS;
  clockCount = 0;
}

void handleClock() {
  clockCount++;
  if (clockCount == divisor) {
    digitalWrite(CLOCK_PIN, HIGH);
    clockTriggerRemaining = TRIGGER_PULSE_MS;
    clockCount = 0;
  }
}

void handleContinue() {
  digitalWrite(CONTINUE_PIN, HIGH);
  continueTriggerRemaining = TRIGGER_PULSE_MS;
}

void handleStop() {
  digitalWrite(STOP_PIN, HIGH);
  stopTriggerRemaining = TRIGGER_PULSE_MS;
}



void setup()
{
    
    pinMode(START_PIN, OUTPUT);
    digitalWrite(START_PIN, LOW);
    pinMode(CLOCK_PIN, OUTPUT);
    digitalWrite(CLOCK_PIN, LOW);
    pinMode(CONTINUE_PIN, OUTPUT);
    digitalWrite(CONTINUE_PIN, LOW);
    pinMode(STOP_PIN, OUTPUT);
    digitalWrite(STOP_PIN, LOW);

    MIDI.setHandleStart(handleStart);
    MIDI.setHandleClock(handleClock);
    MIDI.setHandleContinue(handleContinue);
    MIDI.setHandleStop(handleStop);


    cli();//stop interrupts
    
    //set timer1 interrupt at 1kHz
    TCCR1A = 0;// set entire TCCR1A register to 0
    TCCR1B = 0;// same for TCCR1B
    TCNT1  = 0;//initialize counter value to 0;
    // set timer count for 1khz increments
    OCR1A = 1999;// = (16*10^6) / (1000*8) - 1
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS11 bit for 8 prescaler
    TCCR1B |= (1 << CS11);   
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    
    sei();//allow interrupts

    
    MIDI.begin();
}



ISR(TIMER1_COMPA_vect) {

  if (startTriggerRemaining > 0) {
    startTriggerRemaining--;
    if (startTriggerRemaining == 0) {
      digitalWrite(START_PIN, LOW);
    }
  }

  if (clockTriggerRemaining > 0) {
    clockTriggerRemaining--;
    if (clockTriggerRemaining == 0) {
      digitalWrite(CLOCK_PIN, LOW);
    }
  }

  if (continueTriggerRemaining > 0) {
    continueTriggerRemaining--;
    if (continueTriggerRemaining == 0) {
      digitalWrite(CONTINUE_PIN, LOW);
    }
  }

  if (stopTriggerRemaining > 0) {
    stopTriggerRemaining--;
    if (stopTriggerRemaining == 0) {
      digitalWrite(STOP_PIN, LOW);
    }
  }
  
}


int divisorRange = 1024 / 96;



void loop()
{
//  divisor = 1 + analogRead(DIVISOR_PIN) / divisorRange;

  MIDI.read();
}

