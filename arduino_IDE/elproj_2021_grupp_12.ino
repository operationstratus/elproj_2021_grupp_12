//#include <Arduino.h>


// Här har jag bara klistrat in koden från Crystal Ball

//include "LiquidCrystal.h"

#include <LiquidCrystal.h>


LiquidCrystal lcd(12,11,5,4,3,2); // generates an instance in the lcd

const int switchPin = 13;
const int contrastPin = 9;
int switchState = 0;
int prevSwitchState = 0;
int reply;

void setup() {
  lcd.begin(16,2);
  
  pinMode(contrastPin, OUTPUT);
  lcd.print("Next: 05:00");
  lcd.setCursor(0,1); // changes the Cursor to continue writing in the second row
  lcd.print("Bola de Cristal");
}

void loop() {
  analogWrite(contrastPin, 0);

  /*
  if (switchState != prevSwitchState) {
    if (switchState == LOW) {
      reply = random(8);
      lcd.clear(); // clears the writing
      lcd.setCursor(0,0);
      lcd.print("La bola dice:");
      lcd.setCursor(0,1);

      switch(reply){ // the program will enter the case assigned to the switch
        case 0:
        lcd.print("Si");
        break;
        case 1:
        lcd.print("Es probable");
        break;
        case 2:
        lcd.print("Ciertamente");
        break;
        case 3:
        lcd.print("Buenas perspectivas");
        break;
        case 4:
        lcd.print("No es seguro");
        break;
        case 5:
        lcd.print("Pregunta de nuevo");
        break;
        case 6:
        lcd.print("Ni idea");
        break;
        case 7:
        lcd.print("No");
        break;
      }
    }
  }*/
  

}




