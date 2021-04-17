#include <LiquidCrystal.h>

LiquidCrystal lcd(12,11,5,4,3,2); // generates an instance in the lcd

const int switchPin = 13;


int switchState = 0;
int prevSwitchState = 0;
int reply;
int counter = 0;
int meny = 0;

const int var = 0; // test variable

void setup() {
  lcd.begin(16,2);
  /*
  lcd.print("Next: 05:00");
  lcd.setCursor(0,1); // changes the Cursor to continue writing in the second row
  lcd.print("Bola de Cristal");*/
}

void loop() {

  switchState = digitalRead(switchPin);
  if (switchState != prevSwitchState) {
    if (switchState == LOW) {
      lcd.clear();
      switch(meny){
        case 0:
        LCDprint(["Hej"]);
        break;
      }
    }
  }
  prevSwitchState = switchState;
}

void LCDprint(int row1[]){
  char *line1 = "";
  for (int i=0; i < sizeof(row1); i++){
    line1 = line1 + String(row1[i]);
  }
  
  
  lcd.setCursor(0,0);
  lcd.print(line1);/*
  lcd.setCursor(0,1);
  lcd.print(line2);*/
}
