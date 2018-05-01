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
char[] teapotPacket = new char[14];  // InvenSense Teapot packet
int serialCount = 0;                 // current packet byte position
int aligned = 0;
int interval = 0;
int pitch = 0;
int octave = 72;
int hipValue;
int lopValue;
int waveform;
boolean volumeChange = false;
boolean octaveChanged = false;
float[] q = new float[4];
Quaternion quat = new Quaternion(1, 0, 0, 0);
NetAddress purrData;
float[] gravity = new float[3];
float[] euler = new float[3];
float[] ypr = new float[3];
int[] blackKeys = {1,0,1,1,0,1,1};
void setup() {
    // 300px square viewport using OpenGL rendering
    size(520, 300, OPENGL);
    gfx = new ToxiclibsSupport(this);

    // setup lights and antialiasing
    lights();
    smooth();
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
    purrData = new NetAddress("127.0.01",12000);
}

void draw() {
  OscMessage pitchMsg = new OscMessage("/pitch");
  OscMessage inMsg = new OscMessage("/lowPass");
  OscMessage miMsg = new OscMessage("/highPass");
  OscMessage thMsg = new OscMessage("/waveformSelect");
    //if (millis() - interval > 1000) {
    //    // resend single character to trigger DMP init/start
    //    // in case the MPU is halted/reset while applet is running
    //    port.write('r');
    //    interval = millis();
    //}
    fill(255,255,255);
    // black background
    background(0);
    int booldude=0;
    if (volumeChange) booldude = 1;
    // translate everything to the middle of the viewport
    pushMatrix();


    // 3-step rotation from yaw/pitch/roll angles (gimbal lock!)
    // ...and other weirdness I haven't figured out yet
    //rotateY(-ypr[0]);
    //rotateZ(-ypr[1]);
    //rotateX(-ypr[2]);

    // toxiclibs direct angle/axis rotation from quaternion (NO gimbal lock!)
    // (axis order [1, 3, 2] and inversion [-1, +1, +1] is a consequence of
    // different coordinate system orientation assumptions between Processing
    // and InvenSense DMP)
    float[] axis = quat.toAxisAngle();
    text(axis[0], 10, 30);
    pitch = octave - int(((axis[1]*axis[0]+0.8)/1.6)*12);
    if (pitch>108)pitch=108;
    else if (pitch<21)pitch=21;
    pitchMsg.add(pitch);
    oscP5.send(pitchMsg, purrData);
    inMsg.add(hipValue);
    oscP5.send(inMsg, purrData);
    miMsg.add(lopValue);
    oscP5.send(miMsg, purrData);
    thMsg.add(waveform);
    oscP5.send(thMsg, purrData);
    
    text(pitch, 10, 40);
    text(hipValue, 100,110);text(lopValue, 100,120);
                    text(waveform, 100,130);
                    text(booldude, 100,140);
    if (!octaveChanged){
      if (axis[3]*axis[0]<-0.400){
         octave-=12;
         octaveChanged=true;
      } else if (axis[3]*axis[0]>0.400){
         octave+=12;
         octaveChanged=true;
      }
    }
    if (axis[3]*axis[0]>-0.200&&axis[3]*axis[0]<0.200)octaveChanged=false;
    
    if (octave>101)octave=101;
    else if (octave<27)octave=27;
    text(octave, 10, 50);
    text(axis[2]*axis[0], 10, 60);
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
        if (aligned < 4) {
            // make sure we are properly aligned on a 14-byte packet
            if (serialCount == 0) {
                if (ch == '$') aligned++; else aligned = 0;
            } else if (serialCount == 1) {
                if (ch == 2) aligned++; else aligned = 0;
            } else if (serialCount == 12) {
                if (ch == '\r') aligned++; else aligned = 0;
            } else if (serialCount == 13) {
                if (ch == '\n') aligned++; else aligned = 0;
            }
            //println(ch + " " + aligned + " " + serialCount);
            serialCount++;
            if (serialCount == 14) serialCount = 0;
        } else {
            if (serialCount > 0 || ch == '$') {
                teapotPacket[serialCount++] = (char)ch;
                if (serialCount == 14) {
                    serialCount = 0; // restart packet byte position
                    
                    // get quaternion from data packet
                    q[0] = ((teapotPacket[2] << 8) | teapotPacket[3]) / 16384.0f;
                    q[1] = ((teapotPacket[4] << 8) | teapotPacket[5]) / 16384.0f;
                    q[2] = ((teapotPacket[6] << 8) | teapotPacket[7]) / 16384.0f;
                    q[3] = ((teapotPacket[8] << 8) | teapotPacket[9]) / 16384.0f;
                    for (int i = 0; i < 4; i++) if (q[i] >= 2) q[i] = -4 + q[i];
                    
                    // set our toxilibs quaternion to new data
                    quat.set(q[0], q[1], q[2], q[3]);
                    hipValue = teapotPacket[10];
                    lopValue = teapotPacket[11];
                    if (hipValue == 136) hipValue = 50000;
                    else hipValue =0;
                    if (lopValue == 80) lopValue = 5000;
                    else lopValue = 0;
                    waveform = teapotPacket[12];
                    if ((teapotPacket[13]) == 1)
                    volumeChange = true;
                    else volumeChange = false;
                    fill(255,255,255);
                    text(hipValue, 100,110);text(lopValue, 100,120);
                    text(waveform, 100,130);
                    text(teapotPacket[13], 100,140);
                }
            }
        }
    }
}

boolean checkBlack(int i){
  if (i<7){
    if (i*blackKeys[i]!=0){
      return true;
    }
  } else if (i*blackKeys[i%7]!=0) return true;
  return false;
}
