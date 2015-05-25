/*
*  Name: TSFVT_master_module.ino
*  Description: Test Stand For Vacuum Tube project. Slave module source code.
*  Required: SoftEasyTransfer library (https://github.com/madsci1016/Arduino-EasyTransfer).
*  Author: Rustam Ojukas
*  Date: 13.05.2015
*  Github: https://github.com/rustamojukas/TSFVT-project
*/

//Include libraries
#include <SoftEasyTransfer.h> 
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Constants

#define keyStart 3
#define keyDown 4
#define keyBack 5
#define keySelect 6
#define keyUp 7
#define testLed 8
#define doneLed 9
#define errorLed 10
#define txPin 11
#define rxPin 12
#define DIR 13

//Set Software serial pins
SoftwareSerial RS485 (rxPin, txPin); // RX, TX

//Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

//SoftEasyTransfer setup
SoftEasyTransfer ET;

struct RECEIVE_DATA_STRUCTURE{
  
  byte ID;
  byte moduleSw;
  int measure1;
  int measure2;
  int measure3;
  int measure4;

};

RECEIVE_DATA_STRUCTURE measureData;

const char* modulesMenu[] = {"Module 1", "Module 2", "Module 3"};
const char* module1TubesName[] = {"M1 Lamp 1", "M1 Lamp 2", "M1 Lamp 3", "M1 Lamp 4", "M1 Lamp 5"};
const char* module2TubesName[] = {"M2 Lamp 1", "M2 Lamp 2", "M2 Lamp 3", "M2 Lamp 4", "M2 Lamp 5"};
const char* module3TubesName[] = {"M3 Lamp 1", "M3 Lamp 2", "M3 Lamp 3", "M3 Lamp 4", "M3 Lamp 5"};

// Variables & arrays
byte module1TubesSw[] = {0, 0, 0, 0, 0};
byte module2TubesSw[] = {0, 0, 0, 0, 0};
byte module3TubesSw[] = {0, 0, 0, 0, 0};

int module1MeasuredData[] = {0, 0, 0, 0, 0};
int module2MeasuredData[] = {0, 0, 0, 0, 0};
int module3MeasuredData[] = {0, 0, 0, 0, 0};

byte modulesMenuPos = 0;
byte moduleMenuPos = 0;
byte showMeasuredDataPos = 0;
byte selected = 0;
byte inModuleMenu = 0;

byte module1IsSelected = 0;
byte module2IsSelected = 0;
byte module3IsSelected = 0;
byte module1ForTestOn = 0;
byte module2ForTestOn = 0;
byte module3ForTestOn = 0;
byte module1DataSend = 0;
byte module2DataSend = 0;
byte module3DataSend = 0;
byte module1MeasureDataSend = 0;
byte module2MeasureDataSend = 0;
byte module3MeasureDataSend = 0;

byte defaultDisplay = 0;
byte startKeyActive = 0;
byte startShowCounter = 0;
byte receiveSumm = 0;

//timer
int second = 120;//DEBUG
byte showTimer = 0;
byte testEnd = 0;
byte moduleCounter = 0;
byte savedDataCounter = 0;

byte modulesMenuArrSize = (sizeof(modulesMenu)/sizeof(int));
byte module1MenuArrSize = (sizeof(module1TubesName)/sizeof(int));
byte module2MenuArrSize = (sizeof(module2TubesName)/sizeof(int));
byte module3MenuArrSize = (sizeof(module3TubesName)/sizeof(int));


void setup() {

  RS485.begin(9600);
  
  ET.begin(details(measureData), &RS485);
  
  // initialize the lcd 
  lcd.init();
 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("****************");
  lcd.setCursor (3, 1);
  lcd.print("it-group4you!");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tester, started");
  lcd.setCursor(0, 1);
  lcd.print("Loading...");
  delay(2000);
  lcd.clear();  

  pinMode(keyStart, INPUT);
  pinMode(keyUp, INPUT);
  pinMode(keyDown, INPUT);
  pinMode(keyBack, INPUT);
  pinMode(keySelect, INPUT);
          
}

void timer(){

  int currentHour = (second/60)/60;
  int currentMinute = (second/60)%60;
  int currentSecond = second%60;
  
  if (second == 0){
    
    testEnd = 1;
    showTimer = 0;

  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Test time: ");
  lcd.setCursor(0 , 1);
  lcd.print(currentHour);
  lcd.print(":");    
  if (currentMinute < 10){
    
    lcd.print("0");
    lcd.print(currentMinute);
    
  }else lcd.print(currentMinute);


  lcd.print(":");
  
  if (currentSecond < 10){
    
    lcd.print("0");
    lcd.print(currentSecond);
    
  }else lcd.print(currentSecond);

  delay(1000);
  second--;

}

boolean selectedModule(){

  int selectCounter = 0;
  boolean selStatus = false;
  
    //Module 1
    for (int i = 0; i < module1MenuArrSize; i ++){
    
      if (module1TubesSw[i] == 1){
      
        selectCounter += 1;
        break;
      
      }
    
    }
    delay(10);
    
    //Module 2
    for (int i = 0; i < module2MenuArrSize; i ++){
    
      if (module2TubesSw[i] == 1){
      
        selectCounter += 1;
        break;
      
      }
    
    }
    delay(10);
    
    //Module 3
    for (int i = 0; i < module3MenuArrSize; i ++){
    
      if (module3TubesSw[i] == 1){
      
        selectCounter += 1;
        break;
      
      }
    
    }
    delay(10);
    
    if (selectCounter > 0) selStatus = true;
    return selStatus;

}

boolean debounce(int key){

  boolean debValue = 0;
  boolean current = digitalRead(key);

  if (debValue != current){

    delay(5);
    current = digitalRead(key);

  } 

  return current;

}

void menuModules(boolean key1, boolean key2){
     
    
      
    //Menu move up & down
    if (key2 && modulesMenuPos < (modulesMenuArrSize - 1)) modulesMenuPos++;   
    else if (key1 && modulesMenuPos != 0) modulesMenuPos--;  
 
    lcd.print(modulesMenu[modulesMenuPos]);

}

void loop() { 
  
  //Menu start
  while(!startKeyActive){
      
    //Default display
    if (defaultDisplay){
      
      lcd.clear();
      defaultDisplay = 0;
    
    }
    
    lcd.setCursor(0, 0);
    lcd.print("Menu");
    lcd.setCursor(0, 1);
  
    // keyStart actions
    if (debounce(keyStart)){
      
      if (selectedModule()){
      
        startKeyActive = 1;
      
        //Switch on testLed
        digitalWrite(testLed, 1);
        
        //Switch off errorLed
        digitalWrite(errorLed, 0);
      
      }else{
      
        //Error message: 1 module or more is not selected
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("1 module or more");
        lcd.setCursor(0, 1);
        lcd.print("is NOT selected");
        delay(2000);
        defaultDisplay = 1;
        
      }
      
    }
    
    // keySelect actions
    if (debounce(keySelect)) selected = 1;
    
    // keyBack actions
    if (debounce(keyBack)){
      
      selected = 0;
      moduleMenuPos = 0;
      inModuleMenu = 0;
      defaultDisplay = 1;
      
    }
  
    //Modules menu display
    if (selected == 0) menuModules(debounce(keyUp), debounce(keyDown));
    
    //Module 1 menu display  
    if (modulesMenuPos == 0 && selected == 1){
  
      if (debounce(keyDown) && moduleMenuPos < (module1MenuArrSize - 1)) moduleMenuPos++;   
      else if (debounce(keyUp) && moduleMenuPos != 0) moduleMenuPos--;
    
      lcd.setCursor(0, 0);
      lcd.print(modulesMenu[modulesMenuPos]);
      
      lcd.setCursor(0, 1);
      lcd.print(module1TubesName[moduleMenuPos]);
      
      if (module1TubesSw[moduleMenuPos] == 0) lcd.print(" ");
      else if (module1TubesSw[moduleMenuPos] == 1) lcd.print("*");    
      
      if (debounce(keySelect) && inModuleMenu == 1){
      
        if (module1TubesSw[moduleMenuPos] == 0){

          if(module1IsSelected == 1){
            
            //Error message: Module 1 is selected!
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Module 1 is");
            lcd.setCursor(0, 1);
            lcd.print("selected!");
            delay(2000);
            defaultDisplay = 1;
            
          }else{
            
            module1TubesSw[moduleMenuPos] = 1;
            module1IsSelected = 1;
          
        }

        
        }else if (module1TubesSw[moduleMenuPos] == 1){
          
          module1TubesSw[moduleMenuPos] = 0;
          module1IsSelected = 0; 
        
        }
        
      }
        
      inModuleMenu = 1;
    
    }
    
    //Module 2 menu display
    if (modulesMenuPos == 1 && selected == 1){
      
      if (debounce(keyDown) && moduleMenuPos < (module2MenuArrSize - 1)) moduleMenuPos++;   
      else if (debounce(keyUp) && moduleMenuPos != 0) moduleMenuPos--;
      
      lcd.setCursor(0, 0);
      lcd.print(modulesMenu[modulesMenuPos]);
      
      lcd.setCursor(0, 1);
      lcd.print(module2TubesName[moduleMenuPos]);
      
      if (module2TubesSw[moduleMenuPos] == 0) lcd.print(" ");
      else if (module2TubesSw[moduleMenuPos] == 1) lcd.print("*");    
      
      if (debounce(keySelect) && inModuleMenu == 1){
      
        if (module2TubesSw[moduleMenuPos] == 0){

          if(module2IsSelected == 1){
            
            //Error message: Module 2 is selected!
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Module 2 is");
            lcd.setCursor(0, 1);
            lcd.print("selected!");
            delay(2000);
            defaultDisplay = 1;
            
          }else{
            
            module2TubesSw[moduleMenuPos] = 1;
            module2IsSelected = 1;
          
        }

        
        }else if (module2TubesSw[moduleMenuPos] == 1){
          
          module2TubesSw[moduleMenuPos] = 0;
          module2IsSelected = 0; 
        
        }
          
      }
      
      inModuleMenu = 1;
    
    }
    
    //Module 3 menu display
    if (modulesMenuPos == 2 && selected == 1){
  
      if (debounce(keyDown) && moduleMenuPos < (module3MenuArrSize - 1)) moduleMenuPos++;   
      else if (debounce(keyUp) && moduleMenuPos != 0) moduleMenuPos--;
      
      lcd.setCursor(0, 0);
      lcd.print(modulesMenu[modulesMenuPos]);
      
      lcd.setCursor(0, 1);
      lcd.print(module3TubesName[moduleMenuPos]);
      
      if (module3TubesSw[moduleMenuPos] == 0) lcd.print(" ");
      else if (module3TubesSw[moduleMenuPos] == 1) lcd.print("*");    
      
      if (debounce(keySelect) && inModuleMenu == 1){
      
        if (module3TubesSw[moduleMenuPos] == 0){

          if(module3IsSelected == 1){
            
            //Error message: Module 3 is selected!
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Module 3 is");
            lcd.setCursor(0, 1);
            lcd.print("selected!");
            delay(2000);
            defaultDisplay = 1;
            
          }else{
            
            module3TubesSw[moduleMenuPos] = 1;
            module3IsSelected = 1;
          
        }

        
        }else if (module3TubesSw[moduleMenuPos] == 1){
          
          module3TubesSw[moduleMenuPos] = 0;
          module3IsSelected = 0; 
        
        }
            
      }
      
      inModuleMenu = 1;
    
    }
    
    delay(150);
    
  }
  //End menu start

  //Test start, keyStart pressed  
  if (startKeyActive && startShowCounter == 0 ){
      
    //Module 1 status
    for (int i = 0; i < module1MenuArrSize; i ++){
    
      if (module1TubesSw[i] == 1){
      
        module1ForTestOn = 1;
        break;
      
      }
      
    }
    delay(10);

    //Module 2 status
    for (int i = 0; i < module2MenuArrSize; i ++){
    
      if (module2TubesSw[i] == 1){
      
        module2ForTestOn = 1;
        break;
      
      }
      
    }
    delay(10);

    //Module 3 status
    for (int i = 0; i < module3MenuArrSize; i ++){
    
      if (module3TubesSw[i] == 1){
      
        module3ForTestOn = 1;
        break;
      
      }
      
    }
    delay(10);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("****************");
    lcd.setCursor(0, 1);
    lcd.print("**Test started**");
    
    delay(2000);
    lcd.clear();
    
    //Create Module 1 data package  
    if (module1ForTestOn == 1 && module1DataSend == 0){
      
      //Switch rs485 transmit
      digitalWrite(DIR, 1);
      
      //Module 1 list
      for (int i = 0; i < module1MenuArrSize; i ++){
      
        if (module1TubesSw[i] == 1){
          
          //Send data package to slave 1
          RS485.write(1);
          RS485.write(9);
          RS485.write(i + 1);
          
          //Switch rs485 receive
          digitalWrite(DIR, 0);
          module1DataSend = 1;
          receiveSumm++;
          
        }
      
      }
      
    }
    delay(100);
    //End create Module 1 data package 

    //Create Module 2 data package  
    if (module2ForTestOn == 1 && module2DataSend == 0){
      
      //Switch rs485 transmit
      digitalWrite(DIR, 1);
      
      //Module 1 list
      for (int i = 0; i < module2MenuArrSize; i ++){
      
        if (module2TubesSw[i] == 1){
          
          //Send data package to slave 1
          RS485.write(2);
          RS485.write(9);
          RS485.write(i + 1);
          
          //Switch rs485 receive
          digitalWrite(DIR, 0);
          module2DataSend = 1;
          receiveSumm++;
          
        }
      
      }
      
    }
    delay(100);
    //End create Module 2 data package

    //Create Module 3 data package  
    if (module3ForTestOn == 1 && module3DataSend == 0){
      
      //Switch rs485 transmit
      digitalWrite(DIR, 1);
      
      //Module 1 list
      for (int i = 0; i < module3MenuArrSize; i ++){
      
        if (module3TubesSw[i] == 1){
          
          //Send data package to slave 1
          RS485.write(3);
          RS485.write(9);
          RS485.write(i + 1);
          
          //Switch rs485 receive
          digitalWrite(DIR, 0);
          module3DataSend = 1;
          receiveSumm++;
          
        }
      
      }
      
    }
    delay(100);
    //End create Module 3 data package

    startShowCounter = receiveSumm + 1;
    
  }
  //End test start, keyStart pressed

  //Start send check data
  while(receiveSumm >= 1){

    if (RS485.available() >= 2) {
    
      //Read first byte
      byte id = RS485.read();
      delay(10);    
      
        //Module 1 check
        if (id == 1){
          
          byte moduleSw = RS485.read();
          
          if (module1TubesSw[moduleSw - 1]){
            
            delay(100);
            
            //Switch rs485 transmit
            digitalWrite(DIR, 1);
          
            RS485.write(1);
            RS485.write(8);
            RS485.write(moduleSw);
            
            //Switch rs485 receive
            digitalWrite(DIR, 0);
            receiveSumm--;
            startShowCounter--;
           
          } else{
            
            digitalWrite(errorLed, 1);
            
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("****Module 1****");
            lcd.setCursor(0, 1);
            lcd.print("*****ERROR!*****");
            delay(3000);
            receiveSumm--;
            startShowCounter--;
            
          }
          delay(10); 
          
        }
        //End module 1 check

        //Module 2 check
        if (id == 2){
          
          byte moduleSw = RS485.read();
          
          if (module2TubesSw[moduleSw - 1]){
            
            delay(100);
            
            //Switch rs485 transmit
            digitalWrite(DIR, 1);
          
            RS485.write(2);
            RS485.write(8);
            RS485.write(moduleSw);
            
            //Switch rs485 receive
            digitalWrite(DIR, 0);
            receiveSumm--;
            startShowCounter--;
            
          } else{
            
            digitalWrite(errorLed, 1);
            
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("****Module 2****");
            lcd.setCursor(0, 1);
            lcd.print("*****ERROR!*****");
            delay(3000);
            receiveSumm--;
            startShowCounter--;
            
          }
          delay(10); 
                    
        }
        //End module 2 check

        //Module 3 check
        if (id == 3){
          
          byte moduleSw = RS485.read();
          
          if (module3TubesSw[moduleSw - 1]){
            
            delay(100);
            
            //Switch rs485 transmit
            digitalWrite(DIR, 1);
          
            RS485.write(3);
            RS485.write(8);
            RS485.write(moduleSw);
            
            //Switch rs485 receive
            digitalWrite(DIR, 0);
            receiveSumm--;
            startShowCounter--;
            
          } else{
            
            digitalWrite(errorLed, 1);
            
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("****Module 3****");
            lcd.setCursor(0, 1);
            lcd.print("*****ERROR!*****");
            delay(3000);
            receiveSumm--;
            startShowCounter--;
            
          } 
          delay(10);
          
        }
        //End module 3 check
    
    }  
  
  }
  //End send check data
  
  //Show tested modules
  while(startShowCounter >= 1 && startShowCounter < 4){
                  
    //Displayed if Module 1 is on  
    if (module1ForTestOn){
        
      lcd.setCursor(0, 0);
      lcd.print("*Module 1 test*");
      
      //Module 1 list
      for (int i = 0; i < module1MenuArrSize; i ++){
      
        if (module1TubesSw[i] == 1){
                
          lcd.setCursor(0, 1);
          lcd.print("                ");
          delay(750);
          lcd.setCursor(0, 1);
          lcd.print(module1TubesName[i]);
          delay(1250);
        
        }
      
      }
      
    }

    //Displayed if Module 2 is on
    if (module2ForTestOn){
        
      lcd.setCursor(0, 0);
      lcd.print("*Module 2 test*");
      
      //Module 2 list
      for (int i = 0; i < module2MenuArrSize; i ++){
      
        if (module2TubesSw[i] == 1){
                
          lcd.setCursor(0, 1);
          lcd.print("                ");
          delay(750);
          lcd.setCursor(0, 1);
          lcd.print(module2TubesName[i]);
          delay(1250);
        
        }
      
      }
      
    }

    //Displayed if Module 3 is on
    if (module3ForTestOn){
        
      lcd.setCursor(0, 0);
      lcd.print("*Module 3 test*");
      
      //Module 3 list
      for (int i = 0; i < module3MenuArrSize; i ++){
      
        if (module3TubesSw[i] == 1){
                
          lcd.setCursor(0, 1);
          lcd.print("                ");
          delay(750);
          lcd.setCursor(0, 1);
          lcd.print(module3TubesName[i]);
          delay(1250);
        
        }
      
      }
      
    }    
    
    startShowCounter++;    
    delay(1000);

    if (startShowCounter == 3) showTimer = 1;

  }
  //End show tested modules
  
  //Show timer start
  while(showTimer){
  
    timer();
  
  }
  //End show timer
  
  //Test end
  if (testEnd){
    
    digitalWrite(testLed, 0);
    digitalWrite(doneLed, 1);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("****************");
    lcd.setCursor(0, 1);
    lcd.print("***Test done!***");
    delay(3000);      

    //Create Module 1 data package for measure data  
    if (module1ForTestOn == 1 && module1MeasureDataSend == 0){
      
      //Switch rs485 transmit
      digitalWrite(DIR, 1);
      
      //Module 1 list
      for (int i = 0; i < module1MenuArrSize; i ++){
      
        if (module1TubesSw[i] == 1){
          
          //Send data package to slave 1
          RS485.write(1);
          RS485.write(7);
          RS485.write(i + 1);
          
          //Switch rs485 receive
          digitalWrite(DIR, 0);
          module1MeasureDataSend = 1;
          moduleCounter++;
          
        }
      
      }
      
    }
    delay(100);
    //End create Module 1 data package for measure data
    
    //Create Module 2 data package for measure data  
    if (module2ForTestOn == 1 && module2MeasureDataSend == 0){
      
      //Switch rs485 transmit
      digitalWrite(DIR, 1);
      
      //Module 1 list
      for (int i = 0; i < module2MenuArrSize; i ++){
      
        if (module2TubesSw[i] == 1){
          
          //Send data package to slave 1
          RS485.write(2);
          RS485.write(7);
          RS485.write(i + 1);
          
          //Switch rs485 receive
          digitalWrite(DIR, 0);
          module2MeasureDataSend = 1;
          moduleCounter++;
          
        }
      
      }
      
    }
    delay(100);
    //End create Module 2 data package for measure data
    
    //Create Module 3 data package for measure data  
    if (module3ForTestOn == 1 && module3MeasureDataSend == 0){
      
      //Switch rs485 transmit
      digitalWrite(DIR, 1);
      
      //Module 1 list
      for (int i = 0; i < module3MenuArrSize; i ++){
      
        if (module3TubesSw[i] == 1){
          
          //Send data package to slave 1
          RS485.write(3);
          RS485.write(7);
          RS485.write(i + 1);
          
          //Switch rs485 receive
          digitalWrite(DIR, 0);
          module3MeasureDataSend = 1;
          moduleCounter++;
          
        }
      
      }
      
    }
    delay(100);
    //End create Module 3 data package for measure data

    savedDataCounter = moduleCounter + 1;
    testEnd = 0;
    
  }
  
  //Start get and save measured data
  while(moduleCounter >= 1){
  
    if(ET.receiveData()){
      
      if (measureData.ID == 1){
        
        delay(10);
          
        module1MeasuredData[0] = measureData.moduleSw - 1;
        module1MeasuredData[1] = measureData.measure1;
        module1MeasuredData[2] = measureData.measure2;
        module1MeasuredData[3] = measureData.measure3;
        module1MeasuredData[4] = measureData.measure4;
        moduleCounter--;
        savedDataCounter--;
          
      }
      
      if (measureData.ID == 2){
          
        delay(10);
        
        module2MeasuredData[0] = measureData.moduleSw - 1;
        module2MeasuredData[1] = measureData.measure1;
        module2MeasuredData[2] = measureData.measure2;
        module2MeasuredData[3] = measureData.measure3;
        module2MeasuredData[4] = measureData.measure4;
        moduleCounter--;
        savedDataCounter--;
          
      }

      if (measureData.ID == 3){
        
        delay(10);
        
        module3MeasuredData[0] = measureData.moduleSw - 1;
        module3MeasuredData[1] = measureData.measure1;
        module3MeasuredData[2] = measureData.measure2;
        module3MeasuredData[3] = measureData.measure3;
        module3MeasuredData[4] = measureData.measure4;
        moduleCounter--;
        savedDataCounter--;
          
      }

    }
   
  }
  //End get and save measured data
  
  while(savedDataCounter == 1){
    
    //Show measured data from module 1
    if (module1ForTestOn == 1){
    
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(module1TubesName[module1MeasuredData[0]]);
      lcd.setCursor(0, 1);
      lcd.print(module1MeasuredData[1]);
      lcd.print(" ");
      lcd.print(module1MeasuredData[2]);
      lcd.print(" ");    
      lcd.print(module1MeasuredData[3]);
      delay(3000);

    }
    
    //Show measured data from module 2
    if (module2ForTestOn == 1){
        
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(module2TubesName[module2MeasuredData[0]]);
      lcd.setCursor(0, 1);
      lcd.print(module2MeasuredData[1]);
      lcd.print(" ");
      lcd.print(module2MeasuredData[2]);
      lcd.print(" ");    
      lcd.print(module2MeasuredData[3]);
      delay(3000);
      
    }
    
    //Show measured data from module 3
    if (module3ForTestOn == 1){
        
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(module3TubesName[module3MeasuredData[0]]);
      lcd.setCursor(0, 1);
      lcd.print(module3MeasuredData[1]);
      lcd.print(" ");
      lcd.print(module3MeasuredData[2]);
      lcd.print(" ");    
      lcd.print(module3MeasuredData[3]);
      delay(3000);
    
    }
  
    delay(200);
  
  }

}
