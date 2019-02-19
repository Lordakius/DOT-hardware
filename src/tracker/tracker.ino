int blue = 13; 
int red = 14;

void setup()
{
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);
} 

void loop()
{
  digitalWrite(blue, HIGH);
  digitalWrite(red, HIGH);
  delay(1000);
  digitalWrite(blue, LOW);
  digitalWrite(red, LOW);
  delay(1000);
} 
