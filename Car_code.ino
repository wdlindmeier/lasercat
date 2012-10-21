/*
* Project: LaserCat 
* Description: Arduino IR controlled car for ITP 2012 Pcom mideterm project
* Team: Bill, Max and Harry
*/

#include <IRremote.h>  /*Customized IRremote library*/
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

/*IR code definitin*/
long IRfront= 0x00FFA25D;       //forward
long IRback=0x00FF629D;         //backward
long IRturnright=0x00FFC23D;    //right
long IRturnleft= 0x00FF02FD;    //left
long IRstop=0x00FFE21D;         //stop
long IRcny70=0x00FFA857;        //CNY70 automode
long IRAutorun=0x00FF906F;      //sonar automode
long IRturnsmallleft= 0x00FF22DD; 

#ifdef USE_CN70
/* CNY70 pin definition */
const int SensorLeft = 7;      //sensor left pin
const int SensorMiddle= 4 ;    //sensor mid pin
const int SensorRight = 3;     //sensor right oin

int SL;    //left sensor status
int SM;    //mid sensor status
int SR;    //right sensor status
#endif

IRrecv irrecv(irReceiverPin);  // Setup IR receiver pin
decode_results results;       // Results returned from the IR decoder

#ifdef USE_SONAR
/*pin definition for sonar*/
int inputPin =13 ; //RX pin def
int outputPin =12; //TX pin def
int Fspeedd = 0; //forward distance
int Rspeedd = 0; //right distance
int Lspeedd = 0; //left distance
int directionn = 0; //fron=8 back=2 left=4 right=6 
Servo myservo;
int delay_time = 250; //servo motor stable time
int Fgo = 8; //forward
int Rgo = 6; //right turn
int Lgo = 4; //left turn
int Bgo = 2; //backward
#endif USE_SONAR

void setup()
{  
  Serial.begin(9600);
  pinMode(MotorRight1, OUTPUT);  // pin 5 (PWM)
  pinMode(MotorRight2, OUTPUT);  // pin 6 (PWM)
  pinMode(MotorLeft1,  OUTPUT);  // pin 10 (PWM) 
  pinMode(MotorLeft2,  OUTPUT);  // pin 11 (PWM)
  
  //pinMode(SpeedPin1, OUTPUT);
  //pinMode(SpeedPin2, OUTPUT);
  
  irrecv.enableIRIn();     // IR receiver enable
#ifdef USE_CN70  
  pinMode(SensorLeft, INPUT); 
  pinMode(SensorMiddle, INPUT);
  pinMode(SensorRight, INPUT);
  digitalWrite(2,HIGH);
#endif  
#ifdef USE_SONAR
  pinMode(inputPin, INPUT);
  pinMode(outputPin, OUTPUT);
  myservo.attach(9);// servo to pin 9
#endif
  
  Serial.println("car setup done!"); 
}
 
//******************************************************************(Void)
void advance(int a) //move forward
{
    // HARRY: 
    // Noted that we can only control one side motors right now due to control card (L298) design
    // Or we make a wrong HW layout?
    
    // pin          wheel    movement
    // ----         -----    --------
    // MotorLeft1   left     
    // MotorLeft2   left
    // MotorRight1  right
    // MotorRight2  right
    
    
    digitalWrite(MotorRight1,HIGH);//
    digitalWrite(MotorLeft2,HIGH);// reverse
    //digitalWrite(MotorRight1,LOWs);
    //digitalWrite(MotorRight2,HIGH);
    //digitalWrite(MotorLeft1,LOW);
    //digitalWrite(MotorLeft2,HIGH);
    delay(a * 100); 
}

void right(int b) //move right
{  
     digitalWrite(MotorRight2,HIGH); // right wheels counterclock wise
     digitalWrite(MotorLeft2,HIGH);
     delay(b * 150);
  /*
       digitalWrite(MotorLeft1,LOW);
       digitalWrite(MotorLeft2,HIGH);
       digitalWrite(MotorRight1,LOW);
       digitalWrite(MotorRight2,LOW);
       delay(b * 100);
   */
}

void left(int c) //move left
{
    digitalWrite(MotorRight1,HIGH); // right wheels counterclock wise
    digitalWrite(MotorLeft1,HIGH);
    delay(c * 150);

  /*
  digitalWrite(MotorRight1,LOW);
  digitalWrite(MotorRight2,HIGH);
  digitalWrite(MotorLeft1,LOW);
  digitalWrite(MotorLeft2,LOW);
  delay(c * 100);
   */
}

void turnR(int d) //turn right (unused)
{
    digitalWrite(MotorRight1,HIGH);
    digitalWrite(MotorRight2,LOW);
    digitalWrite(MotorLeft1,LOW);
    digitalWrite(MotorLeft2,HIGH);
    delay(d * 100);
}

void turnL(int e) //turn left (unused)
{
      digitalWrite(MotorRight1,LOW);
      digitalWrite(MotorRight2,HIGH);
      digitalWrite(MotorLeft1,HIGH);
      digitalWrite(MotorLeft2,LOW);
      delay(e * 100);
} 




void SpeedTest(int d) {
    for(int value = 0 ; value <= 255; value+=10)
    {
        digitalWrite(MotorRight2,HIGH);  
        digitalWrite(MotorLeft1, HIGH);      
        analogWrite(SpeedPin1, value);   //PWM Speed Control
        analogWrite(SpeedPin2, value);   //PWM Speed Control
        delay(d);
     }     
}

void backSlower(int g) {
    Serial.println("back slower");
    digitalWrite(MotorRight2,HIGH);//
    digitalWrite(MotorLeft1,HIGH);// reverse
    analogWrite(SpeedPin1,50);
    analogWrite(SpeedPin2,50);
    delay(g* 100);
}


// backforwad
void back(int g){
    
    digitalWrite(MotorRight2,HIGH);//
    digitalWrite(MotorLeft1,HIGH);// reverse
    analogWrite(SpeedPin1,255);
    analogWrite(SpeedPin2,255);
    delay(g* 100);
    
    
    /*
    digitalWrite(MotorRight1,HIGH);
    digitalWrite(MotorRight2,LOW);
    digitalWrite(MotorLeft1,HIGH);
    digitalWrite(MotorLeft2,LOW);;
    delay(g * 100); 
    */
}

void stop(int f) //stop all
{
     digitalWrite(MotorRight1,LOW);
     digitalWrite(MotorRight2,LOW);
     digitalWrite(MotorLeft1,LOW);
     digitalWrite(MotorLeft2,LOW);
     delay(f * 100);
}

#ifdef USE_SONAR
void detection() //measure 3 angles
{ 
    int delay_time = 250; 
    ask_pin_F(); //read distance ahead
    
   // distance smaller than 10cm
   if(Fspeedd < 10) {
      stopp(1);
      back(2); 
   }
   
   if(Fspeedd < 25) {
      stopp(1); 
      ask_pin_L(); 
      delay(delay_time); 
      ask_pin_R(); 
      delay(delay_time);  


      if (Lspeedd > Rspeedd) {
          directionn = Lgo; 
      }
      if (Lspeedd <= Rspeedd) {
        directionn = Rgo; 
      } 
      if (Lspeedd < 15 && Rspeedd < 15) {
        directionn = Bgo;  
      } 
    }
    else { // more than 25cm
      directionn = Fgo; //keep forward 
   }
}   

// to meature distance ahead
void ask_pin_F() 
{
    myservo.write(90);
    digitalWrite(outputPin, LOW); // low 2μs
    delayMicroseconds(2);
    digitalWrite(outputPin, HIGH); // high 10μs
    delayMicroseconds(10);
    digitalWrite(outputPin, LOW); // keep low
    float Fdistance = pulseIn(inputPin, HIGH); //
    Fdistance= Fdistance/5.8/10; // time to distance (cm)
    Serial.print("F distance:"); 
    Serial.println(Fdistance); 
    Fspeedd = Fdistance; // distance as front speed 
} 

void ask_pin_L()
{
    myservo.write(177);
    delay(delay_time);
    digitalWrite(outputPin, LOW);
    delayMicroseconds(2);
    digitalWrite(outputPin, HIGH); 
    delayMicroseconds(10);
    digitalWrite(outputPin, LOW); 
    float Ldistance = pulseIn(inputPin, HIGH); 
    Ldistance= Ldistance/5.8/10; 
    Serial.print("L distance:"); 
    Serial.println(Ldistance); 
    Lspeedd = Ldistance; 
} 

void ask_pin_R() 
{
    myservo.write(5);
    delay(delay_time);
    digitalWrite(outputPin, LOW); 
    delayMicroseconds(2);
    digitalWrite(outputPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(outputPin, LOW); 
    float Rdistance = pulseIn(inputPin, HIGH);
    Rdistance= Rdistance/5.8/10; 
    Serial.print("R distance:"); 
    Serial.println(Rdistance); 
    Rspeedd = Rdistance; 
} 
#endif

void loop() 
{   
  
#ifdef USE_CN70
      SL = digitalRead(SensorLeft);
      SM = digitalRead(SensorMiddle);
      SR = digitalRead(SensorRight);
#endif

   if (irrecv.decode(&results)) // start to parse IR result
   { 
        if (results.value == IRfront) {
          Serial.println("IRrecv: forward");
          advance(10); //delay time is a must
        }
        
        if (results.value ==  IRback) {
          Serial.println("IRrecv: backward");
          //SpeedTest(30);
          back(10);
          backSlower(10);
        }
        
        if (results.value == IRturnright)
        {
          Serial.println("IRrecv: right");
          right(6);
        }
       
       if (results.value == IRturnleft)
       {
         Serial.println("IRrecv: left");
         left(6);
       }
  
      if (results.value == IRstop) {
         Serial.println("IRrecv: stop");
         digitalWrite(MotorRight1,LOW);
         digitalWrite(MotorRight2,LOW);
         digitalWrite(MotorLeft1,LOW);
         digitalWrite(MotorLeft2,LOW);
      }
      
      
/* cny70 auto running mode BLACK: LOW WHITE: */
#ifdef CN70
     if (results.value == IRcny70)
     {                     
       while(IRcny70)
       {  
         SL = digitalRead(SensorLeft);
         SM = digitalRead(SensorMiddle);
         SR = digitalRead(SensorRight);
                     
         if (SM == HIGH) // middle center at black zone
         { 
            if (SL == LOW & SR == HIGH) // left Black, right White, turn left
            {  
               digitalWrite(MotorRight1,LOW);
               digitalWrite(MotorRight2,HIGH);
               analogWrite(MotorLeft1,0);
               analogWrite(MotorLeft2,80);
            } 
            else if (SR == LOW & SL == HIGH) // turn right
            {  
               analogWrite(MotorRight1,0);
               analogWrite(MotorRight2,80);
               digitalWrite(MotorLeft1,LOW);
               digitalWrite(MotorLeft2,HIGH);
            }
           else // both whites, move forward
            { 
               digitalWrite(MotorRight1,LOW);
               digitalWrite(MotorRight2,HIGH);
               digitalWrite(MotorLeft1,LOW);
               digitalWrite(MotorLeft2,HIGH);
               analogWrite(MotorLeft1,200);
               analogWrite(MotorLeft2,200);
               analogWrite(MotorRight1,200);
               analogWrite(MotorRight2,200);
           }      
        } 
        else // middle sensor at white zone
        {  
           if (SL == LOW & SR == HIGH)// L:black, R:white, sharp left turn 
          {  
              digitalWrite(MotorRight1,LOW);
              digitalWrite(MotorRight2,HIGH);
              digitalWrite(MotorLeft1,LOW);
              digitalWrite(MotorLeft2,LOW);
          }
           else if (SR == LOW & SL == HIGH) // L:white, R:black, sharp right turn
          {  
             digitalWrite(MotorRight1,LOW);
             digitalWrite(MotorRight2,LOW);
             digitalWrite(MotorLeft1,LOW);
             digitalWrite(MotorLeft2,HIGH);
          }
           else // all whites, stop
          {    
          digitalWrite(MotorRight1,HIGH);
          digitalWrite(MotorRight2,LOW);
          digitalWrite(MotorLeft1,HIGH);
          digitalWrite(MotorLeft2,LOW);;
          }
        }
         if (irrecv.decode(&results))
         {
               irrecv.resume(); 
                    Serial.println(results.value,HEX);
               if(results.value ==IRstop)
               { 
                 digitalWrite(MotorRight1,HIGH);
                 digitalWrite(MotorRight2,HIGH);
                 digitalWrite(MotorLeft1,HIGH);
                 digitalWrite(MotorLeft2,HIGH);
                 break;
               }
         }
       }
        results.value=0;
     }
#endif     
#ifdef USE_SONAR
   if (results.value ==IRAutorun ) {
     
          while(IRAutorun) {
            
              myservo.write(90); // server to go back 
              detection(); 
              
              // sonar face is ahead?
              if(directionn == 8){ 
                
                  if (irrecv.decode(&results))
                  {
                     irrecv.resume(); 
                     Serial.println(results.value,HEX);
                     if(results.value ==IRstop)
                     { 
                       digitalWrite(MotorRight1,LOW); 
                       digitalWrite(MotorRight2,LOW);
                       digitalWrite(MotorLeft1,LOW);
                       digitalWrite(MotorLeft2,LOW);
                       break;
                     }
                 }
                    results.value=0;
                    advance(1); // 正常前進 
                    Serial.print(" Advance "); //顯示方向(前進)
                    Serial.print(" "); 
              }
              
             // move backward
             if(directionn == 2) {
                  if (irrecv.decode(&results))
                  {
                   irrecv.resume(); 
                   Serial.println(results.value,HEX);
                   if(results.value ==IRstop)
                   { 
                     digitalWrite(MotorRight1,LOW); 
                     digitalWrite(MotorRight2,LOW);
                     digitalWrite(MotorLeft1,LOW);
                     digitalWrite(MotorLeft2,LOW);
                     break;
                   }
                 }
                  results.value=0;
                  back(8); // move backward
                  turnL(3); // slightly turn left to avoid stuck
                  Serial.print("sonar: Reverse ");
            }
            
            // turn right
            if(directionn == 6) {
             if (irrecv.decode(&results)) {
                  irrecv.resume(); 
                  Serial.println(results.value,HEX);
                 
                 if(results.value ==IRstop) { 
                   digitalWrite(MotorRight1,LOW); 
                   digitalWrite(MotorRight2,LOW);
                   digitalWrite(MotorLeft1,LOW);
                   digitalWrite(MotorLeft2,LOW);
                   break;
                 }
             }
               results.value=0;
               back(1); 
               turnR(6); //turn right
               Serial.print("sonar: Right "); 
            }
            
            // turn left
            if(directionn == 4){
              
                 if (irrecv.decode(&results)) {
                     irrecv.resume(); 
                     Serial.println(results.value,HEX);
                     
                     if(results.value ==IRstop) { 
                       digitalWrite(MotorRight1,LOW); 
                       digitalWrite(MotorRight2,LOW);
                       digitalWrite(MotorLeft1,LOW);
                       digitalWrite(MotorLeft2,LOW);
                       break;
                     }
                }
                
                results.value=0;
                back(1); 
                turnL(6); // 左轉
                Serial.print(" Left "); //顯示方向(右轉) 
             } 
              
           if (irrecv.decode(&results)) {
               irrecv.resume(); 
               Serial.println(results.value,HEX);
               
               if(results.value ==IRstop) { 
                 digitalWrite(MotorRight1,LOW); 
                 digitalWrite(MotorRight2,LOW);
                 digitalWrite(MotorLeft1,LOW);
                 digitalWrite(MotorLeft2,LOW);
                 break;
               }
            }
            
          }// end of: while
           
           results.value=0;
         }
  
#else
      else {        
           digitalWrite(MotorRight1,LOW);
           digitalWrite(MotorRight2,LOW);
           digitalWrite(MotorLeft1,LOW);
           digitalWrite(MotorLeft2,LOW);
       }
#endif
          irrecv.resume(); //set IR as a idle state     
     } // end of: start to parse IR result 
}//end of: loop
   

