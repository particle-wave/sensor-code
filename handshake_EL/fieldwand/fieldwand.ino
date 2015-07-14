 
//Light libraries
#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif

//math library
#include <math.h>

//Light initializing 
uint8_t dataPin  = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels

Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin);

const int light_low = 3;
const int light_high = 24;
const int numlights = 25;
const int steady_lights = 3;
int light_val;
uint32_t curr_color = 511; 

//Sensor reading initializing
int COmin = 1000; //change to something that makes sense multipled by sigMultiplier
int COmax = -1000; 
const int sigMultiplier = 100;
char inChar; // Data received from the serial port
String stringVal;
int COVal;



void setup() {
   strip.begin();// Update LED contents, to start they are all 'off'
  strip.show();
   Serial.begin(9600); // Start serial communication at 9600 bps
   //establishContact(); 
 }
 
 void loop() {
    /*
   if (Serial.available() > 0){
     inChar = Serial.read(); 
     if (inChar != '\n')
     {
       stringVal+=inChar;
     }
     */
     COVal = int(stringVal.toFloat()*sigMultiplier);        
   }
   else {
    stringVal = "";
    fieldTune();
    light_val = map(COVal, COmin, COmax, light_low, light_high);
    light_val = constrain(light_val, light_low, light_high);
    update_light_height(); 
    Serial.println("Arduino read " + String(COVal) + " Light val: " + String(light_val));  
   }  
}

void fieldTune(){
  if (COVal > COmax){
    COmax = COVal;
  }
  if (COVal < COmin){
    COmin = COVal;
  }  
}
void establishContact(){
  while (Serial.available()<=0){
    Serial.println("A");
    delay(10);
  }
}

void update_light_height()
{ //turn all except 3 off
  for(int i=0; i <= light_high; i++){
    strip.setPixelColor(i,0x000000);
  }
  for(int i=light_high; i >light_high - steady_lights; i--){
    strip.setPixelColor(i,curr_color);
  }
  //update to new value
  for (int i=light_high - steady_lights; i > light_high - steady_lights - light_val; i--)
 {
  strip.setPixelColor(i, curr_color);
 }
 strip.show();
}
