#include "SevSeg.h"

#define MIN_POT 0
#define MAX_POT 1023
#define MIN_RANGE 0
#define MAX_RANGE 99

const int POTENTIOMETER = A0;
const int TILT_SENSOR = A2;
const int LED = A4;

const byte NUM_DIGITS = 4;
const byte DIGIT_PINS[] = {2, 3, 4, 5}; // 1, 2, 3, 4

                          // A, B,  C, D, E, F,  G, DP
const byte SEGMENT_PINS[] = {6, 7, 8, 9, 10, 11, 12, 13};

int displayValue = 3;

SevSeg display;

void setup()
{
  Serial.begin(38400);
  Serial.println("Hello World");

  pinMode(POTENTIOMETER, INPUT);
  pinMode(TILT_SENSOR, INPUT);
  digitalWrite(TILT_SENSOR, HIGH); // turn on the pull-up resistor

  pinMode(LED, OUTPUT);

  display.begin(COMMON_CATHODE, NUM_DIGITS, DIGIT_PINS, SEGMENT_PINS);
  display.setBrightness(90);

  randomSeed(digitalRead(0));
}

void loop()
{
  display.setNumber(displayValue, 2);
  display.refreshDisplay();

  int potentiometerValue = analogRead(POTENTIOMETER);
  Serial.print(potentiometerValue);
  Serial.print(": ");

  display.refreshDisplay();

  int mappedPotValue = map(potentiometerValue, MIN_POT, MAX_POT, MIN_RANGE, MAX_RANGE);
  Serial.print(mappedPotValue);
  Serial.print("; Tilt: ");

  display.refreshDisplay();

  boolean tiltValue = !digitalRead(TILT_SENSOR);
  Serial.print(tiltValue);

  display.refreshDisplay();

  Serial.print("; Gen #: ");
  Serial.println(displayValue / 100);

  display.refreshDisplay();

  if(tiltValue)
  {
    digitalWrite(LED, HIGH);
    generateNumber(mappedPotValue);
  }
  else
  {
    digitalWrite(LED, LOW);
  }

  display.refreshDisplay();

  displayValue -= (displayValue % 100);
  displayValue += mappedPotValue;

  display.refreshDisplay();
}

void generateNumber(int range)
{
  display.refreshDisplay();
  
  int randomNumber = random(range + 1);
  Serial.print("Generated Number: ");
  Serial.println(randomNumber);

  displayValue = randomNumber*100 + range;

  display.refreshDisplay();
}
