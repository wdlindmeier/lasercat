/*
 * IRRemote ]
 *     NOTE: IR pin must be pin 3
 */
#include <IRremote.h>                    

const int buttonPin = 4;                 // 
int buttonState = 0;                     // 

IRsend irsend;                           //

void setup()
{
  Serial.begin(9600);
  pinMode(buttonPin, INPUT);             // button as input
  Serial.println("setup done");
}

void loop() 
{
  
  buttonState = digitalRead(buttonPin);

  
  // check if button pressed
  if (buttonState == HIGH) {    
    Serial.println("IR send");
    // irsend a code
    irsend.sendNEC(0x4FB48B7, 32);   //
  }
}

