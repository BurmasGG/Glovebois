void thumb(){
  
  int flexADC3 = analogRead(thumbPin);
  float flexV3 = flexADC3 * VCC / 1023.0;
  float flexR3 = R_DIV * (VCC / flexV3 - 1.0);
  thumbVal = analogRead(thumbPin);
  deltat = millis()-prevTime;

  if (deltat > 50)
  {
  angleThumb = map(flexR3, STRAIGHT_RESISTANCE_THUMB, BEND_RESISTANCE_THUMB,0, 90.0);
  prevTime = millis();
  }

  if (angleThumb > thumbThresh && !waveChange) {
  waveChange = true;
  waveform++;
  waveform = waveform % 4; //Used to make sure the number send to puredata is either 0,1,2 or 3. 
  waveSend = waveform;
  //Open sound control message that sends the "waveform" value to pure data.
  OSCMessage waveMsg("/waveformSelect");
  waveMsg.add(waveSend);
  SLIPSerial.beginPacket();
  waveMsg.send(SLIPSerial);
  SLIPSerial.endPacket();
  waveMsg.empty();
  }
 else if(angleThumb < thumbThresh && waveChange){
  waveChange = false;
 }
 
}
