const int drumPin = A0;
const int drumThresh = 5;
int drumRead;
int deltat = 0;
int prevTime;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
    drumRead = analogRead(drumPin);
    if (drumRead > 100){
    Serial.write(1);
    } else Serial.write(0);
  delay(50);
}
