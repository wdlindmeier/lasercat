/*
 * IRRemote ]
 *     NOTE: IR pin must be pin 3
 */
#include <IRremote.h>        

enum {
  TokenIdxLeft = 0,
  TokenIdxRight,
//  TokenIdxSpeed,
  NumTokenIdxs
};

IRsend irsend;                           //

void setup()
{
  Serial.begin(9600);
}

void loop()
{

  if(Serial.available()){
    
    int MAX_CHAR = 100;
    char readVal[MAX_CHAR];
    int numChar = Serial.readBytesUntil('\n', readVal, MAX_CHAR);
    String inVal = String(readVal).substring(0,numChar);
    
    if(inVal.indexOf(',') != -1){ // Steering directions
      
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

        long lVal = (long)tokInts[TokenIdxLeft];
        long rVal = (long)tokInts[TokenIdxRight];

        if(lVal > 0 && rVal > 0 && lVal < 1000 && rVal < 1000){
        
          /*
          Serial.print("lVal: ");
          Serial.print(lVal);
          Serial.print(" rVal: ");
          Serial.println(rVal);          
          */
          
          Serial.print("lVal: ");
          Serial.println(lVal);
          Serial.print("rVal: ");
          Serial.println(rVal);
          
          long totalVal = (lVal*(long)1000)+rVal;
//          Serial.println(totalVal);
          irsend.sendNEC(totalVal, 32);
          
        }

      }      
            
    }
    
  }
}

