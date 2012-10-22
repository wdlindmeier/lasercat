
int LEDval;
int ULval;
int URval;
int LLval;
int LRval;
#define UL  6
#define UR  9
#define LR  10
#define LL  11

enum {
  TokenIdxLeft = 0,
  TokenIdxRight,
  TokenIdxSpeed,
  NumTokenIdxs
};

void setup()
{
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(UL, OUTPUT);
  pinMode(UR, OUTPUT);
  pinMode(LL, OUTPUT);
  pinMode(LR, OUTPUT);

  LEDval = LOW;
  ULval = 255;
  URval = 255;
  LLval = 255;
  LRval = 255;
//  establishContact();
}

/*
void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("!");   // send a starting message
    delay(300);
  }
}
*/

void loop()
{
  digitalWrite(13, LEDval);
  analogWrite(UL, ULval);
  analogWrite(UR, URval);
  analogWrite(LL, LLval);
  analogWrite(LR, LRval);

  int analogIn = analogRead(A0) / 4;
  
  if(Serial.available()){
    
    int MAX_CHAR = 100;
    char readVal[MAX_CHAR];
    int numChar = Serial.readBytesUntil('\n', readVal, MAX_CHAR);
    String inVal = String(readVal).substring(0,numChar);
    
    if(inVal.indexOf(',') != -1){
      char tokens[numChar];
      inVal.toCharArray(tokens, numChar);
      char *token;
      char *ts=tokens;
      int tkIdx=0;
      int tokInts[NumTokenIdxs];
      while ((token = strtok_r(ts, ",", &ts)) != NULL){

        if(tkIdx<NumTokenIdxs){
          tokInts[tkIdx] = atol(token);
        }
        tkIdx++;
      }
      
      if(tkIdx == NumTokenIdxs){

        if(tokInts[TokenIdxLeft] > 0){
          ULval = tokInts[TokenIdxLeft];
          LLval = 0;
        }else{          
          LLval = abs(tokInts[TokenIdxLeft]);
          ULval = 0;
        }

        
        if(tokInts[TokenIdxRight] > 0){
          URval = tokInts[TokenIdxRight];
          LRval = 0;
        }else{          
          LRval = abs(tokInts[TokenIdxRight]);
          URval = 0;
        }

      }

    }else{

      if(inVal == "hotdog"){  
        LEDval = HIGH;
      }else if(inVal == "hamburger"){
        LEDval = LOW;
      }
      
    }
  }
}

