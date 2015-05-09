/*


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
  float measure1;
  float measure2;
  float measure3;

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

byte modulesMenuPos = 0;
byte moduleMenuPos = 0;
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

byte defaultDisplay = 0;
byte startKeyActive = 0;
byte startShowCounter = 0;

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
    if (key2 && modulesMenuPos < (modulesMenuArrSize -1)) modulesMenuPos++;   
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
  
      if (debounce(keyDown) && moduleMenuPos < (module1MenuArrSize -1)) moduleMenuPos++;   
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
      
      lcd.setCursor(0,0);
      lcd.print(modulesMenu[modulesMenuPos]);
      
      lcd.setCursor(0,1);
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
    lcd.print("Test started");
    
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
          
        }
      
      }
      
    }
    delay(10);
    //End create Module 1 data package 

    //Create Module 2 data package  
    if (module2ForTestOn == 1 && module2DataSend == 0){
      
      //Switch rs485 transmit
      digitalWrite(DIR, 1);
      
      //Module 1 list
      for (int i = 0; i < module2MenuArrSize; i ++){
      
        if (module2TubesSw[i] == 1){
          
          //Send data package to slave 1
          RS485.write(1);
          RS485.write(9);
          RS485.write(i + 1);
          
          //Switch rs485 receive
          digitalWrite(DIR, 0);
          module2DataSend = 1;
          
        }
      
      }
      
    }
    delay(10);
    //End create Module 2 data package

    //Create Module 3 data package  
    if (module3ForTestOn == 1 && module3DataSend == 0){
      
      //Switch rs485 transmit
      digitalWrite(DIR, 1);
      
      //Module 1 list
      for (int i = 0; i < module3MenuArrSize; i ++){
      
        if (module3TubesSw[i] == 1){
          
          //Send data package to slave 1
          RS485.write(1);
          RS485.write(9);
          RS485.write(i + 1);
          
          //Switch rs485 receive
          digitalWrite(DIR, 0);
          module3DataSend = 1;
          
        }
      
      }
      
    }
    delay(10);
    //End create Module 3 data package

    if (RS485.available() >= 2) {
    
      //Read first byte
      byte id = RS485.read();    
      
        //Module 1 check
        if (id == 1){
          
          byte module1Sw = RS485.read();
          
          if (module1TubesSw[module1Sw - 1]){
            
            //Switch rs485 transmit
            digitalWrite(DIR, 1);
          
            RS485.write(1);
            RS485.write(8);
            RS485.write(module1Sw);
            
            //Switch rs485 receive
            digitalWrite(DIR, 0);
            startShowCounter = 1;
            
          } else{
            
            digitalWrite(errorLed, 1);
            digitalWrite(testLed, 0);
            
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Module 1");
            lcd.setCursor(0, 1);
            lcd.print("ERROR");
            delay(3000);
            startKeyActive = 0;
            module1DataSend = 0;
            
          } 
          delay(10);
          
        }
        //End module 2 check

        //Module 2 check
        if (id == 2){
          
          byte module2Sw = RS485.read();
          
          if (module2TubesSw[module2Sw - 1]){
            
            //Switch rs485 transmit
            digitalWrite(DIR, 1);
          
            RS485.write(2);
            RS485.write(8);
            RS485.write(module2Sw);
            
            //Switch rs485 receive
            digitalWrite(DIR, 0);
            startShowCounter = 1;
            
          } else{
            
            digitalWrite(errorLed, 1);
            digitalWrite(testLed, 0);
            
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Module 2");
            lcd.setCursor(0, 1);
            lcd.print("ERROR");
            delay(3000);
            startKeyActive = 0;
            module2DataSend = 0;
            
          } 
          delay(10);
          
        }
        //End module 2 check

        //Module 3 check
        if (id == 3){
          
          byte module3Sw = RS485.read();
          
          if (module3TubesSw[module3Sw - 1]){
            
            //Switch rs485 transmit
            digitalWrite(DIR, 1);
          
            RS485.write(3);
            RS485.write(8);
            RS485.write(module3Sw);
            
            //Switch rs485 receive
            digitalWrite(DIR, 0);
            startShowCounter = 1;
            
          } else{
            
            digitalWrite(errorLed, 1);
            digitalWrite(testLed, 0);
            
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Module 3");
            lcd.setCursor(0, 1);
            lcd.print("ERROR");
            delay(3000);
            startKeyActive = 0;
            module3DataSend = 0;
            
          } 
          delay(10);
          
        }
        //End module 3 check
    
    }
    
  }
  //End test start, keyStart pressed
  
  //Show tested modules
  while(startShowCounter != 0 && startShowCounter < 4){
                  
    //Displayed if Module 1 is on  
    if (module1ForTestOn){
        
      lcd.setCursor(0, 0);
      lcd.print("Module 1 test");
      
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
      lcd.print("Module 2 test");
      
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
      lcd.print("Module 3 test");
      
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

  }
  //End show tested modules
  
  //Debug Start
  if(ET.receiveData()){

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(measureData.measure1);
  }
  //Debug end

}
