/*


*/
//Include libraries
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27,16,2);

// Constants
#define keyStart 3
#define keyUp 7
#define keyDown 4
#define keyBack 5
#define keySelect 6
#define testLed 8
#define doneLed 9
#define errorLed 10

const char* modulesMenu[] = {"Module 1", "Module 2", "Module 3"};
const char* module1TubesName[] = {"M1 Lamp 1", "M1 Lamp 2", "M1 Lamp 3"};
const char* module2TubesName[] = {"M2 Lamp 1", "M2 Lamp 2", "M2 Lamp 3"};
const char* module3TubesName[] = {"M3 Lamp 1", "M3 Lamp 2", "M3 Lamp 3"};

// Variables & arrays
int module1TubesSw[] = {0, 0, 0};
int module2TubesSw[] = {0, 0, 0};
int module3TubesSw[] = {0, 0, 0};

int modulesMenuPos = 0;
int moduleMenuPos = 0;
int selected = 0;
int inModuleMenu = 0;
int module1IsSelected = 0;
int module2IsSelected = 0;
int module3IsSelected = 0;
int defaultDisplay = 0;
int startKeyActive = 0;
int startShowCounter = 0;
int modulesMenuArrSize;
int module1MenuArrSize;
int module2MenuArrSize;
int module3MenuArrSize;


void setup() {
  
  // initialize the lcd 
  lcd.init();
 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("****************");
  lcd.setCursor (3,1);
  lcd.print("it-group4you!");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Tester, started");
  lcd.setCursor(0,1);
  lcd.print("Loading...");
  delay(2000);
  lcd.clear();  

  pinMode(keyStart, INPUT);
  pinMode(keyUp, INPUT);
  pinMode(keyDown, INPUT);
  pinMode(keyBack, INPUT);
  pinMode(keySelect, INPUT);
  
  modulesMenuArrSize = (sizeof(modulesMenu)/sizeof(int)) - 1;
  module1MenuArrSize = (sizeof(module1TubesName)/sizeof(int)) - 1;
  module2MenuArrSize = (sizeof(module2TubesName)/sizeof(int)) - 1;
  module3MenuArrSize = (sizeof(module3TubesName)/sizeof(int)) - 1;

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
    if (key2 && modulesMenuPos < modulesMenuArrSize) modulesMenuPos++;   
    else if (key1 && modulesMenuPos != 0) modulesMenuPos--;  
 
    lcd.print(modulesMenu[modulesMenuPos]);

}

void loop() {
  
  while(!startKeyActive){
      
    //Default display
    if (defaultDisplay){
      
      lcd.clear();
      defaultDisplay = 0;
    
    }
    
    lcd.setCursor(0,0);
    lcd.print("Menu");
    lcd.setCursor(0,1);
  
    // keyStart actions
    if (debounce(keyStart)){
      
      if (selectedModule()){
      
        startKeyActive = 1;
        startShowCounter = 1;
      
        //Switch on testLed
        digitalWrite(testLed,1);
      
      }else{
      
        //Error message: 1 module or more is not selected
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("1 module or more");
        lcd.setCursor(0,1);
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
  
      if (debounce(keyDown) && moduleMenuPos < module1MenuArrSize) moduleMenuPos++;   
      else if (debounce(keyUp) && moduleMenuPos != 0) moduleMenuPos--;
      
      lcd.setCursor(0,0);
      lcd.print(modulesMenu[modulesMenuPos]);
      
      lcd.setCursor(0,1);
      lcd.print(module1TubesName[moduleMenuPos]);
      
      if (module1TubesSw[moduleMenuPos] == 0) lcd.print(" ");
      else if (module1TubesSw[moduleMenuPos] == 1) lcd.print("*");    
      
      if (debounce(keySelect) && inModuleMenu == 1){
      
        if (module1TubesSw[moduleMenuPos] == 0){

          if(module1IsSelected == 1){
            
            //Error message: Module 1 is selected!
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Module 1 is");
            lcd.setCursor(0,1);
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
      
      if (debounce(keyDown) && moduleMenuPos < module2MenuArrSize) moduleMenuPos++;   
      else if (debounce(keyUp) && moduleMenuPos != 0) moduleMenuPos--;
      
      lcd.setCursor(0,0);
      lcd.print(modulesMenu[modulesMenuPos]);
      
      lcd.setCursor(0,1);
      lcd.print(module2TubesName[moduleMenuPos]);
      
      if (module2TubesSw[moduleMenuPos] == 0) lcd.print(" ");
      else if (module2TubesSw[moduleMenuPos] == 1) lcd.print("*");    
      
      if (debounce(keySelect) && inModuleMenu == 1){
      
        if (module2TubesSw[moduleMenuPos] == 0){

          if(module2IsSelected == 1){
            
            //Error message: Module 2 is selected!
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Module 2 is");
            lcd.setCursor(0,1);
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
  
      if (debounce(keyDown) && moduleMenuPos < module3MenuArrSize) moduleMenuPos++;   
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
            lcd.setCursor(0,0);
            lcd.print("Module 3 is");
            lcd.setCursor(0,1);
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
  
  //Start testing actions, show tested modules
  while(startShowCounter != 0 && startShowCounter < 4){
    
    int module1ForTestOn = 0;
    int module2ForTestOn = 0;
    int module3ForTestOn = 0;
    
    if (startShowCounter == 1){
      
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("****************");
      lcd.setCursor(0,1);
      lcd.print("Test started");
      
      delay(2000);
      lcd.clear();
      
    }
          
    //Module 1 status
    for (int i = 0; i < module1MenuArrSize; i ++){
    
      if (module1TubesSw[i] == 1){
      
        module1ForTestOn = 1;
        break;
      
      }
      
    }
    delay(10);
    
    if (module1ForTestOn){
        
      lcd.setCursor(0,0);
      lcd.print("Module 1 test");
      
      //Module 1 list
      for (int i = 0; i < module1MenuArrSize; i ++){
      
        if (module1TubesSw[i] == 1){
                
          lcd.setCursor(0,1);
          lcd.print("                ");
          delay(750);
          lcd.setCursor(0,1);
          lcd.print(module1TubesName[i]);
          delay(1250);
        
        }
      
      }
      
    }

    //Module 2 status
    for (int i = 0; i < module2MenuArrSize; i ++){
    
      if (module2TubesSw[i] == 1){
      
        module2ForTestOn = 1;
        break;
      
      }
    
    }
    delay(10);

    if (module2ForTestOn){
        
      lcd.setCursor(0,0);
      lcd.print("Module 2 test");
      
      //Module 2 list
      for (int i = 0; i < module2MenuArrSize; i ++){
      
        if (module2TubesSw[i] == 1){
                
          lcd.setCursor(0,1);
          lcd.print("                ");
          delay(750);
          lcd.setCursor(0,1);
          lcd.print(module2TubesName[i]);
          delay(1250);
        
        }
      
      }
      
    }

    //Module 3 status
    for (int i = 0; i < module3MenuArrSize; i ++){
    
      if (module3TubesSw[i] == 1){
      
        module3ForTestOn = 1;
        break;
      
      }
      
    }
    delay(10);

    if (module3ForTestOn){
        
      lcd.setCursor(0,0);
      lcd.print("Module 3 test");
      
      //Module 3 list
      for (int i = 0; i < module3MenuArrSize; i ++){
      
        if (module3TubesSw[i] == 1){
                
          lcd.setCursor(0,1);
          lcd.print("                ");
          delay(750);
          lcd.setCursor(0,1);
          lcd.print(module3TubesName[i]);
          delay(1250);
        
        }
      
      }
      
    }    
    
    startShowCounter++;    
    delay(1000);

  }

}
