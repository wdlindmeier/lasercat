#include <Servo.h>

/*To define motor pin which is 5, 6, 10, 11*/
int MotorRight1=8;
int MotorRight2=9;
int MotorLeft1=10;
int MotorLeft2=11;

/*Pin definition to control speed*/
int SpeedPinL = 6;
int SpeedPinR = 5;

int counter=0;

long frameNum = 0;

void setup()
{  
  
  Serial.begin(9600);
  
  pinMode(MotorRight1, OUTPUT);  // pin 8 
  pinMode(MotorRight2, OUTPUT);  // pin 9 (PWM)
  pinMode(MotorLeft1,  OUTPUT);  // pin 10 (PWM) 
  pinMode(MotorLeft2,  OUTPUT);  // pin 11 (PWM)

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

long lastSignalFrame = 0;

void loop() 
{  
  
  frameNum = (frameNum + 1) % 100000000;
  
  digitalWrite(TrackerLEDFront, HIGH);
  digitalWrite(TrackerLEDBack, HIGH);
  
  if(Serial.available()){
  
    int MAX_CHAR = 100;
    char readVal[MAX_CHAR];
    int numChar = Serial.readBytesUntil('\n', readVal, MAX_CHAR);
    String inVal = String(readVal).substring(0,numChar);
    
    lastSignalFrame = frameNum;
     
    long val = inVal.toInt();
    int rVal = val % 1000;
    int lVal = (val - rVal) * 0.001;
    
    int lDirection = lVal >= 255 ? 1 : -1;
    int rDirection = rVal >= 255 ? 1 : -1; 
     
    int lAbsVal = abs(lVal-255);
    int rAbsVal = abs(rVal-255);

     if(lAbsVal >= 0 && lAbsVal <= 255 && rAbsVal >= 0 && rAbsVal <= 255){
     
       setLeftDirection(lDirection);
       setRightDirection(rDirection);
       
       analogWrite(SpeedPinL, lAbsVal);
       analogWrite(SpeedPinR, rAbsVal);
      
      /*
       Serial.print("lAbsVal: ");
       Serial.println(lAbsVal);
       Serial.print("rAbsVal: ");
       Serial.println(rAbsVal);
      */
      
     }
   
   }else if((frameNum - (lastSignalFrame % 100000000)) > 5000){
   
     analogWrite(SpeedPinL, 0);
     analogWrite(SpeedPinR, 0);
    
   }

}  

