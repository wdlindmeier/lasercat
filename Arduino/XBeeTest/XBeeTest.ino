#define LED_RCV  9

boolean didEstablishContact = false;

void setup()
{
  Serial.begin(9600);
  pinMode(LED_RCV, OUTPUT);   
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.write('5');   // send a starting message
    delay(300);
  }
  didEstablishContact = true;
}

void loop()
{

  digitalWrite(LED_RCV, LOW);
  
  if(Serial.available()){
    
    int incomingByte = -1;

    while(Serial.available()){
      incomingByte = Serial.read();
    }

    if(incomingByte == 54){ // '6'
//    if(incomingByte == '6'){

      digitalWrite(LED_RCV, HIGH);  
      delay(600);

      Serial.write('5');
      
      digitalWrite(LED_RCV, LOW);            
      delay(600);
      
    }
    
  }else if(didEstablishContact){
    
    // Send out a ping
    // Serial.write('5');   // send a starting message
    // delay(300);

  }else{
    
    establishContact();
    
  }
  
}
