/*
 Tallskogen Rally

 This is a Pinewood Derby timer circuit proof-of-concept with one lane using
 The Arduino Starter Kit.

  The circuit LCD display:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)


 The piezo electric element circuit:
  * Piezo to ground
  * Resistor 1k Ohm to Piezo 
  * Resistor 1k Ohm to digital pin 10

 The Red / Yellow / Green LED circuit:
  * Resistor ~220 Ohm to ground
  * Red LED anode to digital 6
  * Red LED cathode to resistor
  * Yellow LED anode to digital 7
  * Yellow LED cathode to resistor  
  * Green LED anode to digital 8
  * Green LED cathode to resistor 

The phototransistor circuit:
  * phototransistor collector to 5V
  * phototransistor emitter to analog 0
  * Resistor 2k Ohm to emitter
  * Resistor 2k Ohm to ground

The servo circuit:
  * Servo + to 5V
  * Servo pulse to digital 13
  * Servo - to ground

This code is in the public domain and heavily based on Arduino example code also in the public domain.

*/

// include the library code:
#include <LiquidCrystal.h>

// include the servo code:
#include <Servo.h>

Servo servoGate;  // create servo object to control a servo

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int start;
char message[10];

// this is the phototransistor analog pin
int analogPin = 0;
int analogVal = 0;
int analogMax = 0;

// we have no final time yet
int finalTime = 0;
// this is the pizeo element digital pin
int piezoPin = 10;

// these are the LED pins
int redPin = 6;
int yellowPin = 7;
int greenPin = 8;

// beep frequency
int freq = 1000;
int freqOver = 100;

void setup() {
  
  servoGate.attach(13);  // attaches the servo on pin 9 to the servo object
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // setup the digital output pins
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(redPin, OUTPUT);

  // turn off the lights
  digitalWrite(greenPin, LOW); 
  digitalWrite(yellowPin, LOW); 
  digitalWrite(redPin, LOW); 

  // initiate logo
  lcd.setCursor(0,0);
  lcd.print("Tallskogen Rally");
  delay(500);
  servoGate.write(0); 

  // prepare the race with red light
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("READY");
  digitalWrite(redPin, HIGH); 
  tone(piezoPin, freq, 25);
  delay(100);
  tone(piezoPin, freq, 25);
  delay(850);

  // fire up the yellow light
  lcd.setCursor(0, 0);
  lcd.print("SET  ");
  digitalWrite(yellowPin, HIGH); 
  tone(piezoPin, freq, 25);
  delay(100);
  tone(piezoPin, freq, 25);
  delay(850);

  // and start the race with green light
  lcd.setCursor(0, 0);
  lcd.print("GO   ");
  digitalWrite(redPin, LOW); 
  digitalWrite(yellowPin, LOW); 
  digitalWrite(greenPin, HIGH); 
  tone(piezoPin, freq, 300);
  delay(700);
  digitalWrite(greenPin, LOW); 

  // begin counting milliseconds
  start=millis();
  lcd.clear();
  servoGate.write(130); 

}

// Quad Print Time
//   int value - value to be printed
//   side-effect: display the value four times on display

void qpt(int value) { //quad print time
  char c[10];
  int m;
  
  m = sprintf (c, "%8d", value);

  lcd.setCursor(0, 0);
  lcd.print(c);
  lcd.setCursor(8, 0);
  lcd.print(c);
  lcd.setCursor(0, 1);
  lcd.print(c);
  lcd.setCursor(8, 1);
  lcd.print(c);
}

// the main loop
void loop() {
  int eT;
  int lastUpdate;
  
  // calculate elapsed time
  eT=millis()-start;
  
  // read the phototransistor
  analogVal=analogRead(analogPin);

  if (analogVal > analogMax) {
    // light is increasing
    analogMax = analogVal;
  }
  if (analogVal < analogMax/2) {
    // shadow detected above phototransistor
    analogMax = 0;
    finalTime = eT;
    tone(piezoPin, freq, 20);
    qpt (finalTime);
  }
  if (finalTime == 0 && eT <30000) {
      qpt (eT);
  }
  if (eT >30000) {
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Tallskogen Rally");
    lcd.setCursor(0, 1);
    lcd.print("Race over");
    tone(piezoPin, freqOver, 600);
    servoGate.write(0);
    while(1);
  }
  
  delay(0);
}
