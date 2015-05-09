#include <SoftEasyTransfer.h> 
#include <SoftwareSerial.h>

// Constants
#define socket1FailLed 3
#define socket2FailLed 4
#define socket3FailLed 5
#define module1TubesSw1 6
#define module1TubesSw2 7
#define module1TubesSw3 8
#define module1TubesSw4 9
#define module1TubesSw5 10
#define txPin 11
#define rxPin 12
#define DIR 13
#define voltageMeasureReader1 A1
#define voltageMeasureReader2 A2
#define voltageMeasureReader3 A3

SoftEasyTransfer ET; 


struct SEND_DATA_STRUCTURE{
  
  byte ID;
  float measure1;
  float measure2;
  float measure3;
  
 };
 
SEND_DATA_STRUCTURE measureData;

// Variables & arrays

//Module ID
const byte ID = 1;
 
SoftwareSerial RS485 (rxPin, txPin);
 
void setup(){
  
  //Serial.begin(9600);
 
  RS485.begin(9600);
  
  ET.begin(details(measureData), &RS485);
  
  // RS485 data recive
  digitalWrite(DIR, 0);

  //Debug start
  Serial.println("===========[ SLAVE 1 START ]=============");
  //Debug end 
}

void loop(){
  
 if (RS485.available() >= 2) {

    //Debug start
    int dataCount = 1;
    //Debug end
   
    //Read first byte
    byte id = RS485.read();
   
    //Debug start         
    //Serial.println("=========== RECEIVE ID FROM MASTER =============");
    Serial.print("Count: ");
    Serial.println(dataCount);
    Serial.print("ID = ");
    Serial.println(id);
    //Debug end
    
    if (id == ID){
     
      byte check = RS485.read();
      byte module1Sw = RS485.read();
      delay(10);
      
      //Debug start     
      Serial.println("=========== RECEIVE DATA FROM MASTER =============");
      Serial.print("Count: ");
      Serial.println(dataCount);
      Serial.print("CHECK = ");
      Serial.println(check);
      //Debug end
     
     //Send data to master for check
     if (check == 9){

      //Debug start     
      Serial.println("=========== RECEIVE DATA FROM MASTER =============");
      Serial.print("Count: ");
      Serial.println(dataCount);
      Serial.print("CHECK = ");
      Serial.println(check);
      Serial.print("module1Sw = ");
      Serial.println(module1Sw);
      //Debug end
      
      //Debug start     
      Serial.println("=========== SEND DATA TO MASTER =============");
      Serial.print("Count: ");
      Serial.println(dataCount);
      Serial.print("ID = ");
      Serial.println(check);
      Serial.print("module1Sw = ");
      Serial.println(module1Sw);
      //Debug end
       
       digitalWrite(DIR, 1);
     
       RS485.write(ID);
       RS485.write(module1Sw);
         
       digitalWrite(DIR, 0);
       
     }
     
     if (check == 8){
       //Debug start
       Serial.println("=========== RECEIVE CHECK FROM MASTER =============");
       Serial.print("Count: ");
       Serial.println(dataCount);
       //Debug end
       
       switch (module1Sw) {
         case 1:
           //Debug
           digitalWrite(socket1FailLed, 1);
           delay(20000);
           
           measureData.ID = 1;
           measureData.measure1 = 0.41;
           
           digitalWrite(DIR, 1);
     
           ET.sendData();
             
           digitalWrite(DIR, 0);
           break;
           
         case 2:
           
           digitalWrite(socket2FailLed, 1);
           break;
           
         case 3:
           
           digitalWrite(socket3FailLed, 1);
           break;
         
       }
       
     }
     
    }     
    else RS485.flush();
    //Debug start
    Serial.println("===========[ DATA COUNT++ ]=============");
    dataCount++;
    //Debug end
   }

}

