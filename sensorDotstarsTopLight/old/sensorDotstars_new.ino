/* TIPS: Make sure that the echem board and Arduino share a common ground. 
*/

//Light libraries
#include <Adafruit_DotStar.h>
#include "SPI.h" // Comment out this line if using Trinket or Gemma
//#include "Adafruit_WS2801.h"
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif

//time library - At the moment this doesn't involve synching with an outside time, so it is just an internal clock
#include <Time.h>

//math library
#include <math.h>

//Light initializing 
#define NUMPIXELS 60 // Number of LEDs in strip
#define DATAPIN    4    // Green wire on DotStar
#define CLOCKPIN   5    // Yellow wire on DotStar
Adafruit_DotStar strip = Adafruit_DotStar(
  NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);


const int numlights = 60;
const int steady_lights = 3;
const int light_low = 3;
const int light_high = numlights - 1;
int light_val = 0;
int light_direction = 1;
uint8_t change = 1;
uint32_t curr_color = 0x4B0082; 

//Software serial initializing

#define rxPin 8 //White cable from sensor
#define txPin 9 //Infrequently used

//Sfotware serial 
#include <SoftwareSerial.h>
SoftwareSerial mySerial(rxPin, txPin); // RX, TX
char delimiter = '\n';
String dataFlag = "PPM";
boolean startListening = false;
int fieldsOfInterest[2] = [4,9];
#define lightField 9 //number for the field value that will be mapped to light display. Field 9 is unfiltered CO PPM.



char inChar; // Data received from the serial port
String stringVal;
float floatVal;
const int COmin = 400; //multiply manually by sigMultiplier
const int COmax = 1300; //multiply manually by sigMultiplier
const int sigMultiplier = 1000;

void setup() {
   strip.begin();
  // Update LED contents, to start they are all 'off'
  strip.show();
  
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  Serial.begin(9600);
  mySerial.begin(9600); // Start serial communication at 9600 bps

  setTime(0);
 }
 void loop() {
   //test();
    int fieldCount = 0;
    if (mySerial.available() > 0){
      //first, print Arduino timestamp
      Serial.print(now()); //Print time in seconds since activation
      Serial.print(",");
      inChar = mySerial.read();
      //WRITE TIME PLUS COMMA TO LOG FILE
      //WRITE CHAR TO LOG FILE
      
      if (inChar == ","){ //if we see a comma, we've read a value
            for(int i = 0; i < fieldsOfInterest.length(); i ++){
              if (fieldCount == fieldsOfInterest[i]){
                Serial.print(stringVal);
                Serial.print(",");
              }
            if (fieldCount == lightField){ //we are reading a new value to map to the lights
              floatVal = stringVal.toFloat();
              light_val = map(floatVal * sigMultiplier, COmin, COmax, light_low, light_high);
              light_val = constrain(light_val, light_low, light_high);
              stringVal = "";  
            }
            fieldCount++;
            }           
      }
    
      update_light_height(); 
      Serial.println("Arduino read " + String(floatVal) + " Light val: " + String(light_val));
      
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

void test(){
  if(light_val > light_high){
    light_direction = -1;
  }
  else if(light_val < 0){
    light_direction = 1;
  }
  light_val += change * light_direction;
  Serial.print(light_val);
  Serial.print(" ");
  Serial.println(light_direction);
  update_light_height();
}


