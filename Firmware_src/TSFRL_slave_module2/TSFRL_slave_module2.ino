/*
Name: TSFRL_slave_module2.ino
Description: Test Stand For Radio Lamps project. Slave module source code.
Author: Rustam Ojukas
Date: 13.05.2015
Github: https://github.com/rustamojukas/TSFRL-project
*/

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

int second = 7140;//DEBUG
byte proceedTimer = 0;
byte measureStart = 0;

//Module ID
const byte ID = 1;
 
SoftwareSerial RS485 (rxPin, txPin);
 
void setup(){
Serial.begin(9600);//Debug
        Serial.print("Start(setup();) moduleMeasuredData[0]: ");//Debug
        Serial.println(moduleMeasuredData[0]);//Debug 
        Serial.print("moduleMeasuredData[1]: ");//Debug
        Serial.println(moduleMeasuredData[1]);//Debug 
        Serial.print("moduleMeasuredData[2]: ");//Debug
        Serial.println(moduleMeasuredData[2]);//Debug 
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

float voltmeter(int readPin){

  float result = 0.0;
  
  for (byte i = 0; i < 3; i++){
    
    float volts = ((analogRead(readPin)-4.1) * 5.0) / 1023.0;

    result += volts;
    delay(100);

  }
  
  return result/3.0;
  
}

void loop(){
  
  //Start action 
  if (RS485.available() >= 2) {
   
    //Read first byte
    byte id = RS485.read();
    delay(10);

    if (id == ID){
      Serial.print("From Master ID: ");//Debug
      Serial.println(id);//Debug
      byte check = RS485.read();
      byte moduleSw = RS485.read();
      delay(10);
      Serial.print("From Master check: ");//Debug
      Serial.println(check);//Debug
      Serial.print("From Master moduleSw: ");//Debug
      Serial.println(moduleSw);//Debug
      //Send data to master for check
      if (check == 9){
       
        //Delay 2 second before send
        delay(2000);
        Serial.print("check = 9; To Master ID: ");//Debug
        Serial.println(id);//Debug
        Serial.print("To Master moduleSw: ");//Debug
        Serial.println(moduleSw);//Debug      
        digitalWrite(DIR, 1);
       
        RS485.write(ID);
        RS485.write(moduleSw);
           
        digitalWrite(DIR, 0);
       
      }
     
      if (check == 8){
        Serial.print("check = 8; From Master ID: ");//Debug
        Serial.println(id);//Debug
        Serial.print("From Master moduleSw: ");//Debug
        Serial.println(moduleSw);//Debug       
        switch (moduleSw) {

          case 1:

            digitalWrite(moduleTubesSw1, 1);
            proceedTimer = 1;
  
            break;
           
          case 2:
           
            digitalWrite(moduleTubesSw2, 1);
            proceedTimer = 1;
           
            break;
           
          case 3:
           
            digitalWrite(moduleTubesSw3, 1);
            proceedTimer = 1;
             
            break;
                 
          case 4:
           
            digitalWrite(moduleTubesSw4, 1);
            proceedTimer = 1;
           
            break;

          case 5:
           
            digitalWrite(moduleTubesSw5, 1);
            proceedTimer = 1;
           
            break;
         
        }
       
      }

      if (check == 7){
        Serial.print("check = 7; From Master ID: ");//Debug
        Serial.println(id);//Debug 
        Serial.print("From Master moduleSw: ");//Debug
        Serial.println(moduleSw);//Debug          
        switch (moduleSw) {

          case 1:

            digitalWrite(moduleTubesSw1, 0);
            delay(10);
           
            measureData.ID = ID;
            measureData.moduleSw = moduleSw;
            measureData.measure1 = moduleMeasuredData[0];
            measureData.measure2 = moduleMeasuredData[1];
            measureData.measure3 = moduleMeasuredData[2];
        Serial.print("Send to Master measureData.measure1: ");//Debug
        Serial.println(measureData.measure1);//Debug 
        Serial.print("measureData.measure2: ");//Debug
        Serial.println(measureData.measure2);//Debug 
        Serial.print("moduleMeasuredData[2]: ");//Debug
        Serial.println(measureData.measure3);//Debug            
            //Dely 2 second before send
            delay(2000);
           
            digitalWrite(DIR, 1);
     
            ET.sendData();
             
            digitalWrite(DIR, 0);
           
            break;
           
          case 2:
           
            digitalWrite(moduleTubesSw2, 0);
            delay(10);
           
            measureData.ID = ID;
            measureData.moduleSw = moduleSw;
            measureData.measure1 = moduleMeasuredData[0];
            measureData.measure2 = moduleMeasuredData[1];
            measureData.measure3 = moduleMeasuredData[2];

            //Delay 2 second before send
            delay(2000);
           
            digitalWrite(DIR, 1);
     
            ET.sendData();
             
            digitalWrite(DIR, 0);
           
            break;
           
          case 3:
           
            digitalWrite(moduleTubesSw3, 0);
            delay(10);
           
            measureData.ID = ID;
            measureData.moduleSw = moduleSw;
            measureData.measure1 = moduleMeasuredData[0];
            measureData.measure2 = moduleMeasuredData[1];
            measureData.measure3 = moduleMeasuredData[2];

            //Delay 2 second before send
            delay(2000);
                    
            digitalWrite(DIR, 1);
     
            ET.sendData();
             
            digitalWrite(DIR, 0);
           
            break;
                 
          case 4:
           
            digitalWrite(moduleTubesSw4, 0);
            delay(10);
           
            measureData.ID = ID;
            measureData.moduleSw = moduleSw;
            measureData.measure1 = moduleMeasuredData[0];
            measureData.measure2 = moduleMeasuredData[1];
            measureData.measure3 = moduleMeasuredData[2];

            //Delay 2 second before send
            delay(2000);
                    
            digitalWrite(DIR, 1);
     
            ET.sendData();
             
            digitalWrite(DIR, 0);
           
            break;

          case 5:
           
            digitalWrite(moduleTubesSw5, 0);
            delay(10);
           
            measureData.ID = ID;
            measureData.moduleSw = moduleSw;
            measureData.measure1 = moduleMeasuredData[0];
            measureData.measure2 = moduleMeasuredData[1];
            measureData.measure3 = moduleMeasuredData[2];

            //Delay 2 second before send
            delay(2000);
                    
            digitalWrite(DIR, 1);
     
            ET.sendData();
             
            digitalWrite(DIR, 0);
           
            break;         

        }
       
      }

    }     
    else RS485.flush();
    
  }
  //End start action
   
  //Timer start
  while(proceedTimer){
    
    timer();
    
  }
  //End timer

  //Measure start
  while(measureStart){
    
    moduleMeasuredData[0] = voltmeter(voltageMeasureReader1);
    delay(100);
     
    moduleMeasuredData[1] = voltmeter(voltageMeasureReader2);
    delay(100);

    moduleMeasuredData[2] = voltmeter(voltageMeasureReader3);
    delay(100);
     
    measureStart = 0;
        Serial.print("measureStart loop moduleMeasuredData[0]: ");//Debug
        Serial.println(moduleMeasuredData[0]);//Debug 
        Serial.print("moduleMeasuredData[1]: ");//Debug
        Serial.println(moduleMeasuredData[1]);//Debug 
        Serial.print("moduleMeasuredData[2]: ");//Debug
        Serial.println(moduleMeasuredData[2]);//Debug      
  }
  //End measure

}

