/*
 * IRRemote ]
 *     NOTE: IR pin must be pin 3
 */
#include <IRremote.h>        

/*IR code definitin*/
long IRfront= 0x00FFA25D;       //forward
long IRback=0x00FF629D;         //backward
long IRturnright=0x00FFC23D;    //right
long IRturnleft= 0x00FF02FD;    //left
long IRstop=0x00FFE21D;         //stop
long IRcny70=0x00FFA857;        //CNY70 automode
long IRAutorun=0x00FF906F;      //sonar automode
long IRturnsmallleft= 0x00FF22DD; 

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
    irsend.sendNEC(IRback, 32);   //
  }
}

