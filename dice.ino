#include "SevSeg.h"

#define MIN_POT 23
#define MAX_POT 1000
#define MIN_RANGE 1
#define MAX_RANGE 99

const int POTENTIOMETER = A0;
const int TILT_SENSOR = A2;
const int HOLD_SWITCH = A4;

const int LED = A5;


const byte NUM_DIGITS = 4;
const byte DIGIT_PINS[] = {2, 3, 4, 5}; // 1, 2, 3, 4

                          //  A,  B, C, D, E,  F,  G, DP
const byte SEGMENT_PINS[] = {13, 11, 9, 7, 6, 12, 10, 8};

int displayValue = 3;

SevSeg display;

void setup()
{
  cli();  // Stop interrupts
  // Set timer1 interrupt at 1kHz
  TCCR1A = 0; // Set entire TCCR1A register to 0
  TCCR1B = 0; // Same for TCCR1B
  TCNT1  = 0; // Initialize counter value to 0
  // Set timer count for 100Hz increments
  OCR1A = 2002; // = (16*10^5) / (100*8) - 1
  // Had to use 16 bit timer1 for this because 1999>255, 
  // but could switch to timers 0 or 2 with larger prescaler
  // Turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  TCCR1B |= (1 << CS11);  
  // Enable timer compare interrupt
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
  // Decimal point in the dead center, pretend it's "/"
  display.setNumber(displayValue, 2); 

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

    // Turn Right for higher numbers
    // Limit from 1 to 99
    int mappedPotValue = MAX_RANGE + MIN_RANGE 
                       - constrain(map(potentiometerValue, MIN_POT, MAX_POT, 
                                       MIN_RANGE, MAX_RANGE), 
                                   MIN_RANGE, MAX_RANGE); 
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

ISR(TIMER1_COMPA_vect) // Refresh display @ 100Hz
{
  display.refreshDisplay();
}
