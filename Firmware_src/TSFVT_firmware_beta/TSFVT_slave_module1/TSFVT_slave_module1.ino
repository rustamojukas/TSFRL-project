/*
*  Name: TSFVT_slave_module.ino
*  Description: Test Stand For Vacuum Tube project. Slave module source code.
*  Required: SoftEasyTransfer library (https://github.com/madsci1016/Arduino-EasyTransfer).
*  Author: Rustam Ojukas
*  Date: 13.05.2015
*  Github: https://github.com/rustamojukas/TSFVT-project
*/

#include <SoftEasyTransfer.h> 
#include <SoftwareSerial.h>

// Constants
const byte ID = 1;//Module ID

#define socket1FailLed A0
#define socket2FailLed 2
#define socket3FailLed 3
#define socket4FailLed 4
#define moduleTubesSw0 5
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
#define voltageMeasureReader4 A4

SoftEasyTransfer ET; 

struct SEND_DATA_STRUCTURE{
  
  byte ID;
  byte moduleSw;
  float measure1;
  float measure2;
  float measure3;
  float measure4;
  byte errorSw;
  
 };
 
SEND_DATA_STRUCTURE measureData;

// Variables & arrays
float moduleMeasuredData[] = {0.0, 0.0, 0.0, 0.0};

int second = 120;// 2 hours = 7200 seconds
float correction = 4.1;
int sendDelay = ID * 1000;
byte proceedTimer = 0;
byte measureStart = 0;
byte errorSwActive = 0;
byte moduleSwValue = 0;
 
SoftwareSerial RS485 (rxPin, txPin);

//Setup 
void setup(){
  //DEBUG
  Serial.begin(9600);
  Serial.println("-----------------------Slave setup() Start----------------------");
  
  pinMode(moduleTubesSw0, OUTPUT);
  pinMode(moduleTubesSw1, OUTPUT);
  pinMode(moduleTubesSw2, OUTPUT);
  pinMode(moduleTubesSw3, OUTPUT);
  pinMode(moduleTubesSw4, OUTPUT);
  pinMode(moduleTubesSw5, OUTPUT);
  pinMode(socket1FailLed, OUTPUT);
  pinMode(socket2FailLed, OUTPUT);
  pinMode(socket3FailLed, OUTPUT);
  pinMode(socket4FailLed, OUTPUT);
  
  //digitalWrite(moduleTubesSw0, 1);
 
  RS485.begin(9600);
  
  ET.begin(details(measureData), &RS485);
  
  delay(1500);
  
  digitalWrite(socket1FailLed, 1);
  digitalWrite(socket2FailLed, 1);
  digitalWrite(socket3FailLed, 1);
  digitalWrite(socket4FailLed, 1);
  digitalWrite(DIR, 1);
  
  delay(2500);

  digitalWrite(socket1FailLed, 0);
  digitalWrite(socket2FailLed, 0);
  digitalWrite(socket3FailLed, 0);
  digitalWrite(socket4FailLed, 0);
  digitalWrite(DIR, 0);

}

//Functions start
void timer(){

  if (second == 0){
    
    proceedTimer = 0;
    measureStart = 1;

  }

  delay(1000);
  second--;

}

float voltmeter(int readPin){

  float result = 0.0;
  
  for (byte i = 0; i < 3; i++){
    
    float volts = ((analogRead(readPin)-correction) * 5.0) / 1023.0;

    result += volts;
    delay(100);

  }
  
  return result/3.0;
  
}
//End functions

//Loop
void loop(){
  
  //Start action 
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
       
        //Delay before send
        delay(sendDelay);
              
        digitalWrite(DIR, 1);
       
        RS485.write(ID);
        RS485.write(moduleSw);
           
        digitalWrite(DIR, 0);
       
      }
     
      if (check == 8){
               
        switch (moduleSw) {

          case 1:

            //digitalWrite(moduleTubesSw0, 0);
            //digitalWrite(moduleTubesSw1, 1);
            moduleSwValue = moduleSw;
            proceedTimer = 1;
  
            break;
         
        }
       
      }

      if (check == 7){
                  
        switch (moduleSw) {

          case 1:

            //digitalWrite(moduleTubesSw1, 0);
            //delay(10);
    //DEBUG
    Serial.println("-----------------------Send data to master----------------------");           
            measureData.ID = ID;
            measureData.moduleSw = moduleSw;
            measureData.measure1 = moduleMeasuredData[0];
            measureData.measure2 = moduleMeasuredData[1];
            measureData.measure3 = moduleMeasuredData[2];
            measureData.measure4 = moduleMeasuredData[3];
            measureData.errorSw = errorSwActive;
                    
            //Dely before send
            delay(sendDelay);
           
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
    //DEBUG
    Serial.println("-----------------------Timer Start----------------------");
    
    timer();
    
  }
  //End timer

  //Measure start
  while(measureStart){
    //DEBUG
    Serial.println("-----------------------Measure Start----------------------");
    
    //MeasureReader1 in process
    moduleMeasuredData[0] = (voltmeter(voltageMeasureReader1) + 0.04) * 100.0;
    //DEBUG
    Serial.println("-----------------------Reader1----------------------");
    Serial.println(voltmeter(voltageMeasureReader1) + 0.04);
    Serial.println("-----------------------Array[0]----------------------");
    Serial.println(moduleMeasuredData[0]);
    Serial.println("-----------------------//Array[0]----------------------");
    switch (moduleSwValue) {

      case 1:
      
        if (moduleMeasuredData[0] < 15.0 || moduleMeasuredData[0] > 20.0){
        
          digitalWrite(socket1FailLed, 1);
          errorSwActive = 1;
        
        }
        
        break;
     
    }
    
    delay(100);
    
    //MeasureReader2 in process
    moduleMeasuredData[1] = (voltmeter(voltageMeasureReader2) + 0.04) * 100.0;
    //DEBUG
    Serial.println("-----------------------Reader2----------------------");
    Serial.println(voltmeter(voltageMeasureReader1) + 0.04);
    Serial.println("-----------------------Array[1]----------------------");
    Serial.println(moduleMeasuredData[1]);
    Serial.println("-----------------------//Array[1]----------------------");
     
    switch (moduleSwValue) {

      case 1:
      
        if (moduleMeasuredData[1] < 15.0 || moduleMeasuredData[1] > 20.0){
        
          digitalWrite(socket2FailLed, 1);
          errorSwActive = 1;
        
        }
        
        break;
     
    }

    delay(100);
    
    //MeasureReader3 in process
    moduleMeasuredData[2] = (voltmeter(voltageMeasureReader3) + 0.04) * 100.0;
    //DEBUG
    Serial.println("-----------------------Reader3----------------------");
    Serial.println(voltmeter(voltageMeasureReader1) + 0.04);
    Serial.println("-----------------------Array[2]----------------------");
    Serial.println(moduleMeasuredData[2]);
    Serial.println("-----------------------//Array[2]----------------------");
    switch (moduleSwValue) {

      case 1:
      
        if (moduleMeasuredData[2] < 15.0 || moduleMeasuredData[2] > 20.0){
        
          digitalWrite(socket3FailLed, 1);
          errorSwActive = 1;
        
        }
        
        break;
     
    }

    delay(100);

    //MeasureReader4 in process
    moduleMeasuredData[3] = (voltmeter(voltageMeasureReader4) + 0.04) * 100.0;
    //DEBUG
    Serial.println("-----------------------Reader4----------------------");
    Serial.println(voltmeter(voltageMeasureReader1) + 0.04);
    Serial.println("-----------------------Array[3]----------------------");
    Serial.println(moduleMeasuredData[3]);
    Serial.println("-----------------------//Array[3]----------------------");
    switch (moduleSwValue) {

      case 1:
      
        if (moduleMeasuredData[3] < 15.0 || moduleMeasuredData[3] > 20.0){
        
          digitalWrite(socket4FailLed, 1);
          errorSwActive = 1;
        
        }
        
        break;
     
    }

    delay(100);
     
    measureStart = 0;
              
  }
  //End measure

}
