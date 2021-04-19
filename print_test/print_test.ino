#include <LiquidCrystal.h>
LiquidCrystal lcd(12,11,5,4,3,2); // generates an instance in the lcd


void setup() {
  lcd.begin(16,2);
  Serial.begin(9600);

 
  String line0 = "Hej " + String(1);
  String line1 = "Hej " + String(2);
  myLCDprint(line0, line1);
}
void loop() {}


void myLCDprint(String line0, String line1) {
  lcd.setCursor(0,0);
  lcd.print(line0);
  lcd.setCursor(0,1);
  lcd.print(line1);
}







/*
char *someItems[]={"Item1","Item2","Item3", '\0'}; // Make sure a null string exists

void setup() {
  Serial.begin(9600);
  myFunction(someItems);
}

void loop(){}

void myFunction(char *items[]) {
  
  while (*items[0]) {
    Serial.println(*items++);
  } 
}*/
