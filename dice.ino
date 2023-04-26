#include "SevSeg.h"

#define MIN_POT 23
#define MAX_POT 1000
#define MIN_RANGE 1
#define MAX_RANGE 99

const int POTENTIOMETER = A0;
const int TILT_SENSOR = A2;
const int LED = A4;
const int HOLD_SWITCH = A5;

const byte NUM_DIGITS = 4;
const byte DIGIT_PINS[] = {2, 3, 4, 5}; // 1, 2, 3, 4

                          // A, B, C, D,  E,  F,  G, DP
const byte SEGMENT_PINS[] = {6, 7, 8, 9, 10, 11, 12, 13};

int displayValue = 3;

SevSeg display;

void setup()
{
  cli();  // Stop interrupts
  // Set timer1 interrupt at 1kHz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set timer count for 100Hz increments
  OCR1A = 2002;// = (16*10^5) / (100*8) - 1
  //had to use 16 bit timer1 for this bc 1999>255, but could switch to timers 0 or 2 with larger prescaler
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  TCCR1B |= (1 << CS11);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();  //allow interrupts
  
  Serial.begin(38400);
  Serial.println("Hello World");

  pinMode(POTENTIOMETER, INPUT);
  pinMode(TILT_SENSOR, INPUT);
  pinMode(TILT_SENSOR, INPUT_PULLUP);
  pinMode(HOLD_SWITCH, INPUT);
  pinMode(HOLD_SWITCH, INPUT_PULLUP);

  pinMode(LED, OUTPUT);

  display.begin(COMMON_CATHODE, NUM_DIGITS, DIGIT_PINS, SEGMENT_PINS);
  display.setBrightness(90);

  randomSeed(digitalRead(0));

  display.setNumber(displayValue, 2);
  display.refreshDisplay();
}

void loop()
{
  display.setNumber(displayValue, 2); // Decimal point in the dead center, pretend it's "/"

  boolean isHold = !digitalRead(HOLD_SWITCH);
  Serial.print("Holding: ");
  if(isHold)
  {
    Serial.println("YES; ");
  }
  else
  {
    Serial.print("NO; ");
  }
  
  if(!isHold)
  {
    int potentiometerValue = analogRead(POTENTIOMETER);
    Serial.print("Pot value: ");
    Serial.print(potentiometerValue);
    Serial.print(": ");
    
    int mappedPotValue = MAX_RANGE + MIN_RANGE - constrain(map(potentiometerValue, MIN_POT, MAX_POT, MIN_RANGE, MAX_RANGE), MIN_RANGE, MAX_RANGE); // Limit from 1 to 99; turn Right for higher numbers
    Serial.print(mappedPotValue);
    Serial.print("; Tilt: ");
  
    boolean tiltValue = !(digitalRead(TILT_SENSOR));
    Serial.print(tiltValue);
  
    Serial.print("; Gen #: ");
    Serial.println(displayValue / 100);
  
    if(tiltValue)
    {
      digitalWrite(LED, HIGH);
      generateNumber(mappedPotValue);
    }
    else
    {
      digitalWrite(LED, LOW);
    }
  
    displayValue -= (displayValue % 100);
    displayValue += mappedPotValue;
  }

  delay(100); // Run this loop at 10 Hz
}

void generateNumber(int range)
{ 
  int randomNumber = random(range + 1);
//  Serial.print("Generated Number: ");
//  Serial.println(randomNumber);

  displayValue = (randomNumber*100) + range;
}

ISR(TIMER1_COMPA_vect) // refresh display @ 100Hz
{
  display.refreshDisplay();
}
