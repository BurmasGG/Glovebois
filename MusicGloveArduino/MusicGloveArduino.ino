// Defining includes.
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

// Initialising the MPU with the required address (0x69)
MPU6050 mpu(0x69);

#define LED_PIN 13
bool blinkState = false;

bool volumeChange = false;
const int indexPin = A2;
const int middlePin = A1;
const int thumbPin = A3;
// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

////////////////////////////////////////////
///////////////THRESHOLDS///////////////////
////////////////////////////////////////////
const int indexThresh = 300;
const int thumbThresh = 120;
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
int waveform;
float angleIndex;
float angleMiddle;
float angleThumb;
float waveSend;
////////////////////////////////////////////
///////////BOOLEANS FOR ON n OFF////////////
////////////////////////////////////////////
bool waveChange = false; //Boolean used to ensure sensor must be flexed "off" before it can be flexed "on" again.
bool hit = false;
int deltat = 0;
int prevTime = 0;
////////////////////////////////////////////
///////////FILTER VARIABLES/////////////////
////////////////////////////////////////////
uint8_t lopValue = 0;
uint8_t hipValue = 0;
bool lopSwitch = false;
bool hipSwitch = false;

// packet structure for InvenSense teapot demo
uint8_t dataPacket[14] = { '$', 0x02, 0, 0, 0, 0, 0, 0, 0, 0, 0x00, 0x00, '\r', '\n' };

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}

void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
  Wire.begin();

  Serial.begin(115200);

  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
    // turn on the DMP, now that it's ready
    Serial.println(F("Enabling DMP..."));
    mpu.setDMPEnabled(true);

    // enable Arduino interrupt detection
    Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
    attachInterrupt(0, dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();

    // set our DMP Ready flag so the main loop() function knows it's okay to use it
    Serial.println(F("DMP ready! Waiting for first interrupt..."));
    dmpReady = true;

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }

  // configure LED for output
  pinMode(LED_PIN, OUTPUT);
}


void loop() {
  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
  while (!mpuInterrupt && fifoCount < packetSize) {

  }
  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    // reset so we can continue cleanly
    mpu.resetFIFO();
    //Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & 0x01) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

    // read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= packetSize;
    int flexADC = analogRead(indexPin);
    float flexV = flexADC * VCC / 1023.0;
    float flexR = R_DIV * (VCC / flexV - 1.0);
    int flexADC2 = analogRead(middlePin);
    float flexV2 = flexADC2 * VCC / 1023.0;
    float flexR2 = R_DIV * (VCC / flexV2 - 1.0);
    int flexADC3 = analogRead(thumbPin);
    float flexV3 = flexADC3 * VCC / 1023.0;
    float flexR3 = R_DIV * (VCC / flexV3 - 1.0);
    deltat = millis() - prevTime;
    if (deltat > 50) {
      angleIndex = map(flexR, STRAIGHT_RESISTANCE_INDEX, BEND_RESISTANCE_INDEX, 0, 90.0);
      angleMiddle = map(flexR2, STRAIGHT_RESISTANCE_MIDDLE, BEND_RESISTANCE_MIDDLE, 0, 90.0);
      angleThumb = map(flexR3, STRAIGHT_RESISTANCE_THUMB, BEND_RESISTANCE_THUMB, 0, 90.0);
      if (angleThumb > thumbThresh &&angleMiddle > middleThresh &&angleIndex > indexThresh)volumeChange = true;
      else{
        volumeChange = false;
      if (angleThumb > thumbThresh && !waveChange) {
        waveChange = true;
        waveform++;
        waveform = waveform % 4; //Used to make sure the number send to puredata is either 0,1,2 or 3.
        waveSend = waveform;
        //Open sound control message that sends the "waveform" value to pure data.
      }
      else if (angleThumb < thumbThresh && waveChange) {
        waveChange = false;
      }
      if (angleMiddle > middleThresh && hipValue == 0 && !hipSwitch)
      {
        hipSwitch = true;
        hipValue = 1;
      } else if (angleMiddle > middleThresh && hipValue !=0 && !hipSwitch)
      {
        hipValue = 0;
        hipSwitch = true;
      } else if ( angleMiddle < middleThresh && hipSwitch)
        hipSwitch = false;
      if (angleIndex > indexThresh && lopValue == 0 && !lopSwitch)
      {
        lopSwitch = true;
        lopValue = 1;
      } else if (angleIndex > indexThresh && lopValue !=0 && !lopSwitch)
      {
        lopValue = 0;
        lopSwitch = true;
      } else if ( angleIndex < indexThresh && lopSwitch)
        lopSwitch = false;
      }
      prevTime = millis();
    }
    // display quaternion values in InvenSense Teapot demo format:
    dataPacket[2] = fifoBuffer[0];
    dataPacket[3] = fifoBuffer[1];
    dataPacket[4] = fifoBuffer[4];
    dataPacket[5] = fifoBuffer[5];
    dataPacket[6] = fifoBuffer[8];
    dataPacket[7] = fifoBuffer[9];
    dataPacket[8] = fifoBuffer[12];
    dataPacket[9] = fifoBuffer[13];
    
    dataPacket[10] = hipValue;
    dataPacket[11] = lopValue;
    dataPacket[12] = waveform;
    dataPacket[13] = volumeChange;
    Serial.write(dataPacket, 14);
    dataPacket[11]++; // packetCount, loops at 0xFF on purpose

    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
  }
}
