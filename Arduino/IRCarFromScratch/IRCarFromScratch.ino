#include <IRremote.h>
#include <IRremoteInt.h>
#include <Servo.h>

/*To define motor pin which is 5, 6, 10, 11*/
int MotorRight1=8;
int MotorRight2=9;
int MotorLeft1=10;
int MotorLeft2=11;
int TrackerLEDFront=13;
int TrackerLEDBack=12;//...

/*Pin definition to control speed*/
int SpeedPinL = 6;
int SpeedPinR = 5;

int counter=0;
const int irReceiverPin = 2; //IR receiver pin which is 2

/*
/*IR code definitin*/
long IRfront= 0x00FFA25D;       //forward
long IRback=0x00FF629D;         //backward
long IRturnright=0x00FFC23D;    //right
long IRturnleft= 0x00FF02FD;    //left
long IRstop=0x00FFE21D;         //stop
long IRcny70=0x00FFA857;        //CNY70 automode
long IRAutorun=0x00FF906F;      //sonar automode
long IRturnsmallleft= 0x00FF22DD; 

IRrecv irrecv(irReceiverPin);  // Setup IR receiver pin
decode_results results;       // Results returned from the IR decoder
long frameNum = 0;

void setup()
{  
  
  Serial.begin(9600);
  irrecv.enableIRIn();     // IR receiver enable
  
  pinMode(MotorRight1, OUTPUT);  // pin 8 
  pinMode(MotorRight2, OUTPUT);  // pin 9 (PWM)
  pinMode(MotorLeft1,  OUTPUT);  // pin 10 (PWM) 
  pinMode(MotorLeft2,  OUTPUT);  // pin 11 (PWM)

//  pinMode(SpeedPinL, OUTPUT);  // pin 5 (PWM)
//  pinMode(SpeedPinR, OUTPUT);  // pin 6 (PWM)

  pinMode(TrackerLEDFront, OUTPUT);
  pinMode(TrackerLEDBack, OUTPUT);

  Serial.println("Setup done");
  
}

void setLeftDirection(int dir)
{
  digitalWrite(MotorLeft1, dir >= 0 ? LOW : HIGH);
  digitalWrite(MotorLeft2, dir >= 0 ? HIGH : LOW);
}

void setRightDirection(int dir)
{     
  digitalWrite(MotorRight2, dir >= 0 ? LOW : HIGH);
  digitalWrite(MotorRight1, dir >= 0 ? HIGH : LOW);
}

// What are the problems:
// Wheels are going wrong direction
// Wrong set of wheels is turning

long lastSignalFrame = 0;

void loop() 
{  
  
  frameNum = (frameNum + 1) % 100000000;
  
  digitalWrite(TrackerLEDFront, HIGH);
  digitalWrite(TrackerLEDBack, HIGH);
  
  /*
     // Backward

     setLeftDirection(1);
     
     setRightDirection(1);
     
     // Forward

     analogWrite(SpeedPinL, 150);
     analogWrite(SpeedPinR, 150);
     
     delay(1000);
*/     

   if (irrecv.decode(&results)) // start to parse IR result
   {      
     
     lastSignalFrame = frameNum;
     
     long val = results.value;
     int rVal = val % 1000;
     int lVal = (val - rVal) * 0.001;

     // The incoming range is 0-510
     /*
     Serial.print("lVal: ");
     Serial.println(lVal);          
     Serial.print("rVal: ");
     Serial.println(rVal);  
     */
     
     // TODO: Make -1 & 1 constants
     int lDirection = lVal >= 255 ? 1 : -1;
     int rDirection = rVal >= 255 ? 1 : -1; 
     
     int lAbsVal = abs(lVal-255);
     int rAbsVal = abs(rVal-255);

     // Ignore any anomylous data     
     
     if(lAbsVal >= 0 && lAbsVal <= 255 && rAbsVal >= 0 && rAbsVal <= 255){
       
       setLeftDirection(lDirection);
       setRightDirection(rDirection);
       
       analogWrite(SpeedPinL, lAbsVal);
       analogWrite(SpeedPinR, rAbsVal);
  
       Serial.print("lAbsVal: ");
       Serial.println(lAbsVal);
       Serial.print("rAbsVal: ");
       Serial.println(rAbsVal);
       
     }

     irrecv.resume(); //set IR as a idle state          
     
   }else if((frameNum - (lastSignalFrame % 100000000)) > 5000){
       
//     Serial.println("Kill");
     analogWrite(SpeedPinL, 0);
     analogWrite(SpeedPinR, 0);

   }

}  

