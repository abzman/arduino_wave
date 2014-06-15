//Arduino 1.0+ Only!
//Arduino 1.0+ Only!

#include <Wire.h>
#include <ADXL345.h>
#include <WaveHC.h>
#include <WaveUtil.h>

ADXL345 adxl; //variable adxl is an instance of the ADXL345 library
  int x,y,z; 
  int z_stationary;
  int threshold = 23;
  boolean up = 0;
  boolean down = 0;
  int floor_delay = 1500;
  int blip_delay = 200;
  int LED = 6;
  
  
  SdReader card;    // This object holds the information for the card
FatVolume vol;    // This holds the information for the partition on the card
FatReader root;   // This holds the information for the volumes root directory
FatReader file;   // This object represent the WAV file for a pi digit or period
WaveHC wave;      // This is the only wave (audio) object, since we will only play one at a time
/*
 * Define macro to put error messages in flash memory
 */
#define error(msg) error_P(PSTR(msg))
  
  
  
  
void setup(){
  pinMode(LED,OUTPUT);
  Serial.begin(9600);
  adxl.powerOn();
  delay(100);
  adxl.readAccel(&x, &y, &z);
  z_stationary = z;
  
    if (!card.init()) {
    error("Card init. failed!");
  }
  if (!vol.init(card)) {
    error("No partition!");
  }
  if (!root.openRoot(vol)) {
    error("Couldn't open dir");
  }

  PgmPrintln("Files found:");
  root.ls();
  
  
}

void loop(){
  
  //Boring accelerometer stuff    
  adxl.readAccel(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
  if(z > (z_stationary + threshold))
  {
    delay(blip_delay);
      if(z > (z_stationary + threshold))
      {
        going_up();
        delay(floor_delay);
      }
  }
  if(z < (z_stationary - threshold))
  {
    delay(blip_delay);
      if(z < (z_stationary - threshold))
      {
        going_down();
        delay(floor_delay);
      }
  }
}

void going_up()
{
  if(down == 1)
  {
    digitalWrite(LED,LOW);
    if (wave.isplaying) {// already playing something, so stop it!
      wave.stop(); // stop it
    }
    Serial.println("stopping down");
    down = 0;
    delay(3000);
  }
  else
  {
    digitalWrite(LED,HIGH);
    playfile("TARDIS.WAV");
    Serial.println("going up");
    up = 1;
  }
}

void going_down()
{
  if(up == 1)
  {
    digitalWrite(LED,LOW);
        if (wave.isplaying) {// already playing something, so stop it!
      wave.stop(); // stop it
    }
    Serial.println("stopping up");
    up = 0;
    delay(3000);
  }
  else
  {
    digitalWrite(LED,HIGH);
        playfile("TARDIS.WAV");
    Serial.println("going down");
    down = 1;
  }
}

void error_P(const char *str) {
  PgmPrint("Error: ");
  SerialPrint_P(str);
  sdErrorCheck();
  while(1);
}
/*
 * print error message and halt if SD I/O error
 */
void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  PgmPrint("\r\nSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  PgmPrint(", ");
  Serial.println(card.errorData(), HEX);
  while(1);
}
/*
 * Play a file and wait for it to complete
 */
void playcomplete(char *name) {
  playfile(name);
  while (wave.isplaying);
  
  // see if an error occurred while playing
  sdErrorCheck();
}
/*
 * Open and start playing a WAV file
 */
void playfile(char *name) {
  if (wave.isplaying) {// already playing something, so stop it!
    wave.stop(); // stop it
  }
  if (!file.open(root, name)) {
    PgmPrint("Couldn't open file ");
    Serial.print(name); 
    return;
  }
  if (!wave.create(file)) {
    PgmPrintln("Not a valid WAV");
    return;
  }
  // ok time to play!
  wave.play();
}
