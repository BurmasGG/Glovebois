void index(){

  int flexADC = analogRead(indexPin);
  float flexV = flexADC * VCC / 1023.0;
  float flexR = R_DIV * (VCC / flexV - 1.0);
  indexVal = analogRead(indexPin); 
  deltat = millis()-prevTimeI;
  if (deltat > 50)
  {
    angleIndex = map(flexR, STRAIGHT_RESISTANCE_INDEX, BEND_RESISTANCE_INDEX,0, 90.0);
    prevTimeI = millis();
  }
  if(angleIndex > indexThresh && lopValue == 0 && !lopSwitch)
  {
  lopSwitch = true;
  lopValue = 50000;
  OSCMessage lopMsg("/lowPass");
  lopMsg.add(lopValue);
  SLIPSerial.beginPacket();
  lopMsg.send(SLIPSerial);
  SLIPSerial.endPacket();
  lopMsg.empty();
  } else if(angleIndex > indexThresh && lopValue == 50000 && !lopSwitch)
  {
  lopValue = 0;
  lopSwitch = true;
  OSCMessage lopMsg("/lowPass");
  lopMsg.add(lopValue);
  SLIPSerial.beginPacket();
  lopMsg.send(SLIPSerial);
  SLIPSerial.endPacket();
  lopMsg.empty();
  } else if ( angleIndex < indexThresh && lopSwitch)
  lopSwitch = false;
  
  
}


