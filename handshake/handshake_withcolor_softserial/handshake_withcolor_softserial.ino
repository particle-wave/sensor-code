
   
//Light libraries
#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
 #include <avr/power.h>
#endif

//math library
#include <math.h>

//Software serial initializing

#define rxPin 10
#define txPin 11

#include <SoftwareSerial.h>
SoftwareSerial mySerial(rxPin, txPin); // RX, TX
char delimiter = '\n';
String dataFlag = "PPM";
boolean startListening = false;

//Light initializing 
uint8_t dataPin  = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels

Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin);


const int light_low = 3;
const int light_high = 24;
const int numlights = 25;
const int steady_lights = 3;
int light_val;

//color variables
uint32_t good_color = 511; //RGB 24 bit color 0x00BFFF. 701?
uint32_t bad_color = 767; //0xFF4500 
uint32_t curr_color = good_color; 

char inChar; // Data received from the serial port
String stringVal;
float floatVal;
const int COmin = 200; //multiply manually by sigMultiplier
const int COmax = 800; //multiply manually by sigMultiplier
const int sigMultiplier = 1;
void setup() {

   strip.begin();
  // Update LED contents, to start they are all 'off'
  strip.show();

  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  mySerial.begin(9600); // Start serial communication at 9600 bps
 }
 void loop() {
  
   if (mySerial.available() > 0){
     inChar = mySerial.read();  
     if (inChar != delimiter && inChar != ',') //we are not reading the delimiter, so we must be reading data characters
     {
       stringVal+=inChar;
       Serial.println("Hi!");
     }
    else if(inChar != ',' && inChar != ' '){//we are reading the delimiter and have reached the end of the data point. Save it to floatVal, display it, and reset the stringVal buffer.
      if(!startListening){//check if we've received the data flag to start recording)
        if(stringVal == dataFlag){
          startListening = true;
        }
      }
      else{ //we've already seen the dataFlag, convert and display this value
        floatVal = stringVal.toFloat();
        light_val = map(floatVal * sigMultiplier, COmin, COmax, light_low, light_high);
        light_val = constrain(light_val, light_low, light_high);
        //update color
        curr_color = map(floatVal * sigMultiplier, COmin, COmax, good_color, bad_color);
        constrain(curr_color, good_color, bad_color);
        convertColor();
        update_light_height(); 
        Serial.println("Arduino read " + String(floatVal) + " Light val: " + String(light_val));  
        }
        stringVal = "";
        delay(30);
        }
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

void convertColor()
{
  byte redIntensity;
  byte greenIntensity;
  byte blueIntensity;

  if (curr_color <= 255)          // zone 1
  {
    redIntensity = 255 - curr_color;    // red goes from on to offcurr_colorcurr_color
    greenIntensity = curr_color;        // green goes from off to on
    blueIntensity = 0;             // blue is always off
  }
  else if (curr_color <= 511)     // zone 2
  {
    redIntensity = 0;                     // red is always off
    greenIntensity = 255 - (curr_color - 256); // green on to off
    blueIntensity = (curr_color - 256);        // blue off to on
  }
  else // color >= 512       // zone 3
  {
    redIntensity = (curr_color - 512);         // red off to on
    greenIntensity = 0;                   // green is always off
    blueIntensity = 255 - (curr_color - 512);  // blue on to off
  }
  
  //convert to hex
  curr_color = redIntensity;
  curr_color <<= 8;
  curr_color |= greenIntensity;
  curr_color <<= 8;
  curr_color |= blueIntensity;
}
