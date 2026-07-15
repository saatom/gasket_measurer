#include <cmath>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include <RotaryEncoder.h>

#define disp1 0
#define A 1
#define F 2
#define disp2 3
#define disp3 4
#define B 5

#define disp4 6
#define G 7
#define C 8
#define DP 9
#define D 10
#define E 11

#define rstPin 20
#define unitPin 21

#define numbersegments { \
  {1,1,1,1,1,1,0,0},\
  {0,1,1,0,0,0,0,0},\
  {1,1,0,1,1,0,1,0},\
  {1,1,1,1,0,0,1,0},\
  {0,1,1,0,0,1,1,0},\
  {1,0,1,1,0,1,1,0},\
  {1,0,1,1,1,1,1,0},\
  {1,1,1,0,0,0,0,0},\
  {1,1,1,1,1,1,1,0},\
  {1,1,1,0,0,1,1,0},\
  {0,0,0,0,0,0,1,0},\
}

byte numbers[11][8] = numbersegments;
const int segments[8] = {A, B, C, D, E, F, G, DP};
volatile float gasketLength = 0;
volatile int rawLength = 0;
static byte abOld;

bool fordeb = 0;
bool backdeb = 0;
bool units = 0; // 0 = meters, 1 = feet

#define forpin 16
#define backpin 17

//RotaryEncoder encoder(forpin, backpin, RotaryEncoder::LatchMode::TWO03);

void setup() {
  Serial.begin(115200);

  pinMode(rstPin, INPUT_PULLUP);
  pinMode(unitPin, INPUT_PULLUP);

  pinMode(forpin, INPUT_PULLUP); // Handling encoder stuff
  pinMode(backpin, INPUT_PULLUP);
  attachInterrupt(forpin, pinChangeISR, CHANGE);
  attachInterrupt(backpin, pinChangeISR, CHANGE);
  abOld = rawLength;

  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(E, OUTPUT);
  pinMode(F, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(DP, OUTPUT);
  pinMode(disp1, OUTPUT);
  pinMode(disp2, OUTPUT);  
  pinMode(disp3, OUTPUT);
  pinMode(disp4, OUTPUT);
  digitalWrite(A, LOW);
  digitalWrite(B, LOW);
  digitalWrite(C, LOW);
  digitalWrite(D, LOW);
  digitalWrite(E, LOW);
  digitalWrite(F, LOW);
  digitalWrite(G, LOW);
  digitalWrite(DP, LOW);
  digitalWrite(disp1, HIGH);
  digitalWrite(disp2, HIGH);
  digitalWrite(disp3, HIGH);
  digitalWrite(disp4, HIGH);
}

void pinChangeISR() {
  enum { upMask = 0x66, downMask = 0x99 };
  byte abNew = (digitalRead(forpin) << 1) | digitalRead(backpin);
  byte criterion = abNew^abOld;
  if (criterion==1 || criterion==2) {
    if (upMask & (1 << (2*abOld + abNew/2)))
      rawLength--;
    else rawLength++;       // upMask = ~downMask
  }
  abOld = abNew;        // Save new state
}

void setnumber(){
  float number;
  bool isNeg = (gasketLength < 0);
  if (units == 0){
    number = abs(gasketLength);
  }
  else{
    number = abs(gasketLength*1000/25.4/12);
  }
  uint8_t dig1 = floor(number/100.0);
  uint8_t dig2 = floor((number-dig1*100)/10.0);
  uint8_t dig3 = floor((number-dig1*100-dig2*10)/1.0);
  uint8_t dig4 = floor((number-dig1*100-dig2*10-dig3)/0.1);
  if(number>=100){
    setsegments(dig1, disp1, 0); // Only set first digit if number is greater than 99
  }
  else if(number >= 10 && isNeg){
    setsegments(10, disp1, 0);
  }
  if(number>=10){
    setsegments(dig2, disp2, 0);
  }
  else if(isNeg){
    setsegments(10, disp2, 0);
  }
  setsegments(dig3, disp3, 1);
  setsegments(dig4, disp4, 0);
}

void loop1(){
  setnumber();
}

int lastVal = 0;
bool endeb = 0;
int lastPos = 0;
int aState;
int aLastState;

void loop() {
  /*
     encoder.tick();
     int enpos = encoder.getPosition();
     if(enpos != lastPos){
     float incr = (enpos - lastPos);
  // gasketLength = gasketLength + incr;
  rawLength = rawLength + incr;
  gasketLength = static_cast<float>(rawLength) * 40*3.14159/20.0/1000;
  lastPos = enpos;
  }
  */
  gasketLength = static_cast<float>(rawLength)/2.0 * 40*3.14159/20.0/1000;

  if (digitalRead(rstPin) == LOW) {
    rawLength = 0;
    gasketLength = 0;
  }
  units = !digitalRead(unitPin);
}

void setsegments(int number, int digit, int dec){ 
  int ontime = 5000;
  for (int seg=0; seg<8; seg++)
  { 
    if(numbers[number][seg]==1)
    { 
      digitalWrite(segments[seg], HIGH);
    }
    else 
    {
      digitalWrite(segments[seg], LOW);
    }
  }
  if(dec){
    digitalWrite(DP, HIGH);
  }
  else{
    digitalWrite(DP, LOW);
  }
  digitalWrite(digit, LOW);
  delayMicroseconds(ontime);
  digitalWrite(digit, HIGH);
}
