////////////////////////////////////////////
///////////////////INCLUDES/////////////////
////////////////////////////////////////////
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


////////////////////////////////////////////
////////ASSIGNING COMPONENTS TO PINS////////
////////////////////////////////////////////
const int indexPin = A5;
const int middlePin = A4;
const int thumbPin = A3;
const int drumPin = A0;


////////////////////////////////////////////
///////////////THRESHOLDS///////////////////
////////////////////////////////////////////
const int indexThresh = 90;
const int drumThresh = 60;
const int thumbThresh = 90;
const int middleThresh = 90; 

////////////////////////////////////////////
////////VALUES - POWER & RESTISTANCE////////
////////////////////////////////////////////
const float VCC = 5;
const float R_DIV = 68000.0; //68k resistor used
const float STRAIGHT_RESISTANCE_INDEX = 52000.0;
const float BEND_RESISTANCE_INDEX = 110000.0;
const float STRAIGHT_RESISTANCE_MIDDLE = 75000.0;
const float BEND_RESISTANCE_MIDDLE = 140000.0;
const float STRAIGHT_RESISTANCE_THUMB = 70000.0;
const float BEND_RESISTANCE_THUMB = 120000.0;


////////////////////////////////////////////
/////////////VARIABLE READINGS//////////////
////////////////////////////////////////////
float indexVal;
float middleVal;
float thumbVal;
int drumRead;
int waveform;
int prevTime = 0;
  float angleIndex;
  float angleMiddle;
  float angleThumb;
 float waveSend;

////////////////////////////////////////////
///////////BOOLEANS FOR ON n OFF////////////
////////////////////////////////////////////
bool waveChange = false; //Boolean used to ensure sensor must be flexed "off" before it can be flexed "on" again.
bool hit = false;
int deltat =0;
int prevTimeM = 0;
int prevTimeI = 0;
int prevTimeT = 0;
////////////////////////////////////////////
///////////FILTER VARIABLES/////////////////
////////////////////////////////////////////
float lopValue = 0;
float hipValue = 0;
bool lopSwitch = false;
bool hipSwitch = false;

void setup() 
{
  SLIPSerial.begin(9200);
}

void loop() 
{
  
drum();
index();
middle();
thumb();

}
