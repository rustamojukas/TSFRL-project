#include <SoftEasyTransfer.h> 
#include <SoftwareSerial.h>

// Constants
#define socket1FailLed 3
#define socket2FailLed 4
#define socket3FailLed 5
#define moduleTubesSw1 6
#define moduleTubesSw2 7
#define moduleTubesSw3 8
#define moduleTubesSw4 9
#define moduleTubesSw5 10
#define txPin 11
#define rxPin 12
#define DIR 13
#define voltageMeasureReader1 A1
#define voltageMeasureReader2 A2
#define voltageMeasureReader3 A3

SoftEasyTransfer ET; 


struct SEND_DATA_STRUCTURE{
  
  byte ID;
  byte moduleSw;
  float measure1;
  float measure2;
  float measure3;
  
 };
 
SEND_DATA_STRUCTURE measureData;

// Variables & arrays
float moduleMeasuredData[] = {0.0, 0.0, 0.0};

int second = 7190;//DEBUG
byte proceedTimer = 0;
byte measureStart = 0;

//Module ID
const byte ID = 1;
 
SoftwareSerial RS485 (rxPin, txPin);
 
void setup(){
 
  RS485.begin(9600);
  
  ET.begin(details(measureData), &RS485);

}

void timer(){

  int currentHour = (second/60)/60;
  int currentMinute = (second/60)%60;
  int currentSecond = second%60;
  
  //Timer 2:00:00 is stop
  if (currentHour == 2 && currentMinute == 0 && currentSecond == 0){
    
    proceedTimer = 0;
    measureStart = 1;

  }

  delay(1000);
  second++;

}

void loop(){
  
 if (RS485.available() >= 2) {
   
    //Read first byte
    byte id = RS485.read();
    delay(10);

    if (id == ID){

      byte check = RS485.read();
      byte moduleSw = RS485.read();
      delay(10);
     
     //Send data to master for check
     if (check == 9){
       
       digitalWrite(DIR, 1);
     
       RS485.write(ID);
       RS485.write(moduleSw);
         
       digitalWrite(DIR, 0);
       
     }
     
     if (check == 8){
       
       switch (moduleSw) {
         case 1:
           //Debug
           digitalWrite(moduleTubesSw1, 1);
           proceedTimer = 1;

           break;
           
         case 2:
           
           digitalWrite(moduleTubesSw2, 1);
           break;
           
         case 3:
           
           digitalWrite(moduleTubesSw3, 1);
           break;
                 
         case 4:
           
           digitalWrite(moduleTubesSw4, 1);
           break;

         case 5:
           
           digitalWrite(moduleTubesSw5, 1);
           break;
         
       }
       
     }

     if (check == 7){
       
       switch (moduleSw) {
         case 1:
           //Debug
           digitalWrite(moduleTubesSw1, 0);
           delay(5000);
           
           measureData.ID = 1;
           measureData.moduleSw = moduleSw;
           measureData.measure1 = 0.41;
           
           digitalWrite(DIR, 1);
     
           ET.sendData();
             
           digitalWrite(DIR, 0);
           break;
           
         case 2:
           
           digitalWrite(moduleTubesSw2, 0);
           break;
           
         case 3:
           
           digitalWrite(moduleTubesSw3, 0);
           break;
                 
         case 4:
           
           digitalWrite(moduleTubesSw4, 0);
           break;

         case 5:
           
           digitalWrite(moduleTubesSw5, 0);
           break;         
       }
       
     }


    }     
    else RS485.flush();
    
   }
   //End start action
   
   //Show timer start
   while(proceedTimer){
    
     timer();
    
   }
   //End show timer

   //Measure start
   while(measureStart){
    
     //Action;
    
   }
   //End measure

}

