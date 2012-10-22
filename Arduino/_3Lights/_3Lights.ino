
#define LED_R  9
#define LED_G  10
#define LED_B  8
#define LED_B2  11

void setup()
{
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_B2, OUTPUT);
 
}

void loop()
{
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, HIGH);
  digitalWrite(LED_B2, HIGH);
}
