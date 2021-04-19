#include <LiquidCrystal.h>

LiquidCrystal lcd(12,11,5,4,3,2); // generates an instance in the lcd

const int switchPin = 13;

int switchState = 0;
int prevSwitchState = 0;
int meny = 0;


// Variabler som vi får tag på något sätt
String nextTime = "12:40";

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
    if (switchState == HIGH) {
      lcd.clear();

      
      switch(meny){
        case 0:

        String line0 = "Next:  " + nextTime;
        String line1 = "Menue";
        myLCDprint(line0, line1);
        break;

      }
    }
  }
  prevSwitchState = switchState;
}

void myLCDprint(String line0, String line1) {
  lcd.setCursor(0,0);
  lcd.print(line0);
  lcd.setCursor(0,1);
  lcd.print(line1);
}
