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

//assigning components to pins.
const int index = A5;
const int middle = A4;
const int thumb = A3;
const int drumPin = A0;
const int flexThresh = 100;


float indexVal;
float middleVal;
float thumbVal;

//Values for the power, and resistance for the different sensors in different states.
const float VCC = 5;
const float R_DIV = 68000.0; //68k resistor used
const float STRAIGHT_RESISTANCE_INDEX = 56000.0;
const float BEND_RESISTANCE_INDEX = 100000.0;
const float STRAIGHT_RESISTANCE_MIDDLE = 36000.0;
const float BEND_RESISTANCE_MIDDLE = 100000.0;
const float STRAIGHT_RESISTANCE_THUMB = 56000.0;
const float BEND_RESISTANCE_THUMB = 140000.0;
bool waveChange = false; //Boolean used to ensure sensor must be flexed "off" before it can be flexed "on" again.

// constant not changing variables

//Threshold used to read when the drum is hit
const int threshold = 200;
int drumRead = 0;
bool hit = false;

void setup() 
{
  SLIPSerial.begin(9600);
}

void loop() 
{

  int flexADC = analogRead(index);
  float flexV = flexADC * VCC / 1023.0;
  float flexR = R_DIV * (VCC / flexV - 1.0);
  indexVal = analogRead(index);   // read the potentiometer value at the input pin
  float angle = map(flexR, STRAIGHT_RESISTANCE_INDEX, BEND_RESISTANCE_INDEX,0, 90.0);


  int flexADC2 = analogRead(middle);
  float flexV2 = flexADC2 * VCC / 1023.0;
  float flexR2 = R_DIV * (VCC / flexV2 - 1.0);
  middleVal = analogRead(middle);
  float angle2 = map(flexR2, STRAIGHT_RESISTANCE_MIDDLE, BEND_RESISTANCE_MIDDLE,0, 90.0);


  int flexADC3 = analogRead(thumb);
  float flexV3 = flexADC3 * VCC / 1023.0;
  float flexR3 = R_DIV * (VCC / flexV3 - 1.0);
  thumbVal = analogRead(thumb);
  float angle3 = map(flexR3, STRAIGHT_RESISTANCE_THUMB, BEND_RESISTANCE_THUMB,0, 90.0);


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


//if statement used to determine if the flex sensor is bend to the desired state to be "on"

if (angle>flexTresh) 
{

    lowpass = lowpass % 4; //Used to make sure the number send to puredata is either 1,2,3 or 4. 

    //Open sound control message that sends the "waveform" value to pure data.
    OSCMessage msg1("/lowpassSelect");
    msg1.add(lowpass);
    SLIPSerial.beginPacket();
    msg1.send(SLIPSerial);
    SLIPSerial.endPacket();
    msg1.empty();
}

if (angle2 > flexTresh)
{
   
    highpass = highpass % 4; //Used to make sure the number send to puredata is either 1,2,3 or 4. 

    //Open sound control message that sends the "waveform" value to pure data.
    OSCMessage msg1("/highpassSelect");
    msg1.add(highpassSelect
    );
    SLIPSerial.beginPacket();
    msg1.send(SLIPSerial);
    SLIPSerial.endPacket();
    msg1.empty();
}
  if (angle3 > flexThresh && !waveChange) {
    delay(1000);
    waveChange = true;
    waveform++;
    waveform = waveform % 4; //Used to make sure the number send to puredata is either 1,2,3 or 4. 

    //Open sound control message that sends the "waveform" value to pure data.
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
  

}
