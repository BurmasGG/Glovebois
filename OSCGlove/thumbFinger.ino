void thumb(){
  
  int flexADC3 = analogRead(thumbPin);
  float flexV3 = flexADC3 * VCC / 1023.0;
  float flexR3 = R_DIV * (VCC / flexV3 - 1.0);
  thumbVal = analogRead(thumb);
  float angle3 = map(flexR3, STRAIGHT_RESISTANCE_THUMB, BEND_RESISTANCE_THUMB,0, 90.0);


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
