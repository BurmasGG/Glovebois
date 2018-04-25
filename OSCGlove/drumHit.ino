void drum(){
  drumRead = analogRead(drumPin);
  if (drumRead > drumThresh)
    {
    drumRead = 1;
    hit = true;
    }
  else
  {
    drumRead = 0;
    hit = false;
  }

   OSCMessage msg2("/drum");
   msg2.add(drumRead);
   SLIPSerial.beginPacket();
   msg2.send(SLIPSerial);
   SLIPSerial.endPacket();
   msg2.empty();
}

