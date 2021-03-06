void middle(){
  
  int flexADC2 = analogRead(middlePin);
  float flexV2 = flexADC2 * VCC / 1023.0;
  float flexR2 = R_DIV * (VCC / flexV2 - 1.0);
  middleVal = analogRead(middlePin);
  deltat = millis()-prevTimeM;
  
  if (deltat > 50)
  {
  angleMiddle = map(flexR2, STRAIGHT_RESISTANCE_MIDDLE, BEND_RESISTANCE_MIDDLE,0, 90.0);
  prevTimeM = millis();
  }
  
  if(angleMiddle > middleThresh && hipValue == 0 && !hipSwitch)
  {
  hipSwitch = true;
  hipValue = 5000;
  OSCMessage hipMsg("/highPass");
  hipMsg.add(hipValue);
  SLIPSerial.beginPacket();
  hipMsg.send(SLIPSerial);
  SLIPSerial.endPacket();
  hipMsg.empty();
  }else if(angleMiddle > middleThresh && hipValue == 5000 && !hipSwitch)
  {
  hipValue = 0;
  hipSwitch = true;
  OSCMessage hipMsg("/highPass");
  hipMsg.add(hipValue);
  SLIPSerial.beginPacket();
  hipMsg.send(SLIPSerial);
  SLIPSerial.endPacket();
  hipMsg.empty();
  }else if ( angleMiddle < middleThresh && hipSwitch)
  hipSwitch = false;
  
  }
