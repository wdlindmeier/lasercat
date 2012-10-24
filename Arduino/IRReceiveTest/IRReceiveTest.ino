/*
* Project: LaserCat 
* Description: Arduino IR controlled car for ITP 2012 Pcom mideterm project
* Team: Bill, Max and Harry
*/

#include <IRremote.h>  /*Customized IRremote library*/
#include <IRremoteInt.h>
#include <Servo.h>

//#define USE_SONAR;
//#define USE_CN70;

/*Pin definition to control speed*/
int SpeedPin1 = 5;
int SpeedPin2 = 6;


/*To define motor pin which is 5, 6, 10, 11*/
int MotorRight1=8;
int MotorRight2=9;
int MotorLeft1=10;
int MotorLeft2=11;

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

void setup()
{  
  
  Serial.begin(9600);
  irrecv.enableIRIn();     // IR receiver enable

  Serial.println("Setup done");
  
}

void loop() 
{  
   long val = 255255;
  
   if (irrecv.decode(&results)) // start to parse IR result
   {      
     long val = results.value;
     int rVal = val % 1000;
     int lVal = (val - rVal) * 0.001;

     Serial.print("lVal: ");
     Serial.println(lVal);          
     Serial.print("rVal: ");
     Serial.println(rVal);   
     
     irrecv.resume(); //set IR as a idle state          
   }

}  

