// I2C device class (I2Cdev) demonstration Processing sketch for MPU6050 DMP output
// 6/20/2012 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// The Processing sketch has been modified by 4th semester Medialogy students at Aalborg University
// in order to use it for a prototype of a musical glove.
//
// Changelog:
//     2012-06-20 - initial release

/* ============================================
 I2Cdev device library code is placed under the MIT license
 Copyright (c) 2012 Jeff Rowberg
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ===============================================
 */

// Importing libraries
import processing.serial.*;
import processing.opengl.*;
import toxi.geom.*;
import toxi.processing.*;
import oscP5.*;
import netP5.*;


ToxiclibsSupport gfx;

Serial glovePort;                       // The serial port
Serial drumPort;
OscP5 oscP5;                       // OscP5 object to create and send OSC messages
char[] dataPacket = new char[14];  // Char array to receive data
int serialCount = 0;               // current packet byte position
int aligned = 0;
int interval = 0;
int drumVal = 0;
int pitch = 0;                     // Integer for storing the midi value of the pitch
int octave = 72;                   // Integer for storing the current value of the octave
float volume;                      // Floating point for storing the volume
int hipValue, lopValue, waveform;  // Integers for storing the high pass, low pass and waveform values.                    
float prevAx = 0;                  // Floating point to store the previous value of rotation for volume.
boolean volumeChange = false;      // Booleans for whether the volume and octave should change.
boolean octaveChanged = false;
float[] q = new float[4];          // Array to store quaternion values before passing them on to toxiclibs Quaternion object.
Quaternion quat = new Quaternion(1, 0, 0, 0);
NetAddress purrData;               // NetAddress object for purr data.
float deltaX, prevX;               // Floating points for calculating a deltaX, which is used to determine when the octave should change.
int deltat = 0;                    // Integers for calculating a delta time.
int prevTime = 0;

void setup() {
  //Set size.
  size(250, 200, OPENGL);
  gfx = new ToxiclibsSupport(this);

  //Defining serial port to use.
  String glovePortName = "COM3";
  String drumPortName = "COM5";
  // open the serial port
  glovePort = new Serial(this, glovePortName, 115200);
  drumPort = new Serial(this, drumPortName, 9600);
  //Create new OscP5 object with receiving udp port 12001
  oscP5 = new OscP5(this, 12001);
  //Local port for sending OSC messages to purr data.
  purrData = new NetAddress("127.0.01", 12000);
  launch("C:/Users/Thomas/Documents/Glovebois/MusicGloveArduino/Processing/MusicGloveProcessing/Mergeattempt.pd");
}

void draw() {
  // Define OSC messages for purr data.
  OscMessage pitchMsg = new OscMessage("/pitch");
  OscMessage inMsg = new OscMessage("/lowPass");
  OscMessage miMsg = new OscMessage("/highPass");
  OscMessage thMsg = new OscMessage("/waveformSelect");
  OscMessage volMsg = new OscMessage("/volume");
  OscMessage drumMsg = new OscMessage("/drum");
  fill(255, 255, 255); // White text, black background.
  background(0);
  pushMatrix();
  // Using toxiclibs to convert the quaternion to axis angles.
  float[] axis = quat.toAxisAngle();
  //calculating vector from quaternion
  Vec2D vec = new Vec2D(2*(quat.x*quat.y-quat.w*quat.z),2*(quat.z*quat.y+quat.w*quat.x));
  // Drawing text for the different values 
  text("Pitch: " + pitch, 10, 40);
  if (hipValue > 0)
    text("High pass filter: ON",10,50);
    else text("High pass filter: OFF",10,50);
  if (lopValue > 0)text("Low pass filter:  ON",10,60); 
  else text("Low pass filter:  OFF",10,60);
  if (waveform == 0) text("Waveform: SINE",10,70);
  else if (waveform==1) text("Waveform: SAWTOOTH",10,70);
  else if (waveform==2) text("Waveform: SQUARE",10,70);
  else if (waveform==3) text("Waveform: OFF",10,70);
  text("Volume: " + volume*200,10,80);
  // Check if the volume shouldn't change.
  if (!volumeChange) {
    // Calculate pitch.
    pitch = octave - int(((vec.y+0.95)/1.9)*12);
    if (pitch>108)pitch=108;
    else if (pitch<21)pitch=21;
    // Calculate delta time.
    deltat = millis() - prevTime;
    // Check if delta time is above 50 ms.
    if (deltat > 50) {
      // Determine whether the octave should change and how it should change.
      deltaX = vec.x- prevX;
      if (!octaveChanged) {
        if (deltaX<-0.20) {
          octave-=12;
          octaveChanged=true;
          prevTime = millis();
        } 
        if (deltaX>0.20) {
          octave+=12;
          octaveChanged=true;
          prevTime = millis();
        }
        prevX = vec.x;
      }
    }
    if (deltaX>-0.10&&deltaX<0.10)octaveChanged=false;

    if (octave>108)octave=108;
    else if (octave<24)octave=24;
  } else {
    // Change volume according to rotation. Block only entered if three fingers are bent.
    volume += - (axis[2]*axis[0])/2 - prevAx/2;
    if (volume >0.5) volume = 0.5; 
    else if (volume < 0) volume = 0;
  }
  // Send all OSC messages.
  pitchMsg.add(pitch);
  oscP5.send(pitchMsg, purrData);
  inMsg.add(lopValue);
  oscP5.send(inMsg, purrData);
  miMsg.add(hipValue);
  oscP5.send(miMsg, purrData);
  thMsg.add(waveform);
  oscP5.send(thMsg, purrData);
  volMsg.add(volume);
  oscP5.send(volMsg, purrData);
  drumMsg.add(drumVal);
  oscP5.send(drumMsg, purrData);
  prevAx = - axis[2]*axis[0];
 
  popMatrix();
}

void serialEvent(Serial port) {
  if (port == glovePort){
  interval = millis();
  while (port.available() > 0) {
    int ch = port.read();
    print((char)ch);
    if (serialCount > 0 || ch == '$') {
      dataPacket[serialCount++] = (char)ch;
      if (serialCount == 14) {
        serialCount = 0; 

        // get quaternion from data packet
        q[0] = ((dataPacket[2] << 8) | dataPacket[3]) / 16384.0f;
        q[1] = ((dataPacket[4] << 8) | dataPacket[5]) / 16384.0f;
        q[2] = ((dataPacket[6] << 8) | dataPacket[7]) / 16384.0f;
        q[3] = ((dataPacket[8] << 8) | dataPacket[9]) / 16384.0f;
        for (int i = 0; i < 4; i++) if (q[i] >= 2) q[i] = -4 + q[i];

        // set our toxilibs quaternion to new data
        quat.set(q[0], q[1], q[2], q[3]);
        if (dataPacket[10] == 1) hipValue = 5000;
        else hipValue = 0;
        if (dataPacket[11] == 1) lopValue = 50000;
        else lopValue = 0;
        waveform = dataPacket[12];
        if ((dataPacket[13]) == 1)
          volumeChange = true;
        else volumeChange = false;
        fill(255, 255, 255);
      }
    }
  }
  } else if (port == drumPort){
    drumVal = drumPort.read();
  }
}
