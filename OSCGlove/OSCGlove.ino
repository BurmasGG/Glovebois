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

// constant not changing variables
const int buttonPin = 2;// the number of the pushbutton pin
const int drumPin = A0; 
const int threshold = 200;
int drumRead = 0; 
bool hit = false;
// changing variables
int buttonState = 0;    // variable for reading the pushbutton status
int waveform; // variable used for waveform selection
boolean WaveBool = true; // boolean used to avoid 10 clicks being made at once with waveform select

void setup() {
  SLIPSerial.begin(9600);
  pinMode(buttonPin, INPUT); // initialize the pushbutton pin as an input:
}

void loop() {

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


  OSCMessage msg2("/drum");
  msg2.add(drumRead);
  SLIPSerial.beginPacket();
  msg2.send(SLIPSerial);
  SLIPSerial.endPacket();
  msg2.empty();

 
 }
 
  
 
  
  // put your main code here, to run repeatedly:
  buttonState = digitalRead(buttonPin);

  // This if statement cycles through the different waveforms
  // HIGH is pressed, LOW is not pressed
  if (buttonState == HIGH && WaveBool) {
    waveform++;
    waveform = waveform % 4;
    WaveBool = false;

  OSCMessage msg1("/waveformSelect");
  msg1.add(waveform);
  SLIPSerial.beginPacket();
  msg1.send(SLIPSerial);
  SLIPSerial.endPacket();
  msg1.empty();
  
 }
  
// requires the button to be released before allowing a change to avoid 
// it changing 10 times on 1 click
  if (buttonState == LOW) { 
    WaveBool = true;
  }
}
