float pin = 0;                
int distance = 0;                

void setup()
{
 Serial.begin(9600);             
}

void loop()
{
 distance = analogRead(pin);      
 Serial.println(distance);            
 delay(100);                   
}
