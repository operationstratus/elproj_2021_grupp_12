//Det här programmet baseras på information och exempelkod från 
//https://www.arduino.cc/en/reference/SD
//och underliggande sidor

/*
  SD card test
  This example shows how use the utility libraries on which the
  SD library is based in order to get info about your SD card.
  Very useful for testing a card when you're not sure whether its working or not.
  Pin numbers reflect the default SPI pins for Uno and Nano models
  
  The circuit:
    SD card attached to SPI bus as follows:
 ** SDO - pin 11 on Arduino Uno/Duemilanove/Diecimila
 ** SDI - pin 12 on Arduino Uno/Duemilanove/Diecimila
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila
 ** CS - depends on your SD card shield or module.
        Pin 10 used here for consistency with other Arduino examples

  created  28 Mar 2011
  by Limor Fried
  modified 24 July 2020
  by Tom Igoe

*/
// include the SD library:
#include <SPI.h>
#include <SD.h>

// set up variables using the SD utility library functions:

/*
Sd2Card card;
SdVolume volume;
SdFile root;
*/

// change this to match your SD shield or module;
// Default SPI on Uno and Nano: pin 10
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN


void setup() {

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // wait for serial port to connect. Needed for native USB port only
  }

  Serial.begin("test");

  /*
  Serial.print("\nInitializing SD card...");
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("ERR: Cannot read SD card");

  } else {
    Serial.println("SD card connected");
  }
  
  // print the type of card
  Serial.println();
  Serial.print("Card type:         ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1);
  }
  
  Serial.print("Clusters:          ");
  Serial.println(volume.clusterCount());
  Serial.print("Blocks x Cluster:  ");
  Serial.println(volume.blocksPerCluster());
  Serial.print("Total Blocks:      ");
  Serial.println(volume.blocksPerCluster() * volume.clusterCount());
  Serial.println();
  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("Volume type is:    FAT");
  Serial.println(volume.fatType(), DEC);
  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
  Serial.print("Volume size (Kb):  ");
  Serial.println(volumesize);
  Serial.print("Volume size (Mb):  ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Gb):  ");
  Serial.println((float)volumesize / 1024.0);
  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  
  root.openRoot(volume);
  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
  root.close();
  */
  String text = readFromSD("alarms.txt");
  Serial.println("reading...");
  Serial.println(text);
  Serial.println("copying...");
  writeToSD("copy.txt", text);
  text = readFromSD("copy.txt");
  Serial.println("reading copy...");
  Serial.println(text);

  writeToSD("write.txt", "test test test");
}

String readFromSD(String fileName){
  Serial.println("ATTEMPTING CARD READ");
  SD.begin();
  File readFile = SD.open(fileName);
  if(readFile){
    String res = "";
    while(readFile.available()){
      char nextChar = char(readFile.read());
      if(nextChar == '#'){
        Serial.println("EOF; breaking");
        break;
      }
      res += nextChar;
    }
    readFile.close();
    return res;
  }
  else{
    Serial.println("Err: cannot open file (read): "+fileName);
  }
}

void writeToSD(String fileName, String content){
  content += '#';
  SD.begin();
  SD.remove(fileName);
  File writeFile = SD.open(fileName, FILE_WRITE);
  if(writeFile){
    for(int i = 0; i < content.length(); i++ ) {
      writeFile.print(content[i]);
      if(content[i] == '#'){
        break;
      }
    }
  }
  else {
    Serial.println("Err: cannot open file (write): "+fileName);
  }
  writeFile.close();
}

void loop(void) {
}
