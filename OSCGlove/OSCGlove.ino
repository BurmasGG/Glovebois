#include <OSCBoards.h>
#include <OSCBundle.h>
#include <OSCData.h>
#include <OSCMatch.h>
#include <OSCMessage.h>
#include <OSCTiming.h>
#include <SLIPEncodedSerial.h>
#ifdef BOARD_HAS_USB_SERIAL
#include <SLIPEncodedUSBSerial.h>
SLIPEncodedUSBSerial SLIPSerial( thisBoardsSerialUSB );
#else
#include <SLIPEncodedSerial.h>
 SLIPEncodedSerial SLIPSerial(Serial);
#endif

const int index = A5;
const int middle = A4; 
const int thumb = A3;
const int flexThresh = 100;

float indexVal;
float middleVal;
float thumbVal;

const float VCC = 5;
const float R_DIV = 68000.0; //68k resistor
const float STRAIGHT_RESISTANCE_INDEX = 56000.0;
const float BEND_RESISTANCE_INDEX = 100000.0;
const float STRAIGHT_RESISTANCE_MIDDLE = 36000.0;
const float BEND_RESISTANCE_MIDDLE = 100000.0;
const float STRAIGHT_RESISTANCE_THUMB = 56000.0;
const float BEND_RESISTANCE_THUMB = 140000.0;
bool waveChange = false;

// constant not changing variables
const int buttonPin = 2;// the number of the pushbutton pin
const int drumPin = A0; 
const int threshold = 200;
int drumRead = 0; 
bool hit = false;
// changing variables
int buttonState = 0;    // variable for reading the pushbutton status
int waveform; // variable used for waveform selection
//boolean WaveBool = true; // boolean used to avoid 10 clicks being made at once with waveform select

void setup() {
  SLIPSerial.begin(9600);
  pinMode(buttonPin, INPUT); // initialize the pushbutton pin as an input:
}

void loop() {

int flexADC = analogRead(index);
  float flexV = flexADC * VCC / 1023.0;
  float flexR = R_DIV * (VCC / flexV - 1.0);
 // Serial.println("Resistanceindex: " + String(flexR) + " ohms");
  indexVal = analogRead(index);   // read the potentiometer value at the input pin

  float angle = map(flexR, STRAIGHT_RESISTANCE_INDEX, BEND_RESISTANCE_INDEX,
                    0, 90.0);
 //Serial.println("Bend: " + String(angle) + " degrees");
 //Serial.println();
// delay(1000);
 //threshold around50

 int flexADC2 = analogRead(middle);
 float flexV2 = flexADC2 * VCC / 1023.0;
 float flexR2 = R_DIV * (VCC / flexV2 - 1.0);
// Serial.println("Resistancemiddle: " + String(flexR2) + " ohms");
 middleVal = analogRead(middle);
 
  float angle2 = map(flexR2, STRAIGHT_RESISTANCE_MIDDLE, BEND_RESISTANCE_MIDDLE,
                    0, 90.0);
// Serial.println("Bend: " + String(angle2) + " degrees");
 // Serial.println();
//delay(1000);

  int flexADC3 = analogRead(thumb);
  float flexV3 = flexADC3 * VCC / 1023.0;
  float flexR3 = R_DIV * (VCC / flexV3 - 1.0);
 //Serial.println("Resistancethumb: " + String(flexR3) + " ohms");
   thumbVal = analogRead(thumb);
   
  float angle3 = map(flexR3, STRAIGHT_RESISTANCE_THUMB, BEND_RESISTANCE_THUMB,
                    0, 90.0);
// Serial.println("Bend: " + String(angle3) + " degrees");
//  Serial.println();
// delay(1000);

  drumRead = analogRead(drumPin); 

  if (drumRead < threshold)
  {
    drumRead = 0;
    hit = false;
  }

  else
  {
    drumRead = 1;
    hit = true;
        
  }


 /* OSCMessage msg2("/drum");
  msg2.add(drumRead);
  SLIPSerial.beginPacket();
  msg2.send(SLIPSerial);
  SLIPSerial.endPacket();
  msg2.empty();*/
  
    
 
  // This if statement cycles through the different waveforms
  // HIGH is pressed, LOW is not pressed
  if (angle3 > flexThresh && !waveChange) {
    delay(1000);
    waveChange = true;
    waveform++;
    waveform = waveform % 4;
    

  OSCMessage msg1("/waveformSelect");
  msg1.add(waveform);
  SLIPSerial.beginPacket();
  msg1.send(SLIPSerial);
  SLIPSerial.endPacket();
  msg1.empty();
  
 }

 else 
 {
  waveChange = false;
 }
// requires the button to be released before allowing a change to avoid 
// it changing 10 times on 1 click
//  if (buttonState == LOW) { 
  
}
