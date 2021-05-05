#define reedPin A0

void setup() {
  // declare the ledPin as an OUTPUT:
  Serial.begin(9600);
}

void loop() {
  delay(250);
  if(pollReed()){
    Serial.println("MAGNET PRESENT!");
  }
}

//returns true if magnet is present near reed switch
bool pollReed() {
  int value = analogRead(reedPin);
  //Serial.println("Value" + String(value));
  if(value < 500){
    return true;
  }
  else  {
    return false;
  }
}
