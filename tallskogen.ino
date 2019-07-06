/*
 Tallskogen Rally

 This is a Pinewood Derby timer circuit proof-of-concept with one lane using
 The Arduino Starter Kit.

 The LCD display circuit: <https://www.arduino.cc/en/Tutorial/HelloWorld>
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
 *   ends to +5V and ground
 *   wiper to LCD VO pin (pin 3)
 * 220 ohm resistor to 5V
 * LCD LED+ to 220 ohm resistor
 * LCD LED- to ground


 The piezo electric element circuit: <https://github.com/bhagman/Tone#warning>
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

The phototransistor circuits: (up to six of these using analog input 0 to 5)
  * phototransistor collector to 5V
  * phototransistor emitter to analog 0  (from 0 to 5)
  * Resistor 2k Ohm to emitter
  * Resistor 2k Ohm to ground

The servo circuit:
  * Servo + to 5V
  * Servo pulse to digital 13
  * Servo - to ground

This code is in the public domain and heavily based on Arduino example code also in the public domain.

*/


//number of lanes (same as the number of phototransistor circuits)
// CHANGE THIS TO THE NUMBER OF LANES IN YOUR TRACK
int lanes = 6;

// How long will the race run until timed out.
// CHANGE THIS TO HIGH OR LOW VALUE DEPENDING ON TRACK
int timeOut = 10000;

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

// this is the phototransistor analog pin

int analogValArray[6];
int analogMaxArray[6];

int analogPin = A0;
int analogVal = 0;
int analogMax = 0;

// we have no final time yet
int finalTime = 0;
int finalTimeArray[6];

int finishedLanes = 0;

int oldTime = 0;
int raceOver = 0;
  
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
  int i;
  
  // clear array
  for (i=0; i<lanes; i++) {
    finalTimeArray[i] = 0;
  }
  
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
  lcd.print("Tallskogen Derby");
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



// Print Time
void printPos(int i) {
  if (i == 0) {lcd.setCursor(0,0);}
  if (i == 2) {lcd.setCursor(6,0);}
  if (i == 4) {lcd.setCursor(11,0);}
  if (i == 1) {lcd.setCursor(0,1);}
  if (i == 3) {lcd.setCursor(6,1);}
  if (i == 5) {lcd.setCursor(11,1);}
}
void printTime(int i, int v) {
  printPos(i);
  lcd.print(v);
}

// the main loop
void loop() {
  int eT;
  int lastUpdate;
  int i;

  // calculate elapsed time
  eT=millis()-start;
  
  // read the phototransistor
  for (i=0; i<lanes; i++) {
    analogValArray[i] = analogRead(i);
    if (analogValArray[i] > analogMaxArray[i]) {
      analogMaxArray[i] = analogValArray[i];
    }
    
    if (analogValArray[i] < analogMaxArray[i]/2) {
      tone(piezoPin, freq+100*i, 20);
      analogMaxArray[i] = 0;
      if (finalTimeArray[i] == 0 && raceOver == 0) {
        finalTimeArray[i] = eT;
        printTime(i, finalTimeArray[i]);
        finishedLanes++;
      }
    }
    // print the rolling time if race is not over
    if (finalTimeArray[i] == 0 && raceOver == 0) {
      printTime(i, eT);
    }
  }

  // 
  if (eT >timeOut && raceOver == 0) {
    raceOver = 1;
    tone(piezoPin, freqOver, 600);
  }
    
  if (finishedLanes==lanes || raceOver == 1) {
    lcd.setCursor(0, 0);
    //lcd.clear();
    //lcd.print("Tallskogen Rally");
    //lcd.setCursor(0, 1);
    //lcd.print("Race over");
    servoGate.write(0);
    if (millis() > oldTime+500) {
      oldTime=millis();
      int roll;
      roll++;   
      for (i=0; i<lanes; i++) {
        if (finalTimeArray[i] == 0) {
          printPos(i);
          if (roll % 6 == 0) {lcd.print(" ^_^ ");}
          if (roll % 6 == 1) {lcd.print(" -_^ ");}
          if (roll % 6 == 2) {lcd.print(" ^_^ ");}
          if (roll % 6 == 3) {lcd.print(" ^_^ ");}
          if (roll % 6 == 4) {lcd.print(" ^_- ");}
          if (roll % 6 == 5) {lcd.print(" ^_^ ");}

        }
      }
    }
  }
  delay(0);
}
