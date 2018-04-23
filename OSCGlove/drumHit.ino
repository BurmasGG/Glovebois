void drum(){
  drumRead = analogRead(drumPin);
  
  if (drumRead > drumThresh)
    {
    Serial.println("drum tab is working");
    drumRead = 0;
    hit = false;
    }
  else
  {
    drumRead = 1;
    hit = true;
  }

   OSCMessage msg2("/drum");
   msg2.add(drumRead);
   SLIPSerial.beginPacket();
   msg2.send(SLIPSerial);
   SLIPSerial.endPacket();
   msg2.empty();
}

