// I2C device class (I2Cdev) demonstration Processing sketch for MPU6050 DMP output
// 6/20/2012 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
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

import processing.serial.*;
import processing.opengl.*;
import toxi.geom.*;
import toxi.processing.*;
import oscP5.*;
import netP5.*;
// NOTE: requires ToxicLibs to be installed in order to run properly.
// 1. Download from http://toxiclibs.org/downloads
// 2. Extract into [userdir]/Processing/libraries
//    (location may be different on Mac/Linux)
// 3. Run and bask in awesomeness

ToxiclibsSupport gfx;

Serial port;                         // The serial port
OscP5 oscP5;
char[] dataPacket = new char[14];  // InvenSense Teapot packet
int serialCount = 0;                 // current packet byte position
int aligned = 0;
int interval = 0;
int pitch = 0;
int octave = 72;
float volume;
int hipValue;
int lopValue;
int waveform;
float pX, pY, pZ;
float prevAx = 0;
boolean volumeChange = false;
boolean octaveChanged = false;
float[] q = new float[4];
Quaternion quat = new Quaternion(1, 0, 0, 0);
NetAddress purrData;
float deltaX;
float prevX;
int deltat = 0;
int prevTime = 0;

int[] blackKeys = {1, 0, 1, 1, 0, 1, 1};
void setup() {
  // 300px square viewport using OpenGL rendering
  size(250, 200, OPENGL);
  gfx = new ToxiclibsSupport(this);

  // setup lights and antialiasing

  // display serial port list for debugging/clarity

  // get the first available port (use EITHER this OR the specific port code below)
  //String portName = Serial.list()[0];

  // get a specific serial port (use EITHER this OR the first-available code above)
  String portName = "COM3";

  // open the serial port
  port = new Serial(this, portName, 115200);

  // send single character to trigger DMP init/start
  // (expected by MPU6050_DMP6 example Arduino sketch)
  port.write('r');
  oscP5 = new OscP5(this, 12001);
  purrData = new NetAddress("127.0.01", 12000);
  launch("C:/Users/peter/Desktop/MusicGloveArduino/Processing/MusicGloveProcessing/Mergeattempt.pd");
}

void draw() {
  OscMessage pitchMsg = new OscMessage("/pitch");
  OscMessage inMsg = new OscMessage("/lowPass");
  OscMessage miMsg = new OscMessage("/highPass");
  OscMessage thMsg = new OscMessage("/waveformSelect");
  OscMessage volMsg = new OscMessage("/volume");
  //if (millis() - interval > 1000) {
  //    // resend single character to trigger DMP init/start
  //    // in case the MPU is halted/reset while applet is running
  //    port.write('r');
  //    interval = millis();
  //}
  fill(255, 255, 255);
  // black background
  background(0);
  pushMatrix();
  float[] axis = quat.toAxisAngle();
  //calculating points from axis angles. 
  //Note that toAxisAngle() switches the Z and Y axes
  Vec2D vec = new Vec2D(2*(q[1]*q[2]-q[0]*q[3]),2*(q[3]*q[2]+q[0]*q[1]));
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
  if (!volumeChange) {

    pitch = octave - int(((vec.y+0.95)/1.9)*12);
    if (pitch>108)pitch=108;
    else if (pitch<21)pitch=21;
    pitchMsg.add(pitch);
    oscP5.send(pitchMsg, purrData);
    inMsg.add(lopValue);
    oscP5.send(inMsg, purrData);
    miMsg.add(hipValue);
    oscP5.send(miMsg, purrData);
    thMsg.add(waveform);
    oscP5.send(thMsg, purrData);

    deltat = millis() - prevTime;

    if (deltat > 50) {
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
    volume += - (axis[2]*axis[0])/2 - prevAx/2;
    if (volume >0.5) volume = 0.5; 
    else if (volume < 0) volume = 0;
  }
  volMsg.add(volume);
  oscP5.send(volMsg, purrData);
  prevAx = - axis[2]*axis[0];
  
  // draw main body in red
  //for (int i= 0; i<52;i++){
  //  fill(255,255,255);
  //  AABB whiteKey=new AABB(new Vec3D(10*i,0,10),new Vec3D(10,30,10));
  //  gfx.box(whiteKey);
  //  AABB blackKey = new AABB(new Vec3D(10*i+4.44,0,10),new Vec3D(10,20,10));
  //  if (checkBlack(i)){
  //    fill(0,0,0);
  //    gfx.box(blackKey);
  //  }
  //}

  // draw front-facing tip in blue   
  popMatrix();
}

void serialEvent(Serial port) {
  interval = millis();
  while (port.available() > 0) {
    int ch = port.read();
    print((char)ch);
    if (serialCount > 0 || ch == '$') {
      dataPacket[serialCount++] = (char)ch;
      if (serialCount == 14) {
        serialCount = 0; // restart packet byte position

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
}

boolean checkBlack(int i) {
  if (i<7) {
    if (i*blackKeys[i]!=0) {
      return true;
    }
  } else if (i*blackKeys[i%7]!=0) return true;
  return false;
}
