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
const int flexThresh = 100;
const int drumThresh = 200;


////////////////////////////////////////////
////////VALUES - POWER & RESTISTANCE////////
////////////////////////////////////////////
const float VCC = 5;
const float R_DIV = 68000.0; //68k resistor used
const float STRAIGHT_RESISTANCE_INDEX = 21000.0;
const float BEND_RESISTANCE_INDEX = 50000.0;
const float STRAIGHT_RESISTANCE_MIDDLE = 21000.0;
const float BEND_RESISTANCE_MIDDLE = 50000.0;
const float STRAIGHT_RESISTANCE_THUMB = 56000.0;
const float BEND_RESISTANCE_THUMB = 140000.0;


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
  

////////////////////////////////////////////
///////////BOOLEANS FOR ON n OFF////////////
////////////////////////////////////////////
bool waveChange = false; //Boolean used to ensure sensor must be flexed "off" before it can be flexed "on" again.
bool hit = false;
int deltat =0;
////////////////////////////////////////////
///////////FILTER VARIABLES/////////////////
////////////////////////////////////////////
float lopValue = 0;
float hipValue = 0;
bool lopSwitch = false;
bool hipSwitch = false;

void setup() 
{
  SLIPSerial.begin(9600);
}

void loop() 
{
  
//drum();
//index();
middle();
//thumb();

}
